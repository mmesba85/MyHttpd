#pragma once

#include "request.hh"

class GETRequest : public Request
{
  public:
    GETRequest(std::string request, std::string& ip);
    std::string process_request(Response& rp, const ServerConfig& config) override;
};

#include "get_request.hxx"
