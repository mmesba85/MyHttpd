#pragma once

#include "threadpool/thread-pool.hh"
#include "server_config.hh"

class ServerConnection
{
  public:
    ServerConnection()
    {}
    ServerConnection(ServerConfig& server);
    ~ServerConnection();
   // ThreadPool* get_pool();
    ServerConfig& get_config();
    int get_socket() const;
    bool set_connection(struct epoll_event& event, int epoll_fd);
    void set_socket(int& socket);
  private:
    ServerConfig server_;
  //  ThreadPool th_pool_;
    int socket_;
};

#include "server_connection.hxx"
