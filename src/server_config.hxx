#include "server_config.hh"
#include "request.hh"

ServerConfig::ServerConfig()
{}

ServerConfig::ServerConfig(std::string name, std::string port, std::string ip,
  std::string root_dir, const std::string& env_path)
    : server_name_(name), port_(port), ip_(ip), root_dir_(root_dir), env_path_(
            env_path)
{}

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

std::string ServerConfig::get_name() const
{
  return server_name_;
}

std::string ServerConfig::get_root_dir() const
{
  return this->root_dir_;
}

std::map<std::string, std::string> ServerConfig::get_error() const
{
  return this->error_;
}

void ServerConfig::insert_to_configuration(std::string a, std::string& b)
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

void ServerConfig::set_ip(std::string& s)
{
  ip_ = s;
}

void ServerConfig::set_root_dir(std::string& root_dir)
{
  root_dir_ = root_dir;
}

void ServerConfig::insert_to_error(std::string& a, std::string& b)
{
  error_.insert(std::pair<std::string, std::string>(a, b));
}

void ServerConfig::insert_to_proxy_pass(std::string& a, std::string& b)
{
  proxy_pass_.insert(std::pair<std::string, std::string>(a, b));
}

void ServerConfig::print()
{
  std::cout << "server_name = " << server_name_ << '\n';
  std::cout << "ip = " << ip_ << '\n';
  std::cout << "port = " << port_ << '\n';
  std::cout << "root_dir = " << root_dir_ << '\n';
  for (auto it=configurations_.begin(); it!=configurations_.end(); ++it)
  {
    std::cout << it->first << " =  " <<it->second << '\n';
  }
  std::cout << "error : " << '\n';
  for (auto it=error_.begin(); it!=error_.end(); ++it)
  {
    std::cout << it->first << ", " <<it->second << '\n';
  }
}

std::vector<std::string> string_to_array(std::string s, char delim)
{
  std::vector<std::string> strings;
  std::istringstream g(s);
  std::string aux;
  while(getline(g, aux, delim))
    strings.push_back(aux);
  return strings;
}

bool ServerConfig::is_cgi(Request& request)
{
  std::string url = request.get_url();

  std::istringstream g(url);
  std::string script;
  getline(g, script, '?');
  std::string query;
  getline(g, query, '?');

  if (configurations_.find("cgi_ext") == configurations_.end())
    return false;

  std::vector<std::string> cgi_ext = string_to_array(configurations_.at("cgi_ext"), ',');
  std::istringstream h(script);
  std::string ext;
  getline(g, ext, '.');
  getline(g, ext, '.');

  if (std::find(cgi_ext.begin(), cgi_ext.end(), ext) != cgi_ext.end())
    return true;
  return false;
}


///////// CGI PART

int ServerConfig::process_cgi(Request& request, std::string rep_begin)
{
    request = request;
    rep_begin = rep_begin;
    return 0;
}

void ServerConfig::cancel(int fd)
{
    ++fd;
}

void ServerConfig::update_cgi_env(Request& request)
{
    request = request;
}

std::string& ServerConfig::get_server_name()
{
	return server_name_;
}
