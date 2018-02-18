#pragma once

#include "request.hh"
#include "server-config.hh"

class GETRequest : public Request
{
  public:

    /**
    ** \brief GETReqiest constructor
    **/
    GETRequest(std::string& request, std::string& ip,
      ServerConfig& config);

    /**
    ** \brief process the request and do the chekings
    ** \param the right response
    ** \return the string's reponse
    **/
    std::string process_request(Response& rp) override;
};
