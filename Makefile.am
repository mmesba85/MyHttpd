AUTOMAKE_OPTIONS = foreign


CXXFLAGS=-std=c++17 -Wall -Wextra -Werror -pedantic
LDFLAGS=-lpthread
VPATH=src2:src2/threadpool

bin_PROGRAMS = myhttpd
myhttpd_SOURCES = thread-pool.cc executor.cc main.cc configuration.cc get-request.cc request.cc response.cc server-config.cc server-connection.cc
