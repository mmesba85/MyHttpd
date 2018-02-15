#include "cgi-handler.hh"

#include "server_config.hh"
#include "response.hh"

CgiHandler::CgiHandler(const std::string& env_path)
    : env_path_(env_path)
{}
