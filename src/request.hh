#pragma once

#include <fstream>

class ServerConfig;
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
    bool forbidden() const;
    /**
    ** \brief check if resource requested exists
    ** \brief the server configuration to search the resource
    ** \return true if not founded, false otherwise
    */
    bool not_found() const;

    std::string& get_version();
    std::string& get_url();
    std::string& get_host();
    std::string& get_type();
    std::string& get_path();
    bool is_connected();
    static bool check_request_line(std::string& line);
    static bool check_client_request(std::string& request);
    static std::string get_method(std::string& request);
    virtual std::string process_request(Response& rp) = 0;
    std::string& get_client_ip();

  protected:
    std::string url_;
    std::string path_;
    std::string type_;
    std::string version_;
    std::string host_;
    std::string client_ip_;
    bool connected_;
};

#include "request.hxx"
