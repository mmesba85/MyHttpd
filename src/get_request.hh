#pragma once

#include "request.hh"
#include "response.hh"

class GETRequest: public Request 
{
  public:
    GETRequest(std::string request);
    std::string process_request(Response& rp, ServerConfig& config);
};

#include "get_request.hxx"