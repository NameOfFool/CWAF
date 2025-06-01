#include <Poco/Poco.h>
#include <CWAFServer.hpp>

int main(int argc, char **argv)
{
  CWAFServer app;
  return app.run(argc, argv);
}
