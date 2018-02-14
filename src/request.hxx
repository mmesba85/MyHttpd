#include "request.hh"

#include <regex>

std::string& Request::get_version()
{
    return version_;
}

std::string& Request::get_url()
{
    return url_;
}

bool Request::is_connected()
{
    return connected_;
}

/* a voir */
bool Request::bad_method() const
{
    return true;
}

std::string Request::extract_resource_path() const
{
    std::string pattern("");
    // build the pattern
    pattern.append("((/[a-zA-Z0-9-_=\\.#&]?)+)"); // the resource path.
    pattern.append("?([a-zA-Z0-9-_=\\./\\+#]&)*"); // the query

    std::smatch result;
    std::regex regex(pattern);
    if (std::regex_match(url_, result, regex))
    {
        std::string path = result[1];
        if (path.at(path.size() - 1) == '/')
            path.append("index.html");
        return path;
    }

    return "";
}

bool Request::forbidden(const ServerConfig& config) const
{
    std::string path = extract_resource_path();
    std::ifstream file(config.get_root_dir() + path);
    file.get();
    bool res = file.bad();
    file.close();
    return res;
}

bool Request::not_found(const ServerConfig& config) const
{
    std::string path = extract_resource_path();
    if (path.empty())
        return false;

    std::cout << config.get_root_dir() + path << std::endl;
    std::ifstream file(config.get_root_dir() + path);
    bool res = !file;
    file.close();
    return res;
}

std::string get_method(std::string request)
{
    std::istringstream f(request);
    std::string s;    
    getline(f, s);
    std::istringstream g(s);
    std::string res;
    getline(g, res, ' ');
    return res;
} 

bool check_request_line(std::string line)
{
    std::vector<std::string> strings;
    std::istringstream g(line);
    std::string s;
    while (getline(g, s, ' '))
        strings.push_back(s);

    if(strings.size() != 3)
        return false;

    return true;
}

/* check the request (syntax) */
bool check_request(std::string request)
{
    if(request.empty())
        return false;

    std::istringstream f(request);
    std::string s;    
    getline(f, s);

    if(s.at(s.length()-1) != '\r')
        return false;

    s.erase(s.length()-1);
    if (!check_request_line(s))
        return false;

    /* marque l'arrivee a la fin de la requete
       si une autre ligne est lu apres cette fin
       alors erreur
       */
    auto is_end = 0;
    while(getline(f, s))
    {
        std::vector<std::string> strings;
        std::istringstream g(s);
        std::string aux;

        if(is_end == 1)
            return false;

        /* si le dernier caractere de la ligne est different
           de \r alors erreur
           */
        size_t len = s.length();
        if(s.at(len-1) != '\r')
            return false;

        /* on cherche la fin de requete */
        if(s.compare("\r") == 0)
        {
            is_end = 1;
            continue;
        }

        /* on decoupe la ligne en champs */
        while(getline(g, aux, ' '))
            strings.push_back(aux);

        /* s'il y a plus de deux mots alors erreur */
        if(strings.size() != 2)
            return false;

        /* si l'argument du head connection est different 
           de close ou de keep-alive alors erreur
           */
        if(strings[0].compare("Connection:") == 0 && 
                strings[1].compare("close") != 0 &&
                strings[1].compare("keep-alive") != 0)
            return false;
    }
    if(is_end == 0)
        return false;
    return true;
}

void Request::init(std::string&& url, std::string&& version, std::string&& host, bool connected)
{
    url_ = url;
    version_ = version;
    host_ = host;
    connected_ = connected;
}
