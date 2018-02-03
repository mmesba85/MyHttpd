#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "toml/toml.hpp"

class ServerConfig
{
private:
  std::map<std::string, std::string> configurations_;
  std::string log_file_;
  bool dry_run_;

  std::string server_name_;
  std::string port_;
  std::string ip_;
  std::string root_dir_;

public:
  ServerConfig();
  ServerConfig(std::string name, std::string ip, std::string port,
    std::string root_dir);
  virtual ~ServerConfig();
  // std::map<std::string, std::string>& get_configurations();
  // void set_configurations(std::map<std::string, std::string>& configuration);
  // std::string& get_server_name();
  // std::string& get_port();
  // std::string& get_ip();
  // std::string& get_root_dir();

};

#include "server_config.hxx"
