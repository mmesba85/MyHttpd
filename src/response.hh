#pragma once

#include "request.hh"

class Response
{
  public:
    Response();
    std::string build_response(Request& request);
    std::string build_response();
    void set_code(const std::string& code);
    void set_version(const std::string& version);
  private:
    std::string version_;
    std::string status_code_;
    std::string reason_phrase_;
};

#include "response.hxx"