#pragma once

class Request
{
  public:
    Request(std::string url, int version, std::string host, 
      bool connected);
    bool check_request() const;
    bool check_version() const;
    bool check_url() const;
    static bool check_request_line(std::string line);
    static bool check_client_request(std::string request);
  protected:
    std::string url_;
    int version_;
    std::string host_;
    bool connected_;
};

#include "request.hxx"