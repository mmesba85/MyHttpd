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

std::string ServerConfig::get_port() const
{
	return port_;
}

std::string ServerConfig::get_ip() const
{
	return ip_;
}

void ServerConfig::insert_to_configuration(std::string& a, std::string& b)
{
  configurations_.insert(std::pair<std::string, std::string>(a, b));
}

void ServerConfig::set_server_name(std::string& server_name)
{
  server_name_ = server_name;
}

void ServerConfig::set_port(std::string& port)
{
  port_ = port;
}

void ServerConfig::set_ip(std::string& ip)
{
  ip_ = ip;
}

void ServerConfig::set_root_dir(std::string& root_dir)
{
  root_dir_ = root_dir;
}

void insert_to_error(std::string& a, std::string& b);
{
  error_.insert(std::pair<std::string, std::string>(a, b));
}
