#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include "game.h"
#include "storage.h"
#include "routing.h"

using tcp = boost::asio::ip::tcp;

class SpaceGameServer;

class RequestProcessor
{
    SpaceGameServer& gameServer;
    std::thread thread;
    void ProcessLoop();
public:
    RequestProcessor(SpaceGameServer& server) :
    gameServer(server),
    thread(&RequestProcessor::ProcessLoop, this)
    {
        thread.detach();
    }
};

struct SpaceGameKeySelector
{
    friend class RequestProcessor;
    typedef Guid KeyType;
    template <class T>
    static Guid Get(const T& object) { return object.id; }
};

typedef typename SpaceGameKeySelector::KeyType KeyType;
typedef Storage<SpaceGameKeySelector, Object, MatherialObject, PhysicalObject> StorageT;

class SpaceGameServer : protected StorageT
{
    friend class RequestProcessor;
    std::queue<tcp::socket> connectionsQueue;
    std::mutex connectionsQueueMutex;
    std::mutex storageMutex;
    
    routing<char>::RouterNode router;

    void AcceptLoop();
    void GameLoop();
public:
    routing<char>::StaticDocumentEndPoint* StaticDocumentEndPoint(std::string path);

    routing<char>::StaticDirectoryEndPoint* StaticDirectoryEndPoint(std::string path);

    routing<char>::RouterNode* RouterNode(std::initializer_list<std::pair<const std::string, routing<char>::Router*>> init);

    template <class MethodPtr>
    routing<char>::ApiEndPoint<SpaceGameServer, MethodPtr>* ApiEndPoint(const MethodPtr& method)
    { return new routing<char>::ApiEndPoint<SpaceGameServer, MethodPtr>(this, method); }

    void ApiMethodBalls(routing<char>::HttpRequestContext& context)
    {
        context.responseStream << ".!.";
    }

    SpaceGameServer() :
    router
    {
        {
            "",
            StaticDocumentEndPoint ( "./web/index.html" )
        },
        {
            "static",
            StaticDirectoryEndPoint ("./web/static/")
        },
        {
            "api",
            RouterNode
            ({
                {
                    "balls",
                    ApiEndPoint(&SpaceGameServer::ApiMethodBalls)
                }
            })
        }
    }
    { }
    
    template <class T, class ... Args>
    typename StorageT::KeyType Add(Args ... args)
    { return ((StorageT&)*this).Add<T>(args ...); }

    void Run();
    tcp::socket GetSocketFromQueue();
};