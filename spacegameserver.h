#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <boost/asio/ip/tcp.hpp>
#include <map>
#include "game.h"
#include "storage.h"

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
    

    void AcceptLoop();
    void GameLoop();
public:
    template <class T, class ... Args>
    KeyType Add(Args ... args)
    { return StorageT::Add<T>(args ...); }
    
    void Run();
    tcp::socket GetSocketFromQueue();
};