#pragma once

#include <fstream>

#include "server_config.hh"

class Response;

class Request
{
  public:
    bool bad_method() const;

    /**
    ** \brief tell if the acces to the resource is forbidden
    ** \param the server configuration where searching the resource
    ** \return true if forbidden, false otherwise
    */
    bool forbidden(ServerConfig& config) const;
    /**
    ** \brief check if resource requested exists
    ** \brief the server configuration to search the resource
    ** \return true if not founded, false otherwise
    */
    bool not_found(ServerConfig& config) const;

    /**
    ** \brief extract the string corresponding to the requested resource with no
    ** parameters
    ** \return the string correspondint to the resource
    */
    std::string extract_resource_path(const ServerConfig& ) const;
    
    std::string& get_version();
    static bool check_request_line(std::string& line);
    static bool check_client_request(std::string request);
    static std::string get_method(std::string& request);
    virtual std::string process_request(Response& rp, ServerConfig& config) = 0;

    //for  tests purposes
    void init(std::string&& url, std::string&& version, std::string&& host, bool connected);
  protected:
    std::string url_;
    std::string version_;
    std::string host_;
    bool connected_;
};

#include "request.hxx"
