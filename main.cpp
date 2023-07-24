#include <iostream>
#include <string>
#include "spacegameserver.h"



template <class T, class Action>
void ForEach(T some, Action action)
{
    for(auto i = some.begin(); i != some.end(); i++)
        action(*i);
}

int main()
{
    SpaceGameServer spaceGameServer;
    spaceGameServer.Add<PhysicalObject>(
        VectorT(  0,  1000),
        VectorT( 10,     0),
        1,
        10
    );

    spaceGameServer.Add<PhysicalObject>(
        VectorT(  0, -1000),
        VectorT(-10,    10),
        1,
        10
    );

    spaceGameServer.Add<PhysicalObject>(
        VectorT(100,     0),
        VectorT(  0,    -1),
        10,
        20
    );

    spaceGameServer.Add<PhysicalObject>(
        VectorT(-1000,   0),
        VectorT(    0,   2),
        5,
        15
    );
    
    RequestProcessor requestProcessor(spaceGameServer);
    spaceGameServer.Run();
    
    std::string command;
    while(command != "exit")
        std::cin >> command;
    return 0;
}