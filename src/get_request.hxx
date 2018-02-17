#include "response.hh" 
#include "get_request.hh"

GETRequest::GETRequest(std::string& request, std::string& ip, 
  const ServerConfig& config)
{
  type_ = "GET";
  client_ip_ = ip;
  std::vector<std::string> first_line;
  std::istringstream f(request);
  std::string s;    
  getline(f, s);
  s.erase(s.length()-1);
  url_ = s;
  std::istringstream g(s);
  std::string aux;
  while(getline(g, aux, ' '))
    first_line.push_back(aux);
  version_ = first_line[2];
  host_ = "";
  if(version_.compare("HTTP/1.1") == 0)
    connected_ = true;
  else
    connected_ = false;
  while(getline(f, s))
  {
    if(s.compare("\r") == 0)
      continue;
    s.erase(s.length()-1);
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
  std::cout << first_line[1] << std::endl;
  std::string pattern("");
  // build the pattern
  pattern.append(
 "/^(https?:\\/\\/)?([\\da-z\\.-]+)\\.([a-z\\.]{2,6})([\\/\\w \\.-]*)*\\/?$/)");
  //pattern.append("((/[a-zA-Z0-9-_=\\.#&]*)+)"); // the resource path.
  //pattern.append("?([a-zA-Z0-9-_=\\./\\+#])*"); // the query
  std::smatch result;
  std::regex regex(pattern);
  if (std::regex_match(first_line[1], result, regex))
  {
    path_.append(config.get_root_dir());
    path_.append(result[1]);
    if (path_.at(path_.size() - 1) == '/')
      path_.append("index.html");
    std::cout << path_ << std::endl;
  }
}

std::string GETRequest::process_request(Response& rp)
{
  if(not_found())
    rp.set_code("404");
  else if(forbidden())
    rp.set_code("403");
  else if(bad_method())
    rp.set_code("405");
  else if(version_.compare("HTTP/1.1") != 0 && version_.compare("HTTP/1.0") != 0)
    rp.set_code("505");
  else if(version_.compare("HTTP/1.1") == 0 && host_ == "")
    rp.set_code("400");
  else
    rp.set_code("200");
  return rp.build_response();
}
