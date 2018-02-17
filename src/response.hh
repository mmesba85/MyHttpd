#pragma once

#include <string>

class ServerConfig;
class Request;

class Response
{
  public:
    /**
    ** \brienf Response's constructors
    **/
    Response();
    Response(const std::string& version);

    /**
    ** \brief set the status_code
    ** \param the new status_code
    **/
    void set_code(const std::string& code);

    /**
    ** \brief set the version
    ** \param the new version
    **/
    void set_version(const std::string& version);

    /**
    ** \brief get the code_status
    ** \return the code status
    **/
    std::string& get_code();

    /**
    ** \brief build the right response from the request
    ** \param the request and the server's configuration
    ** \return the response
    **/
    std::string build_response(Request& request, const ServerConfig& config);

    /**
    ** \brief build the right response when there is a syntax error
    ** \return the response
    **/
    std::string build_response();
    
    /**
    ** \brief send the header's response
    ** \param the request, the server's configuration and it's sockets's
    ** descriptor
    ** \return 0 if ok, -1 otherwise
    **/
    int process_response(Request& request, ServerConfig& config, int fd);

    /**
    ** \brief send the header's response in case there is a syntax error
    ** \param the request, the server's sockets's
    ** descriptor
    ** \return 0 if ok, -1 otherwise
    **/
    int process_response(ServerConfig& config, int fd);

    /**
    ** \brief get the file descriptor that would be sent
    ** \param the server's configuration and the file's name
    ** \return 0 if ok, -1 otherwise
    **/
    int get_file_dscr(const ServerConfig& config, const std::string& file_name);

    /**
    ** \brief send the requested file
    ** \param the server's socket descriptor, the file's descriptor and 
    ** the response
    ** \return 0 if ok, -1 otherwise
    **/
   int send_response(int fd, int file_fd, ServerConfig& config,
            std::string& response, bool cgi);
  private:
    std::string version_;
    std::string status_code_;
    std::string reason_phrase_;
};

#include "response.hxx"
