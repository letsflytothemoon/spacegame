#pragma once
#include <boost/core/demangle.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <string>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace asio  = boost::asio;
namespace ip    = asio::ip;
using     tcp   = ip::tcp;

template <class CharT>
struct routing
{
    using Request  = http::request<http::dynamic_body>;
    using Response = http::response<http::dynamic_body>;

    typedef std::basic_string<CharT> StringT;
    class NotFoundException : public std::exception
    {
    public:
        const char* what() const noexcept override
        { return "404 - not found"; }
    };

    class Router;
    class RouterEndPoint;
    class StaticDirectoryEndPoint;
    class StaticDocumentEndPoint;
    template <class Callable, class Controller>
    class ApiEndPoint;    

    class HttpRequestContext
    {
    public:
        std::queue<StringT> route;
        Request request;
        tcp::socket socket;
        std::basic_stringstream<CharT> responseStream;

        HttpRequestContext(Request _request, tcp::socket _socket) :
        request (std::move(_request)),
        socket  (std::move(_socket ))
        {
            std::vector<StringT> path;

            boost::split(path, request.target(), boost::is_any_of("/"));

            path.erase(path.begin());

            for(auto i = path.begin(); i != path.end(); i++)
                route.push(*i);
        }

        StringT GetRouteNode()
        {
            StringT result(std::move(route.front()));
            route.pop();
            return result;
        }
    };

    class Router
    {
    public:
        virtual RouterEndPoint& GetEndPoint(HttpRequestContext&) = 0;
        virtual ~Router() { }
    };


    class RouterNode : public Router
    {
        std::map<StringT, Router*> routes;
    public:
        RouterNode(std::initializer_list<std::pair<const StringT, Router*>> init) :
        routes(init)
        { }

        template <class ... Args>
        RouterNode(Args&& ... args) :
        routes(args ...)
        { }

        ~RouterNode()
        {
            for(auto i = routes.begin(); i != routes.end(); i++)
                delete i->second;
        }

        RouterEndPoint& GetEndPoint(HttpRequestContext& context) override
        {
            StringT pathStep = context.GetRouteNode();
            auto i = routes.find(pathStep);
            if(i == routes.end())
            {
                context.socket.shutdown(tcp::socket::shutdown_send);
                throw NotFoundException();
            }
            return i->second->GetEndPoint(context);
        }
    };

    class RouterEndPoint : public Router
    {
    public:
        RouterEndPoint& GetEndPoint(HttpRequestContext& context) override
        { return *this; }

        virtual void SendResponse(HttpRequestContext&) = 0;
    };

    class StaticDirectoryEndPoint : public RouterEndPoint
    {
        StringT dirName;
    public:
        StaticDirectoryEndPoint(StringT _dirName) : dirName(std::move(_dirName))
        { }

        void SendResponse(HttpRequestContext& context) override
        {
            std::basic_stringstream<CharT> pathStringStream;
            pathStringStream << dirName;

            while(!context.route.empty())
            {

                StringT pathStep = context.route.front();
                context.route.pop();
                if(pathStep == "..")
                {
                    context.socket.shutdown(tcp::socket::shutdown_send);
                    throw NotFoundException();
                }
                pathStringStream << JsonSymbol<CharT>::Slash << pathStep;
            }

            std::ifstream fstream(pathStringStream.str());
            context.responseStream << fstream.rdbuf();

            Response response;
            response.version(context.request.version());
            response.keep_alive(false);

            beast::ostream(response.body()) << context.responseStream.str();

            response.content_length(response.body().size());

            http::write(
                context.socket,
                response);
            
            context.socket.shutdown(tcp::socket::shutdown_send);
        }
    };

    class StaticDocumentEndPoint : public RouterEndPoint
    {
        StringT _fileName;
    public:
        StaticDocumentEndPoint(StringT fileName) : _fileName(std::move(fileName))
        { }
        
        void SendResponse(HttpRequestContext& context) override
        {
            std::ifstream fstream(_fileName);
            context.responseStream << fstream.rdbuf();

            Response response;
            response.version(context.request.version());
            response.keep_alive(false);
            response.set(http::field::content_type, "text/html");

            beast::ostream(response.body()) << context.responseStream.str();

            response.content_length(response.body().size());

            http::write(
                context.socket,
                response);
            
            context.socket.shutdown(tcp::socket::shutdown_send);
        }
    };
    
    template <class Callable, class Controller>
    class ApiEndPoint : public RouterEndPoint
    {
        Controller _controller;
        Callable   _callable;
    public:
        ApiEndPoint(Callable callable, Controller controller) :
        _callable  (callable),
        _controller(controller)
        { }

        void SendResponse(HttpRequestContext& context) override
        {
            (_controller->*_callable)(context);

            Response response;
            response.version(context.request.version());
            response.keep_alive(false);
            response.set(http::field::content_type, "application/json");

            beast::ostream(response.body()) << context.responseStream.str();

            response.content_length(response.body().size());

            http::write(
                context.socket,
                response);
        }
    };
};