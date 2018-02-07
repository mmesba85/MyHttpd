#pragma once

class Request
{
  public:
    Request(std::string url, int version, std::string host, 
      bool connected);
    bool check_request() const;
    bool check_version() const;
    bool check_url() const;
  protected:
    std::string url_;
    int version_;
    std::string host_;
    bool connected_;
};

#include "request.hxx"