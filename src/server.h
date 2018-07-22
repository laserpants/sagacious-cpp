#pragma once

#include <cassert>
#include <server_http.hpp>

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

namespace sagacious
{
    namespace http
    {
        enum method { GET = 0, POST, PUT, PATCH, DELETE };

        class request
        {
        public:
            explicit request(std::shared_ptr<HttpServer::Request> request);

        private:
            std::shared_ptr<HttpServer::Request> _r;
        };

        class response
        {
        public:
            explicit response(std::shared_ptr<HttpServer::Response> response);

            void send(SimpleWeb::StatusCode code, std::istream &body);
            void send(SimpleWeb::StatusCode code, std::string body);

            void send_json(SimpleWeb::StatusCode code, std::istream &body);
            void send_json(SimpleWeb::StatusCode code, std::string body);

        private:
            std::shared_ptr<HttpServer::Response> _r;
            SimpleWeb::CaseInsensitiveMultimap    _headers;
        };

        std::string to_string(const method &method);
    }

    class server
    {
    public:
        server();
        ~server();

        void run();
        void run(short port);

        inline void set_port(short port) { _server.config.port = port; }

        void on(
            const http::method &method, 
            std::string pattern, 
            std::function<void(http::request req, http::response res)> handler
        );

    private:
        SimpleWeb::Server<SimpleWeb::HTTP> _server;
    };
}
