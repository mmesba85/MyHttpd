#pragma once

#include "server_config.hh"

ServerConfig::ServerConfig()
{
}

ServerConfig::ServerConfig(std::string name, std::string ip, std::string port,
  std::string root_dir)
  : server_name_(name), ip_(ip), port_(port), root_dir_(root_dir)
{
}

ServerConfig::~ServerConfig()
{
}
