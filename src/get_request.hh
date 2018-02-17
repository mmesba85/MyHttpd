#pragma once

#include "request.hh"

class GETRequest : public Request
{
  public:
    GETRequest(std::string& request, std::string& ip, 
      const ServerConfig& config);
    std::string process_request(Response& rp) override;
};

#include "get_request.hxx"
