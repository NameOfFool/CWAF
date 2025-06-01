#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <string>
#include <array>
#include <re2/re2.h>

class CWAFRequestHandler : public Poco::Net::HTTPRequestHandler {
private:
  const std::array<RE2, 3> WAF_RULES{
    RE2("'\\s*OR\\s*1=1\\s*--"),
    RE2("<script>"),
    RE2("UNION\\s+SELECT")
  };
  bool isMalicious(const std::string& requestBody);
public:
  void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& resp) override;
};
class CWAFRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory{
  public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest&) override;
};
