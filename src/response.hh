#pragma once

#include <string>

#include "server_config.hh"

class Request;
class Response
{
  public:
    Response();
    Response(const std::string& version);
    std::string build_response(Request& request, const ServerConfig& config);
    std::string build_response();
    void set_code(const std::string& code);
    void set_version(const std::string& version);
    int process_response(Request& request, const ServerConfig& config, int fd);
    int process_response(const ServerConfig& config, int fd);
    int get_file_dscr(const ServerConfig& config, const std::string& file_name);
    int send_response(int fd, int file_fd, std::string& response);
    std::string& get_code();
  private:
    std::string version_;
    std::string status_code_;
    std::string reason_phrase_;
};

#include "response.hxx"
