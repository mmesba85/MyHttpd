#pragma once

#include <string>

#include "server_config.hh"

class Request;
class Response
{
  public:
    Response();
    Response(const std::string& version);
    std::string build_response(Request& request);
    std::string build_response();
    void set_code(const std::string& code);
    void set_version(const std::string& version);
    int send_data(Request& request, ServerConfig& config, int fd);
    std::string& get_code();
  private:
    std::string version_;
    std::string status_code_;
    std::string reason_phrase_;
};

#include "response.hxx"
