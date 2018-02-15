#pragma once

#include <string>
#include <map>
#include <cstdio>

class Request;

class ServerConfig;

class CgiHandler
{
    public:
        CgiHandler();
        CgiHandler(const std::string&);
        int process(Request& req_obj, ServerConfig& conf, std::string& rep);
        void cancel(int id);
        void set_env_path(const std::string&);
    private:
        std::string env_path_;
        std::map<int, FILE> map;
};
