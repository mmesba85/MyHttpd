#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "server_connection.hh"

constexpr int max_listen = 1000;
constexpr int max_events = 1000;
constexpr int max_request_len = 255;


/* check the request line 
  (the first line of the request)
  */

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
  std::istringstream f(request);
  std::string s;    
  getline(f, s);

  std::cout << "first line : " << s << std::endl;
  s.erase(s.length()-1);

  if (!check_request_line(s))
    return false;

  std::cout << "ici" << std::endl;
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
      strings.push_back(s);
    
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

/**
** call the appropriate method
** according to the type of the request
*/
void process_request(std::string request, int fd)
{
  bool is_valid = check_request(request);
  std::cout << "is valid : " << is_valid << std::endl;
  std::cout << fd;
}


/**
** main communication method
** read the data
** call process_request to process the data
*/
void communicate(int fd)
{
  char buf[max_request_len];
  std::cout << "read" << std::endl;
  auto res = read(fd, buf, max_request_len);
  if (res == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "read data from descriptor failed."); 
  }
  process_request(buf, fd);
}

/* main server connection loop
  ** accept tcp connection
  ** create a thread each time there is data to read
  ** the thread call the communication method that will process
  ** the request
  */
int main_loop(ServerConnection& s)
{
  if(listen(s.get_socket(), max_listen) < 0)
    return -1;

  int epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "epoll_create failed.");
    return 1;
  }
  
  struct epoll_event event;
  event.data.fd = s.get_socket();
  event.events = EPOLLIN | EPOLLET;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s.get_socket(), &event) == -1)
  if (epollfd == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "epoll_ctl failed.");
    return 1;
  }

  std::array<struct epoll_event, max_events> events;

  while (true)
  {
    auto n = epoll_wait(epollfd, events.data(), max_events, -1);
    for (int i = 0; i < n; ++i)
    {
      if (events[i].events & EPOLLERR ||
          events[i].events & EPOLLHUP ||
          !(events[i].events & EPOLLIN)) 
      {
        std::cout << "ici" << '\n';
        std::error_code ec(errno, std::generic_category());
        throw std::system_error(ec, "event error.");
        close(events[i].data.fd);
      }
      else if (s.get_socket() == events[i].data.fd)
      {
        while (s.set_connection(event, epollfd))
        {
          std::cout << "accept" << '\n';
        }
        
      }
      else 
      {
        auto dscr = events[i].data.fd;
        s.get_pool().add_task(std::bind(communicate, dscr));
        s.get_pool().start();
        //s.get_pool().destroy();
      }
    }
  }
  return 0;
}

int main()
{
  
  ServerConnection s;
  int loop = main_loop(s);
  if(loop != 0)
    return loop;
	return 0;
}
