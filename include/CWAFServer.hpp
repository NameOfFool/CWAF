#pragma once
#include <Poco/Util/Application.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/Option.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Util/ServerApplication.h>
#include <vector>
#include <string>

class CWAFServer : public Poco::Util::ServerApplication
{
private:
  std::string _bindIP = "";
  unsigned short _port = 0;

public:
  static std::string TargetHost;
  static unsigned short TargetPort;

protected:
  void initialize(Poco::Util::Application &self) override;
  void uninitialize() override;
  void defineOptions(Poco::Util::OptionSet &options) override;
  void handleOption(const std::string &name, const std::string &value) override;
  int main(const std::vector<std::string> &args) override;
};

class CWAFRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
  Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &) override;
};
