#pragma once

#include <string>

#include "server_config.hh"
#include "request.hh"

class CgiHandler
{
    public:
        CgiHandler(std::string env_path);
        int process(Request& req_obj, ServerConfig& conf, std::string& rep);
        void cancel();
    private:
        std::string env_path;
};