#ifndef SERVERCONNEXION_HXX
#define SERVERCONNEXION_HXX

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <exception>
#include "server_connexion.hh"

ServerConnection::ServerConnection()
{
  server_ = ServerConfig("test", "4343", "127.0.0.1", "/tmp");
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sin;
  if(sock < 0)
    throw std::invalid_argument("error socket creation");
  sin.sin_family = AF_INET;
  std::string::size_type sz;
  int port = std::stoi(server_.get_port(), &sz);
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(sock, (struct sockaddr *)&sin, sizeof sin) == -1)
  {
    perror("bind");
    throw std::invalid_argument("error bind");
  }
  if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
    throw std::invalid_argument("error non_blocking");
  socket_ = sock;
}

bool ServerConnection::set_connection(struct epoll_event& event, 
  int epoll_fd)
{
  struct sockaddr in_addr;
  socklen_t len = sizeof(in_addr);
  int fd = accept(socket_, &in_addr, &len);
  if(fd == -1)
    return false;
  if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    throw std::invalid_argument("error non_blocking");
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    return false;
  return true;
}

int ServerConnection::get_socket() const
{
  return socket_;
}
#endif /* SERVERCONNEXION_HXX */