#pragma once

#include "server_config.hh"

class Response;
class Request
{
  public:
    bool bad_method() const;
    bool forbidden(ServerConfig& config) const;
    bool not_found(ServerConfig& config) const;

    /**
     * \brief extract the string corresponding to the requested resource with no
     * parameters
     * \param file the string in which the resource is placed
     */
    void extract_resource(std::string& file);
    
    std::string& get_version();
    std::string& get_url();
    static bool check_request_line(std::string& line);
    static bool check_client_request(std::string request);
    static std::string get_method(std::string& request);
    virtual std::string process_request(Response& rp, ServerConfig& config) = 0;
  protected:
    std::string url_;
    std::string version_;
    std::string host_;
    bool connected_;
};

#include "request.hxx"
