#pragma once

#include <ifstream>

#include "server_config.hh"
#include "toml.hpp"

class Configuration
{
private:
  std::vector<ServerConfig> list_server_;
  bool dry_run_;
  std::string log_file_;
  std::ifstream stream_;

public:
  Configuration (const std::string filename);
  virtual ~Configuration ();
  void fill_configuration();
};

#include "configuration.hxx"
