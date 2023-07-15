#include <iostream>
#include "vector.h"
#include "property.h"
#include "guid.h"
#include "game.h"

int main(int argc, char**argv)
{
    PhysicalObject physiaclObject;
    physiaclObject.Id = Guid::New();
    physiaclObject.Mass = 100;
    
    physiaclObject.Speed = VectorT(10, 33);
    physiaclObject.Position.at<0>() = 74;
    physiaclObject.Position.at<1>() = 77;

    std::cout << physiaclObject.Serialize() << std::endl;
    return 0;
}