#include "CWAFServer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "CWAFRequestHandler.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <Poco/Net/NetException.h>

using namespace std;
using namespace Poco::Util;
using namespace Poco::Net;

std::string CWAFServer::TargetHost = "localhost";
unsigned short CWAFServer::TargetPort = 8080;

HTTPRequestHandler *CWAFRequestHandlerFactory::createRequestHandler(const HTTPServerRequest &)
{
    return new CWAFRequestHandler(CWAFServer::TargetHost, CWAFServer::TargetPort);
}
int CWAFServer::main(const vector<string> &args)
{
    try
    {
        // Значения по умолчанию
        if (_bindIP.empty())
            _bindIP = "127.0.0.1";
        if (_port == 0)
            _port = 8000;

        Poco::Net::ServerSocket serverSocket(_port);

        Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams;
        params->setMaxQueued(100);    // максимум запросов в очереди
        params->setMaxThreads(16);    // максимум рабочих потоков
        
        Poco::Net::HTTPServer wafServer(
            new CWAFRequestHandlerFactory(), 
            serverSocket,
            params
        );

        wafServer.start();
        spdlog::info("WAF Server запущен на порту {}", _port);

        waitForTerminationRequest();
        spdlog::info("Получен сигнал завершения работы. Останавливаем сервер...");

        wafServer.stop();
        spdlog::info("WAF Server остановлен.");

        return Application::EXIT_OK;
    }
    catch (const Poco::Net::NetException &ne)
    {
        spdlog::critical("Ошибка сетевого взаимодействия: {}", ne.displayText());
        return Application::EXIT_SOFTWARE;
    }
    catch (const std::exception &ex)
    {
        spdlog::critical("Непредвиденное исключение: {}", ex.what());
        return Application::EXIT_SOFTWARE;
    }
}

void CWAFServer::initialize(Application &self)
{
    spdlog::info("WAF запускается...");
    ServerApplication::initialize(self);
}

void CWAFServer::uninitialize()
{
    spdlog::info("WAF завершает работу...");
    ServerApplication::uninitialize();
}

void CWAFServer::defineOptions(OptionSet &options)
{
    ServerApplication::defineOptions(options);

    // Добавляем кастомные опции
    options.addOption(
        Option("ip", "i", "IP for proxy (default: 127.0.0.1)")
            .required(false)
            .argument("ip")
            .binding("ip"));

    options.addOption(
        Option("port", "p", "Port for proxy (default: 8000)")
            .required(false)
            .argument("port")
            .binding("port"));
    options.addOption(
        Option("targetHost", "H", "Target host to proxy requests to")
            .required(false)
            .repeatable(false)
            .argument("targetHost"));

    options.addOption(
        Option("targetPort", "P", "Target port to proxy requests to")
            .required(false)
            .repeatable(false)
            .argument("targetPort"));
}
void CWAFServer::handleOption(const string &name, const string &value)
{
    ServerApplication::handleOption(name, value);

    if (name == "ip")
    {
        _bindIP = value;
    }
    else if (name == "port")
    {
        _port = static_cast<unsigned short>(stoi(value));
    }
    else if (name == "targetHost")
    {
        TargetHost = value;
    }
    else if (name == "targetPort")
    {
        TargetPort = static_cast<unsigned short>(stoi(value));
    }
}