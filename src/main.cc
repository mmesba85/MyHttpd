#include "server_connection.hh"

constexpr int max_listen = 1000;
constexpr int max_events = 1000;

void communicate(int fd)
{
    std::cout << "hello " << '\n';
}


int main()
{
    ServerConnection s;

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
                std::error_code ec(errno, std::generic_category());
                throw std::system_error(ec, "event error.");
                close(events[i].data.fd);
            }
            else if (s.get_socket() == events[i].data.fd)
            {
                while (s.set_connection(event, epollfd)) {
                    std::cout << "accept" << '\n';
                }
            }
            else 
            {
                auto dscr = events[i].data.fd;
                s.get_pool().add_task(std::bind(communicate, dscr));
                s.get_pool().start();
                s.get_pool().destroy();
            }
        }
    }
	return 0;
}