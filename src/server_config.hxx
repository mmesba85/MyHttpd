#pragma once

#include "server_config.hh"

ServerConfig::ServerConfig()
{
}

ServerConfig::ServerConfig(std::string name, std::string port, std::string ip,
  std::string root_dir)
{
  server_name_ = name;
  port_ = port;
  ip_ = ip;
  root_dir_ = root_dir;

}

ServerConfig::~ServerConfig()
{
}

std::string ServerConfig::get_port()
{
	return port_;
}
