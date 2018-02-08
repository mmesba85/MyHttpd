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
}
    
void Response::set_version(const std::string& version)
{
	version_ = version;
}