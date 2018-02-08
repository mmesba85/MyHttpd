#pragma once

#include "response.hh"
#include "server_config.hh"

class Request
{
  public:
    bool bad_method() const;
    bool forbidden(ServerConfig& config) const;
    bool not_found(ServerConfig& config) const;
    std::string& get_version();
    static bool check_request_line(std::string& line);
    static bool check_client_request(std::string request);
    static std::string get_method(std::string& request);
    //virtual std::string process_request(Response& rp, ServerConfig& config) const = 0;
  protected:
    std::string url_;
    std::string version_;
    std::string host_;
    bool connected_;
};

#include "request.hxx"