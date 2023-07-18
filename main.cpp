#include <iostream>
#include "vector.h"
#include "guid.h"
#include "game.h"

int main()
{
    
    
    PhysicalObject physicalObject;
    SpaceGameObject& ref = physicalObject;
    
    std::cout << physicalObject << std::endl;
    std::cout << ref << std::endl;

    

    return 0;
}