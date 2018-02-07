#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "server_connection.hh"
#include "request.hh"

constexpr int max_listen = 1000;
constexpr int max_events = 1000;
constexpr int max_request_len = 255;

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
