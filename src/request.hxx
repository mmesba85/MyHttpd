#include <ostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "request.hh"
#include "server_config.hh"

#include <regex>

std::string& Request::get_type()
{
  return type_;
}

std::string& Request::get_path()
{
  return path_;
}

std::string& Request::get_client_ip()
{
  return client_ip_;
}

std::string& Request::get_version()
{
    return version_;
}

std::string& Request::get_url()
{
    return url_;
}

std::string& Request::get_host()
{
  return host_;
}


bool Request::is_connected()
{
    return connected_;
}

bool Request::bad_method() const
{
  if(type_.compare("GET") != 0)
    return true;
  return false;
}

bool Request::forbidden() const
{
  if(access(path_.c_str(), R_OK) == 0)
    return false;
  return true;
}

bool Request::not_found() const
{
  struct stat buf;
  return !(stat(path_.c_str(), &buf) == 0);
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

bool check_request_line(std::string& line)
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
bool check_request(std::string& request)
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

        /* split the line into fields */
        while(getline(g, aux, ' '))
            strings.push_back(aux);

        if(strings[0].compare("Connection:") == 0 &&
                strings[1].compare("close") != 0 &&
                strings[1].compare("Keep-Alive") != 0)
          return false;
    }
    if(is_end == 0)
        return false;
    return true;
}
