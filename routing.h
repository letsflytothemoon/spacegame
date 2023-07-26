#pragma once
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
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

    class Router;
    class RouterEndPoint;
    class StaticDirectoryEndPoint;
    class StaticDocumentEndPoint;
    template <class ControllerT, class MethodPtr>
    class ApiEndPoint;    

    class HttpRequestContext
    {
        friend class RouterEndPoint;
        template <class ControllerT, class MethodPtr>
        friend class ApiEndPoint;
    protected:
        std::queue<StringT> route;
        Request request;
        tcp::socket socket;
    public:
        std::basic_stringstream<CharT> responseStream;

        HttpRequestContext(Request _request, tcp::socket _socket) :
        request (std::move(_request)),
        socket  (std::move(_socket ))
        {
            beast::basic_flat_buffer<std::allocator<CharT>> buffer {8192};
            
            http::read(socket, buffer, request);

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

        ~RouterNode()
        {
            for(auto i = routes.begin(); i != routes.end(); i++)
                delete i->second;
        }

        RouterEndPoint& GetEndPoint(HttpRequestContext& context) override
        { return routes[context.GetRouteNode()]->GetEndPoint(context); }
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
        { }
    };

    class StaticDocumentEndPoint : public RouterEndPoint
    {
        StringT fileName;
    public:
        StaticDocumentEndPoint(StringT _fileName) : fileName(std::move(_fileName))
        { }
        
        void SendResponse(HttpRequestContext& context) override
        { }
    };
    
    template <class ControllerT, class MethodPtr>
    class ApiEndPoint : public RouterEndPoint
    {
        const MethodPtr& methodPtr;
        ControllerT* ptrController;
    public:
        ApiEndPoint(ControllerT* _ptrController, const MethodPtr& _methodPtr) :
        ptrController(_ptrController),
        methodPtr(_methodPtr)
        { }

        void SendResponse(HttpRequestContext& context) override
        {
            std::invoke(methodPtr, ptrController, context);
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