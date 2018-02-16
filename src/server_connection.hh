#pragma once

#include "server_config.hh"

class ServerConnection
{
  public:
    ServerConnection()
    {}
    ServerConnection(ServerConfig& server);
    ~ServerConnection();
    ServerConfig& get_config();
    int get_socket() const;
    bool set_connection(struct epoll_event& event, int epoll_fd);
    void set_socket(int& socket);
  private:
    ServerConfig server_;
    int socket_;
};

#include "server_connection.hxx"
