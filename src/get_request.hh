#pragma once

#include "request.hh"

class GETRequest : public Request
{
  public:
    
    /**
    ** \brief GETRequest constructor
    **/
    GETRequest(std::string& request, std::string& ip, 
      const ServerConfig& config);

    /**
    ** \brief process the request and do the chekings
    ** \param the right response
    ** \return the string's reponse
    **/
    std::string process_request(Response& rp) override;
};

#include "get_request.hxx"
