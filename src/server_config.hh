#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "toml.hpp"

class ServerConfig
{
private:
  std::map<std::string, std::string> configurations_;
  std::string log_file_;

  std::string server_name_;
  std::string port_;
  std::string ip_;
  std::string root_dir_;

public:
  ServerConfig ();
  virtual ~ServerConfig ();
  // std::map<std::string, std::string>& get_configurations();
  // void set_configurations(std::map<std::string, std::string>& configuration);
  // std::string& get_server_name();
  // std::string& get_port();
  // std::string& get_ip();
  // std::string& get_root_dir();

};
