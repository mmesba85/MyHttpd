#pragma once

#include "server_config.hh"
#include "threadpool/thread-pool.hh"

class ServerConnection
{
  public:
    ServerConnection();
    ~ServerConnection();
    ThreadPool& get_pool();
    int get_socket() const;
    bool set_connection(struct epoll_event& event, int epoll_fd);
  private:
    ServerConfig server_;
    ThreadPool th_pool_;
    int socket_;
};

#include "server_connection.hxx"