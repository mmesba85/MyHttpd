#pragma once

#include <string>

class Request;
class Response
{
  public:
    Response();
    std::string build_response(Request& request, const ServerConfig& conf);
    std::string build_response();
    void set_code(const std::string& code);
    void set_version(const std::string& version);
    std::string& get_code();
  private:
    std::string version_;
    std::string status_code_;
    std::string reason_phrase_;
};

#include "response.hxx"
