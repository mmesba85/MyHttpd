#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <signal.h>
#include <mutex>
#include "server_connection.hh"
#include "request.hh"
#include "response.hh"
#include "get_request.hh"
#include "configuration.hh"
#include "threadpool/thread-pool.hh"




constexpr int max_listen = 100;
constexpr int max_events = 100;
constexpr int max_request_len = 2000;
bool loop_handler = true;

/**
** main communication method
** read the data
** check if the syntax is valid, otherwise send 400 error
** if valid, check for the method then call the process_request
** that would do all the other checks and return the right
** response
*/
void communicate(int fd, ServerConfig config)
{
  char buf[max_request_len];
  auto res = read(fd, buf, max_request_len);
  buf[res] = '\0';

  if(res == 0)
    return;

  std::cout << "request: \n" << buf << std::endl;

  if (res == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "read data from descriptor failed."); 
  }

  bool is_valid = check_request(buf);
  std::cout << "is valid : " << is_valid << std::endl;
  std::string response;
  if(!is_valid) //syntax error
  {
    Response rp("HTTP/1.1");    
    rp.set_code("400");
    rp.process_response(config, fd);
  }
  else
  {
    std::string method = get_method(buf);
    if(method.compare("GET") == 0)
    {
      struct sockaddr_in addr;
      socklen_t addr_size = sizeof(struct sockaddr_in);
      getpeername(fd, (struct sockaddr *)&addr, &addr_size);
      std::string ip = inet_ntoa(addr.sin_addr);
      GETRequest rq(buf, ip);
      std::string version = rq.get_version();
      Response rp(version);
      rp.process_response(rq, config, fd);
    } 
  } 
}

bool is_server_socket(std::vector<ServerConnection>& s, 
                      std::vector<struct epoll_event>& ev, int fd)
{
  for(auto it = s.begin(); it != s.end(); ++it)
  {
    for(auto it_ = ev.begin(); it_ != ev.end(); ++it_)
    {
      if ((*it).get_socket() == fd)
      {
        return true;
      }
    }
  }
  return false; 
}


/* main server connection loop
  ** accept tcp connection
  ** create a thread each time there is data to read
  ** the thread call the communication method that will process
  ** the request
  */
int main_loop(std::vector<ServerConnection> list_c)
{
  for(auto it = list_c.begin(); it != list_c.end(); ++it)
  {
    if(listen((*it).get_socket(), max_listen) < 0)
      return -1;
  }
  int epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "epoll_create failed.");
    return 1;
  }
  
  std::vector<struct epoll_event> ev;
  for(auto it = list_c.begin(); it != list_c.end(); ++it)
  {
    struct epoll_event event;
    event.data.fd = (*it).get_socket();
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, (*it).get_socket(), &event) == -1)
    {  
      if (epollfd == -1)
      {
        std::error_code ec(errno, std::generic_category());
        throw std::system_error(ec, "epoll_ctl failed.");
        return 1;
      }
    }
    ev.push_back(event);
  } 
  ThreadPool th;
  ServerConfig aux;
  std::array<struct epoll_event, max_events> events;
  while (loop_handler)
  {
    auto n = epoll_wait(epollfd, events.data(), max_events, -1);
    for (int i = 0; i < n; ++i)
    {
      if (events[i].events & EPOLLERR ||
          events[i].events & EPOLLHUP ||
          !(events[i].events & EPOLLIN)) 
      {
        close(events[i].data.fd);
      }
      else if(is_server_socket(list_c, ev, events[i].data.fd))
      {
        for(auto it = list_c.begin(); it != list_c.end(); ++it)
        {
          for(auto it_ = ev.begin(); it_ != ev.end(); ++it_)
          {
            if ((*it).get_socket() == events[i].data.fd)
            {
              while((*it).set_connection(*it_, epollfd))
              {
                std::cout << "accept " << std::endl;
                aux = (*it).get_config();
              }
            }
          }
        }
      }
      else
      {
        auto dscr = events[i].data.fd;
        th.add_task(std::bind(communicate, dscr, aux));
      } 
    }
  }
  return 1;
}

int build_socket(int port, std::string& ip_s, int sock)
{
  if(sock < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "error socket creation.");  
  }

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  const char* ip = ip_s.c_str();
  sin.sin_addr.s_addr = inet_addr(ip);
  
  int reuse = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (
    const char*)&reuse, sizeof(reuse)) < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "setsockopt(SO_REUSEPORT) failed.");
  }

  if(bind(sock, (struct sockaddr *)&sin, sizeof sin) == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "bind failed.");
  }

  if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "fcntl failed.");
  }  
  return sock;  
}


void sig_handler(int sig)
{
  if(sig == SIGINT)
    std::cout << "\n Connexion closed, process terminated." << std::endl;
  loop_handler = false;
  exit(1);
}

int main(int argc, char* argv[])
{
  if(argc > 1)
  {
    std::string s(argv[1]);
    if(s.compare("--dry-run") == 0 && argc == 3)
    {
      Configuration conf(argv[2]);
      try
      {
        conf.fill_configuration();
        conf.print();
        return 0;
      }
      catch(std::exception)
      {
        return 2;
      }
    }  
    else
    {
      Configuration conf(argv[1]);
      try
      {
        conf.fill_configuration();
      }
      catch(std::exception)
      {
        return 2;
      }
      try
      {
        std::vector<ServerConnection> list_connection;
        std::vector<ServerConfig> list_config = conf.get_list();
        for(auto it = list_config.begin(); it != list_config.end(); it++)
        {
          ServerConnection s(*it);
          list_connection.push_back(s);
        }
   
        int size_list = list_connection.size();
        int i = 0;
        while(i < size_list)
        {
          int socke = socket(AF_INET, SOCK_STREAM, 0);
          std::string::size_type sz;
          int port = std::stoi(list_connection[i].get_config().get_port(), &sz);
          std::string ip = list_connection[i].get_config().get_ip();
          int sock = build_socket(port, ip, socke);
          list_connection[i].set_socket(sock);
          i++;
        }
        signal(SIGINT, &sig_handler);
        return main_loop(list_connection);
      }
      catch(std::exception& e)
      {
        std::cout << e.what() << std::endl;
        return 1;
      }
    }
  }
  return 0;
}
