#include <iostream>
#include <string>
#include "spacegameserver.h"

int main()
{
    SpaceGameServer spaceGameServer;

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(  0, 1000)),
        Speed   (VectorT( 10,    0)),
        Mass    (10),
        Radius  (30)
    );

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(-700, -500)),
        Speed   (VectorT(  -5,    7)),
        Mass    (10),
        Radius  (30)
    );

    spaceGameServer.Add<PhysicalObject>(
        Position(VectorT(700,  -500)),
        Speed   (VectorT( -5,   -7)),
        Mass    (10),
        Radius  (30)
    );
    
    RequestProcessor requestProcessor(spaceGameServer);
    spaceGameServer.Run();
    
    std::string command;
    while(command != "exit")
        std::cin >> command;
    return 0;
}