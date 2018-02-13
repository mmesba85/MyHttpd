#include <ctime>
#include <fstream>
#include <iostream>
#include <time.h>
#include "response.hh"
#include "request.hh"

Response::Response()
{
  version_ = "";
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

int get_file_len(Request& request, const ServerConfig& conf)
{
  std::string file_name = request.extract_resource_path(conf);
  std::ifstream file(file_name, std::ifstream::binary | std::ifstream::in);
  file.seekg(0, std::ios::end);
  int size = file.tellg();
  file.close();
  return size;
}

std::string Response::build_response(Request& request, const ServerConfig& conf)
{
  std::string res;
  std::stringstream ss;
  ss << version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";
  ss << "Date: " << get_date() << " GMT" << "\r\n" << "\r\n"; 
  std::cout << "response: " << ss.str() << std::endl;
  ss << "Content-Length: " << get_file_len(request, conf) << "\r\n\r\n";
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
