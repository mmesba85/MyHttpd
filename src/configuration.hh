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

  std::string& get_log_file();
  int fill_configuration();

  /* Getter for the list_server */
  std::vector<ServerConfig>& get_list();

  /* Print the configuration */
  void print();
};

#include "configuration.hxx"
