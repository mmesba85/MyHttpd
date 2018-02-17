#pragma once

#include <fstream>

class ServerConfig;
class Response;

class Request
{
  public:

    /**
    ** \brief Class getters for the attributs
    **/
    std::string& get_version();
    std::string& get_url();
    std::string& get_host();
    std::string& get_type();
    std::string& get_path();
    std::string& get_client_ip();
    bool is_connected();

    /**
    ** \brief check the request's method
    ** \return true if the method is right
    */
    bool bad_method() const;

    /**
    ** \brief tell if the acces to the resource is forbidden
    ** \return true if forbidden, false otherwise
    */
    bool forbidden() const;

    /**
    ** \brief check if resource requested exists
    ** \brief the server configuration to search the resource
    */
    bool not_found() const;

    /**
    ** \brief check the client's request syntax
    ** \param the client's request
    ** \return true if the syntax is right
    **/
    static bool check_client_request(std::string& request);

    /**
    ** \brief check if the first request's line syntax is right
    ** \param the first line
    ** \return true if the syntax is right
    **/
    static bool check_request_line(std::string& line);
    
    /**
    ** \brief get the method from the request's line
    ** \param client's request
    ** \return the method
    **/
    static std::string get_method(std::string& request);
    
    /**
    ** \brief process the request and does the necessary checks
    ** set the response's status code
    ** \param the reponse
    ** \return the string associated to the response
    **/
    virtual std::string process_request(Response& rp) = 0;
   
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
