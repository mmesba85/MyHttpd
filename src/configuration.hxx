#include "configuration.hh"
#include <map>
#include <array>
#include <vector>

Configuration::Configuration(const std::string filename)
  : dry_run_(false)
{
  stream_.open(filename);
  if (!stream_.is_open())
    exit(2);
}

Configuration::~Configuration()
{
  stream_.close();
}

int Configuration::fill_configuration()
{
  toml::Data data;
  try
  {
    data = toml::parse(stream_);
  }
  catch(toml::syntax_error)
  {
    exit(2);
  }

  try
  {
    log_file_ = toml::get<std::string>(data.at("log_file"));
  }
  catch(const std::exception& e)
  {
    log_file_ = "";
  }

  auto table = toml::get<std::vector<toml::Table>>(data.at("server"));
  for (auto it = table.begin(); it != table.end(); ++it)
  {
    ServerConfig one = ServerConfig();
    try
    {
      auto server_name =  toml::get<std::string>(it->at("server_name"));
      one.set_server_name(server_name);

      auto ip =  toml::get<std::string>(it->at("ip"));
      one.set_ip(ip);

      auto port =  toml::get<std::string>(it->at("port"));
      one.set_port(port);

      auto root_dir =  toml::get<std::string>(it->at("root_dir"));
      one.set_root_dir(root_dir);
    }
    catch(toml::type_error)
    {
      exit(2);
    }

    try
    {
      auto gzip = toml::get<std::string>(it->at("gzip"));
      one.insert_to_configuration("gzip", gzip);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto log = toml::get<std::string>(it->at("log"));
      one.insert_to_configuration("log", log);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto cgi_ext = toml::get<std::string>(it->at("cgi_ext"));
      one.insert_to_configuration("cgi_ext", cgi_ext);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto ssl_certificate = toml::get<std::string>(it->at("ssl_certificate"));
      one.insert_to_configuration("ssl_certificate", ssl_certificate);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto ssl_certificate_key =
                toml::get<std::string>(it->at("ssl_certificate_key"));
      one.insert_to_configuration("ssl_certificate_key", ssl_certificate_key);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto basic_auth = toml::get<std::string>(it->at("basic_auth"));
      one.insert_to_configuration("basic_auth", basic_auth);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto basic_auth_file = toml::get<std::string>(it->at("basic_auth_file"));
      one.insert_to_configuration("basic_auth_file", basic_auth_file);
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto error =
          toml::get<toml::Array<toml::Array<toml::String>>>(it->at("error"));
      for (auto iu = error.begin(); iu != error.end(); ++iu)
        one.insert_to_error(iu->at(0), iu->at(1));
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    try
    {
      auto error =
          toml::get<toml::Array<toml::Array<toml::String>>>(it->at("proxy_pass"));
      for (auto iu = error.begin(); iu != error.end(); ++iu)
        one.insert_to_proxy_pass(iu->at(0), iu->at(1));
    }
    catch(std::out_of_range)
    {}
    catch(toml::type_error)
    {}

    list_server_.push_back(one);
  }
  return 0;
}

void Configuration::print()
{
  std::cout << "dry_run = " << dry_run_ << '\n';
  std::cout << "log_file = " << log_file_ << '\n';
  std::cout << std::endl;
  for (auto it=list_server_.begin(); it!=list_server_.end(); ++it)
  {
    it->print();
    std::cout << std::endl;
  }
}

std::vector<ServerConfig>& Configuration::get_list()
{
  return list_server_;
}

std::string& Configuration::get_log_file()
{
  return log_file_;
}
