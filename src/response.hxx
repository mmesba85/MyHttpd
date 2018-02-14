#include <ctime>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
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

int get_file_len(Request& request)
{
  std::string file_name = request.extract_resource_path();
  std::ifstream file(file_name, std::ifstream::binary | std::ifstream::in);
  file.seekg(0, std::ios::end);
  int size = file.tellg();
  file.close();
  return size;
}

std::string Response::build_response(Request& request)
{
  std::string res;
  std::stringstream ss;
  ss << version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";
  ss << "Date: " << get_date() << " GMT" << "\r\n" << "\r\n"; 
  std::cout << "response: " << ss.str() << std::endl;
  ss << "Content-Length: " << get_file_len(request) << "\r\n\r\n";
  return ss.str();
}
   
std::string Response::build_response()
{
  std::string res;
  std::stringstream ss;
  ss << version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";
  ss << "Date: " << get_date() << " GMT" << "\r\n" << "\r\n"; 
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

int Response::send_data(Request& rq, ServerConfig& config, int fd)
{
  std::string response = rq.process_request(*this, config);
  std::string file_name = rq.extract_resource_path();
  size_t response_len = response.length();
  int res = send(fd, response.c_str(), response_len, 0);
  if(!res)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "Fail send.");
  }
  if(file_name != "")
  {
    int file_fd = open(file_name.c_str(), O_RDONLY);
    struct stat stat_buf;
    fstat(file_fd, &stat_buf);
    off_t offset = 0;
    res = sendfile(fd, file_fd, &offset, stat_buf.st_size);
    if(!res)
    {
      close(file_fd);
      std::error_code ec(errno, std::generic_category());
      throw std::system_error(ec, "Fail sendfile.");
    }
    close(file_fd);
  }
  if(!rq.is_connected())
    close(fd);
  return 0;
}
