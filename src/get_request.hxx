#include "get_request.hh"
#include "response.hh" 

GETRequest::GETRequest(std::string request)
{
  std::vector<std::string> first_line;
  std::istringstream f(request);
  std::string s;    
  getline(f, s);
  std::istringstream g(s);
  std::string aux;
  while(getline(g, aux, ' '))
    first_line.push_back(aux);
  url_ = first_line[1];
  version_ = first_line[2];
  host_ = "";
  if(version_.compare("HTTP/1.1") == 0)
    connected_ = true;
  else
    connected_ = false;
  while(getline(f, s))
  {
    std::vector<std::string> strings;
    std::istringstream g(s);
    std::string aux;
    while(getline(g, aux, ' '))
      strings.push_back(aux);
    if(strings[0].compare("Host:") == 0)
      host_ = strings[1];
    if(strings[0].compare("Connected:") == 0)
    {
      if(strings[1].compare("close") == 0)
        connected_ = false;
      else
        connected_ = true;
    }
  }
}

std::string GETRequest::process_request(Response& rp, ServerConfig& config)
{
  if(forbidden(config))
    rp.set_code("403");
  else if(not_found(config))
    rp.set_code("404");
  else if(bad_method())
    rp.set_code("405");
  else if(version_.compare("HTTP/1.1") != 0 && version_.compare("HTTP/1.0") != 0)
    rp.set_code("505");
  else if(version_.compare("HTTP/1.1") == 0 && host_ == "")
    rp.set_code("400");
  else
  {
    rp.set_code("200");
    return rp.build_response(*this);
  }
  return rp.build_response();
}
