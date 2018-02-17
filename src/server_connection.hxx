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

ServerConnection::ServerConnection(ServerConfig& server)
: server_(server)
{}


ServerConnection::~ServerConnection()
{
  //close(socket_);
}

bool ServerConnection::set_connection(struct epoll_event& event, 
  int epoll_fd)
{
  struct sockaddr in_addr;
  socklen_t len = sizeof(in_addr);
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

ServerConfig& ServerConnection::get_config()
{
  return server_;
}

void ServerConnection::set_socket(int socket)
{
  socket_ = socket;
}
