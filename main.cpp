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
        Position(VectorT(  0,  1000)),
        Speed   (VectorT( 10,     0)),
        Mass    (1),
        Radius  (10)
    );

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(  0, -1000)),
        Speed   (VectorT(-10,    10)),
        Mass    (1),
        Radius  (10)
    );

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(100,     0)),
        Speed   (VectorT(  0,    -1)),
        Mass    (10),
        Radius  (20)
    );

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(-1000,   0)),
        Speed   (VectorT(    0,   2)),
        Mass    (5),
        Radius  (15)
    );
    
    RequestProcessor requestProcessor(spaceGameServer);
    spaceGameServer.Run();
    
    std::string command;
    while(command != "exit")
        std::cin >> command;
    return 0;
}