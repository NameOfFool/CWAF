#include <Poco/Util/Application.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/ServerApplication.h>
#include <vector>
#include <string>

class CWAFServer: public Poco::Util::ServerApplication{
private:
  std::string _bindIP = "";
  uint16_t _port = 0;
protected:
  void initialize(Poco::Util::Application& self) override;
  void defineOptions(Poco::Util::OptionSet& options) override;
  void handleOption(const std::string& name, const std::string& value) override;
  int main(const std::vector<std::string>& args) override;
};
