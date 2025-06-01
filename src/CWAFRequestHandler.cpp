#include "CWAFRequestHandler.hpp"

using namespace std;
using namespace Poco::Net;

bool CWAFRequestHandler::isMalicious(const std::string& requestBody){
    for (const auto& rule : WAF_RULES) {
        if (RE2::PartialMatch(requestBody, rule)) {
            return true;
        }
    }
    return false;
}
void CWAFRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp) {
        // Чтение тела запроса
        string requestBody;
        istream& bodyStream = req.stream();
        char buffer[4096];
        while (bodyStream.read(buffer, sizeof(buffer))) {
            requestBody.append(buffer, bodyStream.gcount());
        }

        // Проверка на атаки
        if (isMalicious(requestBody)) {
            // Логируем атаку
            //logAttack(req.clientAddress().toString(), requestBody);

            // Блокируем запрос
            resp.setStatus(HTTPResponse::HTTP_FORBIDDEN);
            resp.send() << "403 Forbidden (WAF Blocked)";
            return;
        }

        // Если запрос безопасен, передаем его дальше (здесь можно добавить проксирование)
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.send() << "200 OK (Safe Request)";
    }
 HTTPRequestHandler* CWAFRequestHandlerFactory::createRequestHandler(const HTTPServerRequest&) {
        return new CWAFRequestHandler;
    }