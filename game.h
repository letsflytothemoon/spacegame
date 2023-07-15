#pragma once
#include "guid.h"
#include "property.h"
#include "vector.h"
#include "serializable.h"

using namespace vector;

typedef Vector<int, int> VectorT;

template<>
struct Format<Guid>
{
    static void format(std::ostream& stream, const Guid& guid)
    { stream << "\"" << guid.operator std::string() << "\""; }

    static void format(std::wostream& wstream, const Guid& guid)
    { wstream << L"\"" << guid.operator std::wstring() << L"\""; }
};

template<>
struct Format<VectorT>
{
    static void format(std::ostream& stream, const VectorT& vector)
    { stream << "{ \"x\" : " << vector.at<0>() << ", \"y\" : " << vector.at<1>() << "}"; }

    static void format(std::wostream& wstream, const VectorT& vector)
    { wstream << L"{ \"x\" : " << vector.at<0>() << L", \"y\" : " << vector.at<1>() << L"}"; }
};

struct IdId{ typedef Guid Type; static const char* name() { return "id"; } };

template <class ... ChildPropIds>
class SpaceGameObject : public Serializable<IdId, ChildPropIds ...>
{
public:
    Property<IdId>& Id;
    SpaceGameObject() :
    Id(*this)
    {}
};

struct PositionId { typedef VectorT Type; static const char* name() { return "position"; } };
struct SpeedId    { typedef VectorT Type; static const char* name() { return "speed"; } };
struct MassId     { typedef int     Type; static const char* name() { return "mass"; } };
struct RadiusId   { typedef int     Type; static const char* name() { return "radius"; } };

template <class ... ChildProps>
class PhysicalObject : public SpaceGameObject<PositionId, SpeedId, MassId, RadiusId, ChildProps ...>
{
public:
    VectorT& Position;
    Property<SpeedId>&    Speed;
    Property<MassId>&     Mass;
    Property<RadiusId>&   Radius;

    PhysicalObject() :
    Position((Property<PositionId>&)*this),
    Speed((Property<SpeedId>&)*this),
    Mass(*this),
    Radius(*this)
    {}
};

