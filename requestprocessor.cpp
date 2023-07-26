#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <memory>
#include <string>
#include <sstream>
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
            Response response;
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

            /*
            std::vector<std::string> path;
            boost:split(path, request.target(), boost::is_any_of("/"));
            path.erase(path.begin());
            
            response.version(request.version());
            response.keep_alive(false);
            
            response.set(http::field::content_type, "text/html");

            std::vector<PhysicalObject> vector;
            {
                std::lock_guard<std::mutex> lock_guard(gameServer.storageMutex);
                std::map<Guid, PhysicalObject*>& map = gameServer.GetStorage<PhysicalObject>();
                
                
                for(auto i = map.begin(); i != map.end(); i++)
                    vector.push_back(*i->second);
            }

            std::stringstream stringStream;
            stringStream << "[ ";
            for(int i = 0; i < vector.size(); i++)
            {
                if(i > 0) stringStream << ", ";
                stringStream << vector[i];
            }
            stringStream << " ]";

            beast::ostream(response.body()) << stringStream.str();

            response.content_length(response.body().size());
            http::write(
                socket,
                response);
            */
        }
        catch(std::exception const& exception)
        {
            std::cerr << "process request error: " << exception.what() << std::endl;
        }
    }
}