#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <signal.h>
#include "server_connection.hh"
#include "request.hh"
#include "response.hh"
#include "get_request.hh"

constexpr int max_listen = 1000;
constexpr int max_events = 1000;
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

  std::cout << "request: \n" << buf << "*" << std::endl;

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
      GETRequest rq(buf);
      std::string version = rq.get_version();
      Response rp(version);
      rp.process_response(rq, config, fd);
    } 
  }
  
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

  while (loop_handler)
  {
    auto n = epoll_wait(epollfd, events.data(), max_events, -1);
    for (int i = 0; i < n; ++i)
    {
      if (events[i].events & EPOLLERR ||
          events[i].events & EPOLLHUP ||
          !(events[i].events & EPOLLIN)) 
      {
       // std::error_code ec(errno, std::generic_category());
       // throw std::system_error(ec, "event error.");
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
        s.get_pool().add_task(std::bind(communicate, dscr, s.get_config()));
        s.get_pool().start();
      }
    }
  }
  return 0;
}

void end(int sig)
{
  std::cout << "end "<<  sig << '\n';
  loop_handler = false;
}

int main()
{
  signal(SIGINT, &end);
  ServerConnection s;
  int loop = main_loop(s);
  if(loop != 0)
    return loop;
	return 0;
}
