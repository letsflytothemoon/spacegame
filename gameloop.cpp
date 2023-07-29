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

std::ostream& operator<<(std::ostream& stream, const VectorT& vector)
{
    return JsonFormatter<VectorT>::PutToStream(stream, vector);
}

void SpaceGameServer::GameLoop()
{
    static auto vectorLength = [](const VectorT& vector)
    { return std::sqrt(vector.at<0>() * vector.at<0>() + vector.at<1>() * vector.at<1>()); };

    static auto gravitate = [](PhysicalObject& l, PhysicalObject& r)
    {
        int forceMassMul = l.mass * r.mass / 10; // / pow(length(...), 2)

        VectorT positionDifference = r.position - l.position;

        double posDiffLength = vectorLength(positionDifference);

        VectorT force = positionDifference * (forceMassMul / posDiffLength);

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