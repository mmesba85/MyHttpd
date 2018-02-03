#ifndef SERVERCONNEXION_HH
#define SERVERCONNEXION_HH

#include "server_config.hh"

class ServerConnection
{
  public:
    ServerConnection();
    int get_socket() const;
    bool set_connection(struct epoll_event& event, int epoll_fd);
   // ~ServerConnection();
  private:
    ServerConfig server_;
    int socket_;
};

#include "server_connexion.hxx"

#endif /* SERVERCONNEXION_HH */