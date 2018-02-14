#include <iostream>
#include <string>
#include <map>
#include <vector>

static std::vector<std::string> string_to_array(std::string s, char delim)
{
  std::vector<std::string> strings;
  std::istringstream g(s);
  std::string aux;
  while(getline(g, aux, delim))
    strings.push_back(aux);
  return strings;
}

bool is_cgi(std::string str, std::vector<std::string> cgi_ext)
{
  // std::string url = request.get_url();
  // if (request.get_method() == "GET")
  // {
    std::istringstream g(str);
    std::string script;
    getline(g, script, '$');
    std::string query;
    getline(g, query, '$');

    // if (configurations_.find("cgi_get") == configurations_.end())
    //   return false;

    // std::vector<std::string> cgi_ext = string_to_array(configurations_.at("cgi_ext"));
    std::istringstream h(script);
    std::string ext;
    getline(h, ext, '.');
    getline(h, ext, '.');

    if (std::find(cgi_ext.begin(), cgi_ext.end(), ext) != cgi_ext.end())
      return true;
    return false;
  // }
}

int main()
{
  std::string s = "php,sh";
  std::string url = "/script.php?var1=abc&var2=4 HTTP/1.1";
  std::vector<std::string> cgi_ext = string_to_array(s, ",");
  std::cout << is_cgi(url, cgi_ext) << " " << true << '\n';
  return 0;
}
