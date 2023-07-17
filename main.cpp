#include <iostream>
#include "vector.h"
#include "property.h"
#include "guid.h"
#include "game.h"

int main()
{
    SpaceGameObject spaceGameObject;
    spaceGameObject.Id = Guid::New();

    PhysicalObject physicalObject;

    physicalObject.GetProp<IdId>() = Guid::New();
    physicalObject.GetProp<MassId>() = 100;
    physicalObject.GetProp<SpeedId>() = VectorT(10, 33);
    physicalObject.GetProp<PositionId>() = VectorT(12,77);

    //SpaceGameObject<>& some = physicalObject;

    std::wcout << spaceGameObject << std::endl;
    std::wcout << physicalObject << std::endl;

    return 0;
}