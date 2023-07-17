#pragma once
#include "guid.h"
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

struct PropId
{
    typedef Guid Type;
    Type Id;
    static const char* Name() { return "id"; }
    PropId() : Id(0, 0, 0, 0) { }
    operator Type&() { return Id; }
    operator const Type&() const { return Id; }
};

struct PropPosition
{
    typedef VectorT Type;
    Type Position;
    static const char* Name() { return "position"; }
    PropPosition() : Position(0, 0) { }
    operator Type&() { return Position; }
    operator const Type&() const { return Position; }
};

struct PropSpeed
{
    typedef VectorT Type;
    Type Speed;
    static const char* Name() { return "speed"; }
    PropSpeed() : Speed(0, 0) { }
    operator Type&() { return Speed; }
    operator const Type&() const { return Speed; }
};

struct PropMass
{
    typedef int Type;
    Type Mass;
    static const char* Name() { return "mass"; }
    PropMass() : Mass(0) { }
    operator Type&() { return Mass; }
    operator const Type&() const { return Mass; }
};

struct PropRadius
{
    typedef int Type;
    Type Radius;
    static const char* Name() { return "radius"; }
    PropRadius() : Radius(0) { }
    operator Type&() { return Radius; }
    operator const Type&() const { return Radius; }
};

//----------------------------------------------------------------------------
class SpaceGameObject : public Inherits<PropId>, public Serializable
{
public:
    typedef TypesList<PropId> PropTypes;
    void Serialize(std::ostream& stream) const override
    { Serializer<PropTypes>::Do(stream, *this); }

    void Serialize(std::wostream& stream) const override
    { Serializer<PropTypes>::Do(stream, *this); }
};



class PhysicalObject : public Inherits<SpaceGameObject, PropPosition, PropSpeed, PropMass, PropRadius>
{
public:
    typedef PropTypes::Concat<PropPosition, PropSpeed, PropMass, PropRadius>::Result PropTypes;
    void Serialize(std::ostream& stream) const override
    { Serializer<PropTypes>::Do(stream, *this); }

    void Serialize(std::wostream& stream) const override
    { Serializer<PropTypes>::Do(stream, *this); }
};