#pragma once

#include <string>

class Request;

class ServerConfig;

class CgiHandler
{
    public:
        CgiHandler(std::string env_path);
        //int process(Request& req_obj, ServerConfig& conf, std::string& rep);
        //void cancel();
    private:
        std::string env_path;
};
