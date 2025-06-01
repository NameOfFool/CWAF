#include "CWAFRequestHandler.hpp"
#include "spdlog/spdlog.h"
#include <Poco/URI.h>
#include <sstream>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/StreamCopier.h>

using namespace std;
using namespace Poco::Net;

CWAFRequestHandler::CWAFRequestHandler(const std::string &targetHost, unsigned short targetPort)
    : _targetHost(targetHost), _targetPort(targetPort) {}

bool CWAFRequestHandler::isMalicious(const Poco::Net::HTTPServerRequest &request, const std::string &body)
{
    // 1. Проверяем тело запроса (payload) на SQL-инъекцию
    if (RE2::PartialMatch(body, sqlInjectionPattern_))
    {
        spdlog::warn("SQL Injection detected in request URI {}",
                     request.getURI());
        return true;
    }
    // 2. Проверяем тело на XSS
    if (RE2::PartialMatch(body, xssPattern_))
    {
        spdlog::warn("XSS attempt detected in request URI {}",
                     request.getURI());
        return true;
    }
    // 3. Проверяем тело на командную инъекцию
    if (RE2::PartialMatch(body, commandInjectionPattern_))
    {
        spdlog::warn("Command injection detected in request URI {}",
                     request.getURI());
        return true;
    }
    // 4. Если ни одно из правил не сработало — считаем запрос безопасным
    return false;
}
void CWAFRequestHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
{
    // 1. Получаем IP клиента для логов
    std::string clientIP = req.clientAddress().toString();
    spdlog::info("Получен запрос от {}: {}", clientIP, req.getURI());

    // 2. Читаем тело запроса из входного потока
    std::istream &inStream = req.stream();
    std::string body((std::istreambuf_iterator<char>(inStream)),
                     std::istreambuf_iterator<char>());

    // 3. Проверяем запрос на наличие вредоносного содержимого
    if (isMalicious(req, body))
    {
        // 3a. Если обнаружена атака, возвращаем 403 и логируем предупреждение
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_FORBIDDEN);
        resp.setContentType("text/plain");
        std::ostream &outStream = resp.send();
        outStream << "403 Forbidden: WAF blocked your request";
        return;
    }
    forwardRequest(req, resp);
}

void CWAFRequestHandler::forwardRequest(Poco::Net::HTTPServerRequest &clientRequest, Poco::Net::HTTPServerResponse &clientResponse)
{
    Poco::URI uri("http://" + _targetHost + ":" + std::to_string(_targetPort) + clientRequest.getURI());
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    Poco::Net::HTTPRequest forwardedRequest(clientRequest.getMethod(), uri.getPathEtc(), clientRequest.getVersion());

    for (const auto &header : clientRequest)
    {
        forwardedRequest.set(header.first, header.second);
    }

    std::stringstream bodyStream;
    Poco::StreamCopier::copyStream(clientRequest.stream(), bodyStream);
    std::string body = bodyStream.str();
    forwardedRequest.setContentLength(body.length());

    std::ostream &os = session.sendRequest(forwardedRequest);
    os << body;

    Poco::Net::HTTPResponse forwardedResponse;
    std::istream &rs = session.receiveResponse(forwardedResponse);

    clientResponse.setStatus(forwardedResponse.getStatus());
    clientResponse.setReason(forwardedResponse.getReason());
    for (const auto &header : forwardedResponse)
    {
        clientResponse.set(header.first, header.second);
    }

    Poco::StreamCopier::copyStream(rs, clientResponse.send());
}
