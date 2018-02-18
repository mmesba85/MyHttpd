#include <ostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <regex>

#include "request.hh"
#include "server-config.hh"

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
