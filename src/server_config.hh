#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "toml/toml.hpp"

class ServerConfig
{
private:
  std::string server_name_;
  std::string port_;
  std::string ip_;
  std::string root_dir_;

  std::map<std::string, std::string> configurations_;
  std::map<std::string, std::string> error_;

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
  void set_server_name(std::string& server_name);
  void set_port(std::string& port);
  void set_ip(std::string& ip);
  void set_root_dir(std::string& root_dir);
  void insert_to_error(std::string& a, std::string& b);
  void insert_to_configuration(std::string a, std::string& b);
  /* Print this server configuration */
  void print();
};

#include "server_config.hxx"
