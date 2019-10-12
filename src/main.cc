#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <csignal>
#include <string.h>
#include <tuple>

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
** \brief communication method
** read the data
** check if the syntax is valid, otherwise send 400 error
** if valid, check for the method then call the process_request
** that would do all the other checks and send the response
** \param the client's socket, the server's configuration and the log stream
*/
void communicate(int fd, ServerConfig& config, std::ofstream& log)
{
  char buf[max_request_len];
  auto res = read(fd, buf, max_request_len);
  buf[res] = '\0';

  if(res == 0)
    return;

  if (res == -1)
  {
    std::error_code ec(errno, std::generic_category());
    throw std::system_error(ec, "read data from descriptor failed."); 
  }

  std::string check(buf);
  bool is_valid = check_request(check);
  
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
      GETRequest rq(check, ip, config);
      log << "[" << config.get_server_name() << "] ";
      log << "GET " << rq.get_version() << " " << rq.get_url() << '\n';
      std::string version = rq.get_version();
      Response rp(version);
      rp.process_response(rq, config, fd);
    } 
  } 
}

/**
** \brief get the struct epoll_event of the file descriptor
** and the server's associated to it
** \param the list of ServerConnection and a list of the epoll_event structures
** associated to it, the file descriptor returned by epoll
** \return a pair of ServerConnection and struct epoll_event
**/

std::pair<ServerConnection, struct epoll_event> get_server_infos(std::vector<ServerConnection>& s, 
                      std::vector<struct epoll_event>& ev, int fd)
{
  std::pair<ServerConnection, struct epoll_event> res;
  for(auto it = s.begin(); it != s.end(); ++it)
  {
    for(auto it_ = ev.begin(); it_ != ev.end(); ++it_)
    {
      if ((*it).get_socket() == fd)
      {
        return std::make_pair(*it, *it_);
      }
    }
  }
  return res;
}

/**
** \brief main server connection loop
** accept tcp connection
** create a thread each time there is data to read
** the thread call the communication method that will process
** the request
** \param list of ServerConnection objects and the stream of the log
** \return 0 if ok, -1 othersize
**/
int main_loop(std::vector<ServerConnection>& list_c, std::ofstream& log)
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
    memset(&event, 0, sizeof(struct epoll_event));
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
      std::pair<ServerConnection, struct epoll_event> p = get_server_infos(list_c, ev, events[i].data.fd);
      if(p.first.get_socket() == events[i].data.fd)
      {
        while(p.first.set_connection(p.second, epollfd))
          aux = p.first.get_config();
      }
      else
      {
        auto dscr = events[i].data.fd;
        th.add_task(std::bind(communicate, dscr, std::ref(aux), std::ref(log)));
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
}

int main(int argc, char* argv[])
{
  if(argc > 1)
  {
    std::string s(argv[1]);
    if(s.compare("--dry-run") == 0 && argc == 3)
    {
      Configuration conf(argv[2]);
      conf.fill_configuration();
      conf.print();
      return 0;
    } 
    else
    {
      Configuration conf(argv[1]);
      conf.fill_configuration();
      try
      {
        std::vector<ServerConnection> list_connection;
        std::vector<ServerConfig> list_config = conf.get_list();
        for(auto it = list_config.begin(); it != list_config.end(); it++)
        {
          ServerConnection s(*it);
          list_connection.push_back(s);
        }
   
        std::ofstream log;
        log.open(conf.get_log_file());
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
        std::signal(SIGINT, sig_handler);
        main_loop(list_connection, log);
        log.close();
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
