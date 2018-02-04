#pragma once

#include <fstream>

#include "server_config.hh"
#include "toml/toml.hpp"

class Configuration
{
private:
  std::vector<ServerConfig> list_server_;
  bool dry_run_;
  std::string log_file_;
  std::ifstream stream_;

public:
  Configuration(const std::string filename);
  ~Configuration();
  void fill_configuration();
  void print();
};

#include "configuration.hxx"
