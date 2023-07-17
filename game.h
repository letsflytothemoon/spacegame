#pragma once
#include "guid.h"
#include "property.h"
#include "vector.h"
#include "serializable.h"

using namespace vector;

typedef Vector<int, int> VectorT;




std::ostream& operator <<(std::ostream& stream, const VectorT& vector)
{
    stream << "{ \"x\" : " << vector.at<0>() << ", \"y\" : " << vector.at<1>() << " }";
    return stream;
}

std::wostream& operator <<(std::wostream& stream, const VectorT& vector)
{
    stream << L"{ \"x\" : " << vector.at<0>() << L", \"y\" : " << vector.at<1>() << L" }";
    return stream;
}

struct IdId
{
    typedef Guid Type;
    static const char* Name() { return "id"; }
    static Type DefaultValue() { return Guid::New(); }
};

struct PositionId
{
    typedef VectorT Type;
    static const char* Name() { return "position"; }
    static Type DefaultValue() { return VectorT(0, 0); }
};

struct SpeedId
{
    typedef VectorT Type;
    static const char* Name() { return "speed"; }
    static Type DefaultValue() { return VectorT(0, 0); }
};

struct MassId
{
    typedef int Type;
    static const char* Name() { return "mass"; }
    static Type DefaultValue() { return 0; }
};

struct RadiusId
{
    typedef int Type;
    static const char* Name() { return "radius"; }
    static Type DefaultValue() { return 0; }
};

template <class ... PropIds>
class SpaceGameObject : protected Property<IdId>
{
public:
    Property<IdId>& Id;
    SpaceGameObject() :
    Id((Property<IdId>&)*this)
    {}
};

template <class Id, class ... PropsIds>
class SpaceGameObject : public SpaceGameObject
{

};

class PhysicalObject : public SpaceGameObject<PositionId, SpeedId, MassId, RadiusId>
{
public:
    VectorT&            Position;
    Property<SpeedId>&  Speed;
    Property<MassId>&   Mass;
    Property<RadiusId>& Radius;

    PhysicalObject() :
    Position((Property<PositionId>&)*this),
    Speed((Property<SpeedId>&)*this),
    Mass(*this),
    Radius(*this)
    {}
};