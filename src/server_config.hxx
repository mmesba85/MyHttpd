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

std::string method(std::string url)
{
  std::istringstream g(url);
  std::string method;
  getline(g, method, ' ');
  return method;
}

std::string get_script(std::string url)
{
  std::istringstream g(url);
  std::string script;
  while(getline(g, script, '/'))
    if(script.find('.') != std::string::npos)
      break;
  return script;
}

std::string get_path_info(std::string url)
{
  std::istringstream g(url);
  std::string path_info;

  while(getline(g, path_info, '/'))
    if(path_info.find('.') != std::string::npos)
      break;
  path_info = "/";
  std::string tmp;
  getline(g, tmp, '?');
  path_info.append(tmp);

  return path_info;
}

std::string get_query(std::string url)
{
  std::istringstream g(url);
  std::string query;
  getline(g, query, '?');
  getline(g, query, ' ');
  return query;
}

std::string get_current_path()
{
  char temp[1024];
  if (getcwd(temp, 1024) != 0)
    return std::string(temp);
  return "";
}

bool ServerConfig::is_cgi(Request& request)
{
  std::string url = request.get_url();

  std::string script = get_script(url);

  if (configurations_.find("cgi_ext") == configurations_.end())
    return false;

  std::vector<std::string> cgi_ext =
        string_to_array(configurations_.at("cgi_ext"), ',');
  std::istringstream h(script);
  std::string ext;
  getline(h, ext, '.');
  getline(h, ext, '.');

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

/**
** \brief Execute the script env with the good configuration and return a file
** descriptor to the output of the execution
** \return 0 if success, else return -1
*/
void ServerConfig::update_cgi_env(Request& request)
{
  std::string url = request.get_url();

  std::string path_info = get_path_info(url);
  setenv("PATH_INFO", path_info.c_str(), 1);

  std::string path_translated = root_dir_.append(path_info);
  setenv("PATH_TRANSLATED", path_translated.c_str(), 1);

  std::string query_string = get_query(url);
  setenv("QUERY_STRING", query_string.c_str(), 1);

  std::string remote_addr = request.get_client_ip(); //TODO
  setenv("REMOTE_ADDR", remote_addr.c_str(), 1);

  // std::string remote_host = request.get_host();
  setenv("REMOTE_HOST", "", 1);

  std::string request_method = method(url);
  setenv("REQUEST_METHOD", request_method.c_str(), 1);

  std::string script_name = get_script(url);
  setenv("SCRIPT_NAME", script_name.c_str(), 1);

  // if (configurations_.find("ip") == configurations_.end())
  //   return -1;
  std::string server_name = configurations_.at("ip");
  setenv("SERVER_NAME", server_name.c_str(), 1);

  //   // if (configurations_.find("port") == configurations_.end())
  //   //   return -1;
  std::string server_port = configurations_.at("port");
  setenv("SERVER_PORT", server_port.c_str(), 1);

  std::string server_protocol = request.get_version();
  setenv("SERVER_PROTOCOL", server_protocol.c_str(), 1);

  std::string server_software = "My";
    server_software.append(server_protocol);
  setenv("SERVER_SOFTWARE", server_software.c_str(), 1);

  std::string script_filename = get_current_path();
  script_filename.append("/");
  script_filename.append(root_dir_);
  script_filename.append(script_name);
  setenv("SCRIPT_FILENAME", script_filename.c_str(), 1);
}

std::string& ServerConfig::get_server_name()
{
	return server_name_;
}
