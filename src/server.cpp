#include "server.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

namespace sagacious
{
    namespace http
    {
        request::request(std::shared_ptr<HttpServer::Request> request)
          : _r(request)
        {
        }

        response::response(std::shared_ptr<HttpServer::Response> response)
          : _r(response),
            _headers{}
        {
        }

        void response::send(SimpleWeb::StatusCode code, std::string body)
        {
            _headers.emplace("Content-Length", std::to_string(body.length()));
            _r->write(code, body, _headers);
        }

        void response::send(SimpleWeb::StatusCode code, std::istream &body)
        {
            _r->write(code, body, _headers);
        }

        void response::send_json(SimpleWeb::StatusCode code, std::string body)
        {
            _headers.emplace("Content-Type", "application/json");
            send(code, body);
        }

        void response::send_json(SimpleWeb::StatusCode code, std::istream &body)
        {
            _headers.emplace("Content-Type", "application/json");
            send(code, body);
        }

        std::string to_string(const method &method)
        {
            switch (method)
            {
            case POST:
                return "POST";
            case PUT:
                return "PUT";
            case PATCH:
                return "PATCH";
            case DELETE:
                return "DELETE";
            default:
                assert(method::GET == method);
                return "GET";
            }
        }
    }

    server::server() : _server{}
    {
        set_port(9080);
    }

    server::~server()
    {
    }

    void server::run()
    {
        _server.start();
    }

    void server::run(short port)
    {
        set_port(port);
        run();
    }

    void server::on(
        const http::method &method, 
        std::string pattern, 
        std::function<void(http::request req, http::response res)> handler)
    {
        _server.resource[pattern][http::to_string(method)] = [handler](
            std::shared_ptr<HttpServer::Response> response, 
            std::shared_ptr<HttpServer::Request> request) 
        {
            handler(http::request{request}, http::response{response});
        };
    }
}
