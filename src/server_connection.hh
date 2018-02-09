#pragma once

#include "server_config.hh"
#include "response.hh"
#include "request.hh"
#include "threadpool/thread-pool.hh"

class ServerConnection
{
  public:
    ServerConnection();
    ~ServerConnection();
    ThreadPool& get_pool();
    ServerConfig& get_config();
    int get_socket() const;
    bool set_connection(struct epoll_event& event, int epoll_fd);
  private:
    ServerConfig server_;
    ThreadPool th_pool_;
    int socket_;
};

#include "server_connection.hxx"
