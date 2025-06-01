#pragma once
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <string>
#include <array>
#include <re2/re2.h>

class CWAFRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
  RE2 sqlInjectionPattern_ = RE2("(?i)(union\\s+select|drop\\s+table|--|or\\s+1=1)");
  RE2 xssPattern_ = RE2("<script>|onerror=|javascript:");
  RE2 commandInjectionPattern_ = RE2("(;|&&|\\||wget|curl)");
  bool isMalicious(const Poco::Net::HTTPServerRequest &request, const std::string &body);

  std::string _targetHost;
  unsigned short _targetPort;

public:
  CWAFRequestHandler(const std::string &targetHost, unsigned short targetPort);

  void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) override;
  void forwardRequest(Poco::Net::HTTPServerRequest &clientRequest, Poco::Net::HTTPServerResponse &clientResponse);
};

