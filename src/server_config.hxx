#include "server_config.hh"
#include "request.hh"

ServerConfig::ServerConfig()
    : pipe_lock(new std::mutex())
{}

ServerConfig::ServerConfig(std::string name, std::string port, std::string ip,
  std::string root_dir)
    : server_name_(name), port_(port), ip_(ip), root_dir_(root_dir), pipe_lock(
            new std::mutex())
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
  std::string tmp;
  while(getline(g, tmp, ' '))
    if(tmp.find('.') != std::string::npos)
      break;

  std::istringstream h(tmp);
  std::string script;
  while(getline(h, script, '?'))
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
  getline(g, tmp, ' ');

  if(tmp.find('?') != std::string::npos)
  {
    std::istringstream h(tmp);
    getline(h, tmp, '?');
  }

  if(tmp.find('.') != std::string::npos)
    return "";

  path_info.append(tmp);
  return path_info;
}

std::string get_query(std::string url)
{
  if(url.find('?') == std::string::npos)
    return "";
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

bool ServerConfig::is_cgi(Request& request) const
{
  std::string url = request.get_url();
  std::string script = get_script(url);
  if (configurations_.find("cgi_ext") == configurations_.end())
  {
    std::cout << "COUCOU" << std::endl;
    return false;
  }

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

void ServerConfig::parse_cgi_headers(std::map<std::string, std::string>& map, FILE* file)
{
    if (!file)
        return;
    constexpr int NB_BYTES = 100;
    char buffer[NB_BYTES + 1] = {'\0'};
    size_t cpt = 0;
    size_t res = 0;

    std::string builder;
    builder.reserve(101);
    size_t last = 0;
    size_t point = 0;
    while (true)
    {
        // read the file
        cpt = res = 0;
        while (cpt < NB_BYTES && (res = fread(buffer + cpt, sizeof (char),
                        NB_BYTES - cpt, file)) > 0)
            cpt += res;
        buffer[cpt] = '\0';
        if (feof(file)) // if  eof, return the data readed
        {
            builder.append(buffer);
            map["myhttpd_exceed"] = builder;
            break;
        }
        else if (ferror(file)) // if  error, return error
        {
            map.clear();
            break;
        }
        else
        {
            // else parse the buffer
            builder.append(buffer);
            for (size_t max = builder.size(); last < max - 2; ++last)
            {
                if (!point && builder.at(last) == ':') // field detected
                    point = last;
                else if (point && builder.at(last) == '\r' && builder.at(last +
                            1) == '\n' && (builder.at(last + 2) != '\t' &&
                                builder.at(last + 2) != ' ' ))
                { // value detected
                    map[std::string(builder, 0, point)] = std::string(builder,
                            point + 1, last + 1 - point);
                    builder.erase(0, last + 2);
                    point = 0;
                    last = -1;
                    max = builder.size();
                    if (max >= 2 && builder.at(0) == '\r' && (builder.at(1) ==
                            '\n'))
                    { // there is no more entity-headers
                        map["myhttpd_exceed"] = builder;
                        builder.clear();
                        while (true)
                        {
                            res = 0;
                            cpt = 0;
                            while (cpt < NB_BYTES && (res = fread(buffer + cpt,
                                sizeof (char), NB_BYTES - cpt, file)) > 0)
                                cpt += res;
                            buffer[cpt] = '\0';
                            builder.append(buffer);
                            if (feof(file)) // if  eof, return the data readed
                            {
                                map["myhttpd_eof"] = std::move(builder);
                                break;
                            }
                            else if (ferror(file)) // if  error, return error
                            {
                                map.clear();
                                break;
                            }
                        }
                        return;
                    }
                }
            }
            cpt = 0;
            res = 0;
        }
    }
    return;
}

int ServerConfig::process_cgi(Request& request, std::string& rep_begin)
{
    std::unique_lock<std::mutex> lock(*pipe_lock);
    update_cgi_env(request); // update environment

    // run the script
    FILE* file = popen(request.get_path().c_str(), "r");
    if (file == NULL)
        return 0;

    int fd = fileno(file);
    pipes[fd] = file;
    lock.unlock();

    std::map<std::string, std::string> headers;
    parse_cgi_headers(headers, file);
    if (!headers.size())
    {
        pclose(file);
        lock.lock();
        pipes.erase(fd);
        return 0;
    }
    else if (headers.find("Content-Length") == headers.end())
    {
        headers["Content-Length"] = " " +
            std::to_string(headers["myhttpd_exceed"].size() +
            headers["myhttpd_eof"].size() - 2) + "\r\n";
    }
    fill_with_header(headers, rep_begin);
    headers.clear();
    return fd;
}

void ServerConfig::fill_with_header(std::map<std::string, std::string>&
        headers, std::string& into) const
{
    for (auto& it : headers)
    {
        if (!it.first.compare("myhttpd_exceed") ||
                !it.first.compare("myhttpd_eof"))
            continue;
        into.append(it.first);
        into.append(": ");
        into.append(it.second);
    }
    into.append(headers["myhttpd_exceed"]);
    into.append(headers["myhttpd_eof"]);
}

void ServerConfig::cancel(int fd)
{
    std::unique_lock<std::mutex> lock(*pipe_lock);
    FILE* file = pipes[fd];
    pipes.erase(fd);
    lock.unlock();
    pclose(file);
}

/**
** \brief Execute the script env with the good configuration and return a file
** descriptor to the output of the execution
** \return 0 if success, else return -1
*/
void ServerConfig::update_cgi_env(Request& request) const
{
  std::string url = request.get_url();

  std::string path_info = get_path_info(url);
  setenv("PATH_INFO", path_info.c_str(), 1);
  std::string path_translated(root_dir_);
  path_translated.append(path_info);
  setenv("PATH_TRANSLATED", path_translated.c_str(), 1);

  std::string query_string = get_query(url);
  setenv("QUERY_STRING", query_string.c_str(), 1);

  std::string remote_addr = request.get_client_ip();
  setenv("REMOTE_ADDR", remote_addr.c_str(), 1);

  // std::string remote_host = request.get_host();
  setenv("REMOTE_HOST", "", 1);

  std::string request_method = method(url);
  setenv("REQUEST_METHOD", request_method.c_str(), 1);

  std::string script_name = get_script(url);
  setenv("SCRIPT_NAME", script_name.c_str(), 1);
  // if (configurations_.find("ip") == configurations_.end())
  //   return -1;
  std::string server_name = ip_;
  setenv("SERVER_NAME", server_name.c_str(), 1);

  std::string server_port = port_;
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

  setenv("REDIRECT_STATUS", "1", 1);
}

std::string& ServerConfig::get_server_name()
{
	return server_name_;
}
