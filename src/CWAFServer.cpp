#include "CWAFServer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "CWAFRequestHandler.hpp"
#include <iostream>

using namespace std;
using namespace Poco::Util;
using namespace Poco::Net;

int CWAFServer::main(const vector<string>& args) {
        // Значения по умолчанию
        if (_bindIP.empty()) _bindIP = "0.0.0.0";
        if (_port == 0) _port = 8080;

        SocketAddress sa(_bindIP, _port);
        ServerSocket svs(sa);

        HTTPServer server(new CWAFRequestHandlerFactory, svs, new HTTPServerParams);
        server.start();

        cout << "WAF запущен на " << _bindIP << ":" << _port << endl;
        waitForTerminationRequest();
        server.stop();

        return Application::EXIT_OK;
    }

void CWAFServer::initialize(Application& self) {
        loadConfiguration(); // Загрузка конфигурации (опционально)
        ServerApplication::initialize(self);
    }

void CWAFServer::defineOptions(OptionSet& options) {
        ServerApplication::defineOptions(options);

        // Добавляем кастомные опции
        options.addOption(
            Option("ip", "i", "IP-адрес для биндинга (по умолчанию: 0.0.0.0)")
                .required(false)
                .argument("ip")
                .binding("ip")
        );

        options.addOption(
            Option("port", "p", "Порт для прослушивания (по умолчанию: 8080)")
                .required(false)
                .argument("port")
                .binding("port")
        );
    }
void CWAFServer::handleOption(const string& name, const string& value) {
        ServerApplication::handleOption(name, value);

        if (name == "ip") {
            _bindIP = value;
        } else if (name == "port") {
            _port = static_cast<uint16_t>(stoi(value));
        }
    }