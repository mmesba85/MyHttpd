#pragma once

#include "response.hh"

Response::Response()
{
  version_ = "";
  status_code_ = "";
  reason_phrase_ = "";
}

std::string Response::build_response(Request& request)
{
  request = request;
  std::string res("");
  return res;
}
   
std::string Response::build_response()
{
  std::string res("");
  return res;
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