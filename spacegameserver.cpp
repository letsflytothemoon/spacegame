#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <sstream>
#include "spacegameserver.h"
#include "game.h"
#include "serializable.h"

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
        }
        catch(std::exception const& exception)
        {
            std::cerr << "process request error: " << exception.what() << std::endl;
        }
    }
}

void SpaceGameServer::Run()
{
    std::thread(&SpaceGameServer::AcceptLoop, this).detach();
    std::thread(&SpaceGameServer::GameLoop,   this).detach();
}

void SpaceGameServer::GameLoop()
{
    static auto vectorLength = [](const VectorT& vector)
    { return std::sqrt(vector.at<0>() * vector.at<0>() + vector.at<1>() * vector.at<1>()); };

    static auto gravitate = [](PhysicalObject& l, PhysicalObject& r)
    {
        int forceMassMul = l.mass * r.mass; // / pow(length(...), 2)
        
        VectorT positionDifference = r.position - l.position;
        double posDiffLength = vectorLength(positionDifference);

        VectorT force = positionDifference * (forceMassMul * 10 / posDiffLength);

        l.acceleration += force / l.mass;
        r.acceleration -= force / r.mass;
    };

    std::chrono::time_point nextTimePoint = std::chrono::system_clock::now();
    while(true)
    {
        while(nextTimePoint > std::chrono::system_clock::now()) {}
        nextTimePoint += std::chrono::milliseconds(30);
        try
        {
            std::lock_guard<std::mutex> lock_guard(storageMutex);

            std::map<Guid, PhysicalObject*>& map = GetStorage<PhysicalObject>();

            for(auto i = map.begin(); i != map.end(); i++)
            {
                //clear cached
                i->second->acceleration = VectorT(0, 0);
            }

            for(auto i = map.begin(); i != map.end(); i++)
                //for(auto j = [&](){ auto j = i; j++; return j; }(); j != map.end(); j++)
                for(auto j = [](auto i){ return ++i; }(i); j != map.end(); j++)
                {
                    PhysicalObject& l = *i->second;
                    PhysicalObject& r = *j->second;
                    //gravitate or some
                    gravitate(l, r);
                }
            
            for(auto i = map.begin(); i != map.end(); i++)
            {
                //update
                PhysicalObject& object = *i->second;
                object.speed += object.acceleration;
                object.position += object.speed;
            }
        }
        catch(std::exception const& e)
        {
            std::cerr << "gameloop error: " << e.what() << std::endl;
        }
    }
}

void SpaceGameServer::AcceptLoop()
{
    try
    {
        ip::address address = ip::make_address("0.0.0.0");
        unsigned short port = 80;

        asio::io_context io_context{1};

        tcp::acceptor acceptor {io_context, { address, port}};
        while(true)
        {
            tcp::socket socket{io_context};
            beast::error_code error_code;
            acceptor.accept(socket, error_code);
            {
                std::lock_guard<std::mutex> lock_guard(connectionsQueueMutex);
                connectionsQueue.push(std::move(socket));
            }
        }
    }
    catch(std::exception const& e)
    {
        std::cerr << "acceptloop error: " << e.what() << std::endl;
    }
}

tcp::socket SpaceGameServer::GetSocketFromQueue()
{
    while(true)
    {
        {
            std::lock_guard<std::mutex> lock_guard(connectionsQueueMutex);
            if(!connectionsQueue.empty())
            {
                tcp::socket result = std::move(connectionsQueue.front());
                connectionsQueue.pop();
                return result;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}