#include "../src/configuration.hh"
#include "../src/server_config.hh"

int main()
{
  Configuration conf("server.conf");
  conf.fill_configuration();
  conf.print();
  return 0;
}
