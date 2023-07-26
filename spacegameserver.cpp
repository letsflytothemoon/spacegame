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

routing<char>::StaticDocumentEndPoint* SpaceGameServer::StaticDocumentEndPoint(std::string path)
{
    return new routing<char>::StaticDocumentEndPoint(path);
}

routing<char>::StaticDirectoryEndPoint* SpaceGameServer::StaticDirectoryEndPoint(std::string path)
{
    return new routing<char>::StaticDirectoryEndPoint(path);
}

routing<char>::RouterNode* SpaceGameServer::RouterNode(std::initializer_list<std::pair<const std::string, routing<char>::Router*>> init)
{
    return new routing<char>::RouterNode(init);
}