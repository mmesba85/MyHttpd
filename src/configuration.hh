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
  /* Constructors */
  Configuration(const std::string filename);
  ~Configuration();

  /* Read the stream_ and fill the configuration */
  void fill_configuration();

  /* Print the configuration */
  void print();
};

#include "configuration.hxx"
