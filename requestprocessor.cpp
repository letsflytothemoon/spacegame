#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include "spacegameserver.h"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace asio  = boost::asio;
namespace ip    = asio::ip;
using     tcp   = ip::tcp;

using Request  = http::request<http::dynamic_body>;
using Response = http::response<http::dynamic_body>;
using Buffer   = beast::flat_buffer;

void RequestProcessor::ProcessLoop()
{
    while(true)
    {
        try
        {
            tcp::socket socket = std::move(gameServer.GetSocketFromQueue());

            Request request;
            Buffer buffer{8192};

            http::read(
                socket,
                buffer,
                request);

            routing<char>::HttpRequestContext httpRequestContext(
                std::move(request),
                std::move(socket));
            
            gameServer
                .router
                .GetEndPoint(httpRequestContext)
                .SendResponse(httpRequestContext);
        }
        catch(std::exception const& exception)
        {
            std::cerr << "process request error: " << exception.what() << std::endl;
        }
    }
}