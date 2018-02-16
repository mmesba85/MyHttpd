#include <ostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "request.hh"
#include "server_config.hh"

#include <regex>

std::string& Request::get_version()
{
    return version_;
}

std::string& Request::get_url()
{
    return url_;
}

std::string& get_host()
{
  return host_;
}


bool Request::is_connected()
{
    return connected_;
}

/* a voir */
bool Request::bad_method() const
{
    return false;
}

std::string Request::extract_resource_path(const ServerConfig& config) const
{
  std::string pattern("");
  // build the pattern
  pattern.append("((/[a-zA-Z0-9-_=\\.#&]*)+)"); // the resource path.
  pattern.append("?([a-zA-Z0-9-_=\\./\\+#]&)*"); // the query

  std::smatch result;
  std::regex regex(pattern);
  if (std::regex_match(url_, result, regex))
  {
    std::string path(config.get_root_dir());
    path.append(result[1]);
    if (path.at(path.size() - 1) == '/')
      path.append("index.html");
    return path;
  }
  return "";
}

bool Request::forbidden(const ServerConfig& config) const
{
  std::string path = extract_resource_path(config);
  if(access(path.c_str(), R_OK) == 0)
    return false;
  return true;
}

bool Request::not_found(const ServerConfig& config) const
{
  std::string path = extract_resource_path(config);
  struct stat buf;
  return !(stat(path.c_str(), &buf) == 0);
}

std::string get_method(std::string request)
{
    std::istringstream f(request);
    std::string s;
    getline(f, s);
    std::istringstream g(s);
    std::string res;
    getline(g, res, ' ');
    return res;
}

bool check_request_line(std::string line)
{
    std::vector<std::string> strings;
    std::istringstream g(line);
    std::string s;
    while (getline(g, s, ' '))
        strings.push_back(s);

    if(strings.size() != 3)
        return false;

    return true;
}

/* check the request (syntax) */
bool check_request(std::string request)
{
    if(request.empty())
        return false;

    std::istringstream f(request);
    std::string s;
    getline(f, s);

    if(s.at(s.length()-1) != '\r')
        return false;

    s.erase(s.length()-1);
    if (!check_request_line(s))
        return false;

    /* marque l'arrivee a la fin de la requete
       si une autre ligne est lu apres cette fin
       alors erreur
       */
    auto is_end = 0;
    while(getline(f, s))
    {
        size_t len = s.length();
        if(s.at(len-1) != '\r')
            return false;

        if(s.compare("\r") == 0)
        {
            is_end = 1;
            continue;
        }

        s.erase(len-1);

        std::vector<std::string> strings;
        std::istringstream g(s);
        std::string aux;

        if(is_end == 1)
            return false;

        /* on decoupe la ligne en champs */
        while(getline(g, aux, ' '))
            strings.push_back(aux);

        /* si l'argument du head connection est different
           de close ou de keep-alive alors erreur
           */
        if(strings[0].compare("Connection:") == 0 &&
                strings[1].compare("close") != 0 &&
                strings[1].compare("Keep-Alive") != 0)
        {
          std::cout << "laaa " << std::endl;
          return false;
        }
    }
    if(is_end == 0)
        return false;
    return true;
}

void Request::init(std::string&& url, std::string&& version, std::string&& host, bool connected)
{
    url_ = url;
    version_ = version;
    host_ = host;
    connected_ = connected;
}
