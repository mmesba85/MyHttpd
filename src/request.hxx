#pragma once

#include "request.hh"

Request::Request(std::string url, int version, std::string host, 
  bool connected)
  : url_(url), version_(version), host_(host), connected_(connected)
  {}

bool check_request_line(std::string line)
{
  std::vector<std::string> strings;
  std::istringstream g(line);
  std::string s;
  while (getline(g, s, ' '))
    strings.push_back(s);

  if(strings.size() != 3)
    return false;
  if(strings[0].compare("GET") != 0 && 
    strings[0].compare("POST") != 0)
    return false;
  
  if(strings[2].compare("HTTP/1.1") != 0 &&
     strings[2].compare("HTTP/1.0") != 0)
    return false;  
  return true;
}

/* check the request */
bool check_request(std::string request)
{
  if(request.empty())
    return false;
  std::istringstream f(request);
  std::string s;    
  getline(f, s);

  std::cout << "first line : " << s << std::endl;
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
  return true;
}