#include <boost/beast/core.hpp>
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

namespace beast = boost::beast;
namespace asio  = boost::asio;
namespace ip    = asio::ip;
using     tcp   = ip::tcp;

void SpaceGameServer::Run()
{
    std::thread(&SpaceGameServer::AcceptLoop, this).detach();
    std::thread(&SpaceGameServer::GameLoop,   this).detach();
}

void SpaceGameServer::AcceptLoop()
{
    try
    {
        ip::address address = ip::make_address("0.0.0.0");
        unsigned short port = 12345;

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

bool SpaceGameServer::CheckSocketQueueEmpty()
{
    std::lock_guard<std::mutex> lock_guard(connectionsQueueMutex);
    return connectionsQueue.empty();
}

tcp::socket SpaceGameServer::GetSocketFromQueue()
{
    while(true)
    {
        while(CheckSocketQueueEmpty())
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        
        std::lock_guard<std::mutex> lock_guard(connectionsQueueMutex);
        if(!connectionsQueue.empty())
        {
            

            struct Poper
            {
                std::queue<tcp::socket>& _queue;
                Poper(std::queue<tcp::socket>& queue) : _queue(queue) { }
                ~Poper() { _queue.pop(); }
            } poper(connectionsQueue);
            return std::move(connectionsQueue.front());
        }
    }
}

routing<char>::StaticDocumentEndPoint* SpaceGameServer::StaticDocumentEndPoint(std::string path)
{
    return new routing<char>::StaticDocumentEndPoint(path);
}

routing<char>::StaticDirectoryEndPoint* SpaceGameServer::StaticDirectoryEndPoint(std::string path)
{
    return new routing<char>::StaticDirectoryEndPoint(path);
}

void SpaceGameServer::ApiMethodBalls(routing<char>::HttpRequestContext& context)
{
    std::vector<PhysicalObject> vector;
    {
        std::lock_guard<std::mutex> lock_guard(storageMutex);
        std::map<Guid, PhysicalObject*>& map = GetStorage<PhysicalObject>();


        for(auto i = map.begin(); i != map.end(); i++)
            vector.push_back(*i->second);
    }

    context.responseStream << "[ ";
    for(int i = 0; i < vector.size(); i++)
    {
        if(i > 0) context.responseStream << ", ";
        context.responseStream << vector[i];
    }
    context.responseStream << " ]";
}