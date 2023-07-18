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

struct IdPropDescriptor
{
    operator const char*() const { return "id"; }
    operator const wchar_t*() const{ return L"id"; }
    template <class T> static Guid& Value(T& object) { return object.id; }
    template <class T> static const Guid& Value(const T& object) { return object.id; }
};

struct PositionPropDescriptor
{
    operator const char*() { return "position"; }
    operator const wchar_t*() { return L"position"; }
    template <class T> static VectorT& Value(T& object) { return object.position; }
    template <class T> static const VectorT& Value(const T& object) { return object.position; }
};

struct SpeedPropDescriptor
{
    operator const char*() { return "speed"; }
    operator const wchar_t*() { return L"speed"; }
    template <class T> static VectorT& Value(T& object) { return object.speed; }
    template <class T> static const VectorT& Value(const T& object) { return object.speed; }
};

struct MassPropDescriptor
{
    operator const char*() { return "mass"; }
    operator const wchar_t*() { return L"mass"; }
    template <class T> static int& Value(T& object) { return object.mass; }
    template <class T> static const int& Value(const T& object) { return object.mass; }
};

struct RadiusPropDescriptor
{
    operator const char*() { return "radius"; }
    operator const wchar_t*() { return L"radius"; }
    template <class T> static int& Value(T& object) { return object.radius; }
    template <class T> static const int& Value(const T& object) { return object.radius; }
};

//----------------------------------------------------------------------------
class SpaceGameObject : public Serializable
{
protected:
    typedef TypesList<IdPropDescriptor> PropDescriptorsTypesList;
public:
    Guid id;
    void Serialize(std::ostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
    void Serialize(std::wostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
};



class PhysicalObject : public SpaceGameObject
{
protected:
    typedef typename PropDescriptorsTypesList::Concat<PositionPropDescriptor, SpeedPropDescriptor, MassPropDescriptor, RadiusPropDescriptor>::Result PropDescriptorsTypesList;
public:
    VectorT position;
    VectorT speed;
    int     mass;
    int     radius;
    void Serialize(std::ostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
    void Serialize(std::wostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
};