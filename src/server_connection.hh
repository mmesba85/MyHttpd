#pragma once

#include "server_config.hh"

class ServerConnection
{
  public:
    
    /**
    ** \brief default constructor
    **/
    ServerConnection()
    {}
    
    /** 
    ** \brief Construct the ServerConnection with the right server's config
    **/
    ServerConnection(ServerConfig& server);
    /**
    ** \brief Class's desctructor
    **/
    ~ServerConnection();

    /**
    ** \brief configuraion getter
    ** \return server's configuration
    **/
    ServerConfig& get_config();
    
    /**
    ** \brief socket getter
    ** \return server's socket
    **/
    int get_socket() const;

    /**
    ** \brief set the connection with the clien
    ** \param the epoll_event of the client and the epoll descriptor 
    ** of the server
    ** \return true if the connection is set
    **/
    bool set_connection(struct epoll_event& event, int epoll_fd);
    
    /**
    ** \brief set the socket descriptor
    ** \param the socket's descriptor
    **/
    void set_socket(int socket);

  private:
    ServerConfig server_; /* server's configuration */
    int socket_; /* server's socket */
};

#include "server_connection.hxx"
