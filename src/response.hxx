#include <ctime>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include "response.hh"
#include "request.hh"

Response::Response(const std::string& version)
{
  version_ = version;
  status_code_ = "";
  reason_phrase_ = "";
}

std::string get_date()
{
  time_t rawtime;
  struct tm* ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  char buffer[80];
  strftime(buffer, 80, "%a, %d %b %Y %T", ptm);
  return buffer;
}
   
std::string Response::build_response()
{
  std::string res;
  std::stringstream ss;
  ss << version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";
  ss << "Date: " << get_date() << " GMT" << "\r\n"; 
  std::cout << "response: " << ss.str() << std::endl;
  return ss.str();
}


void Response::set_code(const std::string& code)
{
	status_code_ = code;
  if(code.compare("200") == 0)
    reason_phrase_ = "OK";
  else if(code.compare("505") == 0)
    reason_phrase_ = "HTTP Version not supported";
  else if(code.compare("403") == 0)
    reason_phrase_ = "Forbidden";
  else if(code.compare("404") == 0)
    reason_phrase_ = "Not Found";
  else if(code.compare("405") == 0)
    reason_phrase_ = "Method Not Allowed";
  else if(code.compare("400") == 0)
    reason_phrase_ = "Bad Request";
}
    
void Response::set_version(const std::string& version)
{
  version_ = version;
}

std::string& Response::get_code()
{
  return status_code_;
}

int Response::get_file_dscr(const ServerConfig& config, const std::string& file_name)
{
  std::map<std::string, std::string> map = config.get_error();
  int file_fd = 0;
  if(status_code_.compare("200") == 0 && file_name.compare("") != 0)
    file_fd = open(file_name.c_str(), O_RDONLY);
  std::cout << "file_fd " << file_fd << std::endl;
  if(!map.empty())
  {
    std::map<std::string, std::string>::iterator it;
    it = map.find(status_code_);
    if(it != map.end())
      file_fd = open(it->second.c_str(), O_RDONLY);
  }
  if(file_fd == -1)
  {
    close(file_fd);
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "Fail open ressource"); 
  }
  return file_fd;
}

int Response::process_response(const ServerConfig& config, int fd)
{
  std::string response = build_response();
  int res = 0;
  int file_fd = get_file_dscr(config, "");
  if(file_fd == 0)
  {
    response.append("\r\n");
    res = send(fd, response.c_str(), response.length(), MSG_MORE);
    if(res == -1)
    {
      close(file_fd);
      std::error_code ec(errno, std::generic_category());
      throw std::system_error(ec, "Fail sendfile.");
    }
    return 0; 
  }
  return send_response(fd, file_fd, response);
}

int Response::process_response(Request& rq, const ServerConfig& config, int fd)
{
  std::string response = rq.process_request(*this, config);
  std::string file_name = rq.extract_resource_path(config);
  int res = 0;
  int file_fd = get_file_dscr(config, file_name.c_str());
  if(file_fd == 0)
  {
    response.append("\r\n");
    res = send(fd, response.c_str(), response.length(), MSG_MORE);
    if(res == -1)
    {
      close(file_fd);
      std::error_code ec(errno, std::generic_category());
      throw std::system_error(ec, "Fail sendfile.");
    }
    return 0; 
  }
  send_response(fd, file_fd, response);
  if(!rq.is_connected())
    close(fd);
  return 0;
}

int Response::send_response(int fd, int file_fd, std::string& response)
{
  auto file_len = lseek(file_fd, 0, SEEK_END);
  lseek(file_fd, 0, SEEK_SET);
  response.append("Content-Length: ");
  response.append(std::to_string(file_len));
  response.append("\r\n\r\n");

  int res = send(fd, response.c_str(), response.length(), MSG_MORE);
  if(res == -1)
  {
    close(file_fd);
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "Fail sendfile.");
  }

  res = sendfile(fd, file_fd, 0, file_len);
  if(res == -1)
  {
    close(file_fd);
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "Fail sendfile.");
  }

  close(file_fd);
  return 0; 
}