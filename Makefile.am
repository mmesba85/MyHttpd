AUTOMAKE_OPTIONS = foreign


CXXFLAGS=-std=c++17 -Wall -Wextra -Werror -pedantic -g
LDFLAGS=-lpthread
VPATH=src:src/threadpool

bin_PROGRAMS = myhttpd
myhttpd_SOURCES = thread-pool.cc executor.cc main.cc
