#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <exception>
#include <system_error>
#include <cerrno>
#include <string>
#include "server_connection.hh"

ServerConnection::ServerConnection()
{
  server_ = ServerConfig("tests", "4242", "127.0.0.1", "./tests");
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if(sock < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "error socket creation.");  
  }

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  std::string::size_type sz;
  int port = std::stoi(server_.get_port(), &sz);
  sin.sin_port = htons(port);
  const char* ip = server_.get_ip().c_str();
  sin.sin_addr.s_addr = inet_addr(ip);
  
  int reuse = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (
    const char*)&reuse, sizeof(reuse)) < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "setsockopt(SO_REUSEPORT) failed.");
  }

  if(bind(sock, (struct sockaddr *)&sin, sizeof sin) == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "bind failed.");
  }

  if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "fcntl failed.");
  }  

  socket_ = sock;
}

ServerConnection::~ServerConnection()
{
  close(socket_);
  th_pool_.destroy();
}

bool ServerConnection::set_connection(struct epoll_event& event, 
  int epoll_fd)
{
  struct sockaddr in_addr;
  socklen_t len = sizeof(in_addr);
  std::cout << "** " << socket_ << std::endl;
  int fd = accept(socket_, &in_addr, &len);

  if(fd == -1)
  {
    if(errno == EAGAIN || errno == EWOULDBLOCK)
    {
      return false;
    }
    else
    {
      close(fd);
     /* std::error_code ec(errno, std::generic_category());
      throw std::system_error(ec, "No connections are present to be accepted."); */
      return false;   
    }
  }
  
  if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
  {
    close(fd);
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "fcntl failed.");
  }

  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
  {
    close(fd);
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "epoll_ctl failed.");
  }
  
  return true;
}

int ServerConnection::get_socket() const
{
  return socket_;
}

ThreadPool& ServerConnection::get_pool()
{
  return th_pool_;
}

ServerConfig& ServerConnection::get_config()
{
  return server_;
}
