#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cstdio>
#include <mutex>
#include <memory>

#define _SVID_SOURCE

#include "toml/toml.hpp"

class Request;

class ServerConfig
{
private:
  std::string server_name_;
  std::string port_;
  std::string ip_;
  std::string root_dir_;
  
  std::shared_ptr<std::mutex> pipe_lock;
  std::map<int, FILE*> pipes;

  std::map<std::string, std::string> configurations_;
  std::map<std::string, std::string> error_;
  std::map<std::string, std::string> proxy_pass_;
  
  void update_cgi_env(Request& request) const;
  /**
  ** \brief parse the entity-header responses of the CGI script and return a map of
  ** header fields/values
  ** \param file the file corresponding to the CGI response
  ** \return a map containing all hears fields/values or an empty map if no
  ** header
  */
  void parse_cgi_headers(std::map<std::string, std::string>& map, FILE* file);

  /**
  ** \brief fill the string with the entity headers
  ** \param headers the map of headers
  ** \param into the string to fill
  */
  void fill_with_header(std::map<std::string, std::string>& headers,
          std::string& into) const;
public:
  /* Constructors */
  ServerConfig();
  ServerConfig(std::string name, std::string ip, std::string port,
    std::string root_dir);
  virtual ~ServerConfig();

  /* Getter and setters */
  std::string get_port() const;
  std::string get_ip() const;
  std::string get_name() const;
  std::string get_root_dir() const;
  std::map<std::string, std::string> get_error() const;

  void set_server_name(std::string& server_name);
  void set_port(std::string& port);
  void set_ip(std::string& ip);
  void set_root_dir(std::string& root_dir);
  void insert_to_error(std::string& a, std::string& b);
  void insert_to_configuration(std::string a, std::string& b);
  void insert_to_proxy_pass(std::string& a, std::string& b);
  /* Print this server configuration */
  void print();
  std::string& get_server_name();
  bool is_cgi(Request& request) const;
  int process_cgi(Request& request, std::string& rep_begin);
  void cancel(int fd);
};

#include "server_config.hxx"
