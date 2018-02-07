#pragma once

#include "request.hh"

Request::Request(std::string url, int version, std::string host, 
  bool connected)
  : url_(url), version_(version), host_(host), connected_(connected)
  {}

