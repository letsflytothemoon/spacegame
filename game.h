#pragma once
#include "guid.h"
#include "vector.h"
#include "serializable.h"

typedef SpaceGameMath::Vector<int, int> VectorT;

std::ostream&  operator <<(std::ostream&  stream, const VectorT& vector);
std::wostream& operator <<(std::wostream& stream, const VectorT& vector);
std::ostream&  operator <<(std::ostream&  stream, const Guid& guid);
std::wostream& operator <<(std::wostream& stream, const Guid& guid);

struct IdPropDescriptor
{
    operator const char*   () const { return  "id"; }
    operator const wchar_t*() const { return L"id"; }
    template <class T> static       Guid& Value(      T& object) { return object.id; }
    template <class T> static const Guid& Value(const T& object) { return object.id; }
};

struct MassPropDescriptor
{
    operator const char*   () const { return  "mass"; }
    operator const wchar_t*() const { return L"mass"; }
    template <class T> static       int& Value(      T& object) { return object.mass; }
    template <class T> static const int& Value(const T& object) { return object.mass; }
};

struct RadiusPropDescriptor
{
    operator const char*   () const { return  "radius"; }
    operator const wchar_t*() const { return L"radius"; }
    template <class T> static       int& Value(      T& object) { return object.radius; }
    template <class T> static const int& Value(const T& object) { return object.radius; }
};

struct PositionPropDescriptor
{
    operator const char*   () const { return  "position"; }
    operator const wchar_t*() const { return L"position"; }
    template <class T> static       VectorT& Value(      T& object) { return object.position; }
    template <class T> static const VectorT& Value(const T& object) { return object.position; }
};

struct SpeedPropDescriptor
{
    operator const char*   () const { return  "speed"; }
    operator const wchar_t*() const { return L"speed"; }
    template <class T> static       VectorT& Value(      T& object) { return object.speed; }
    template <class T> static const VectorT& Value(const T& object) { return object.speed; }
};

struct AccelerationDescriptor
{
    operator const char*   () const { return  "acceleration"; }
    operator const wchar_t*() const { return L"acceleration"; }
    template <class T> static       VectorT& Value(      T& object) { return object.acceleration; }
    template <class T> static const VectorT& Value(const T& object) { return object.acceleration; }
};

struct RotationDescriptor
{
    operator const char*   () const { return  "rotation"; }
    operator const wchar_t*() const { return L"rotation"; }
    template <class T> static       int& Value(      T& object) { return object.rotation; }
    template <class T> static const int& Value(const T& object) { return object.rotation; }
};
//----------------------------------------------------------------------------
class Object : public Serializable
{
protected:
    typedef TypesList<IdPropDescriptor> PropDescriptorsTypesList;
public:
    Object() : id(Guid::New()) {}
    virtual ~Object() {}
    Guid id;
    void Serialize(std::ostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
    void Serialize(std::wostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
};

class MatherialObject : public Object
{
protected:
    typedef typename PropDescriptorsTypesList::Concat<
        MassPropDescriptor,
        RadiusPropDescriptor
        >::Result PropDescriptorsTypesList;
public:
    int mass;
    int radius;

    MatherialObject(int Mass, int Radius) :
    mass(Mass),
    radius(Radius)
    { }
};

class PhysicalObject : public MatherialObject
{
protected:
    typedef typename PropDescriptorsTypesList::Concat<
        PositionPropDescriptor,
        SpeedPropDescriptor,
        AccelerationDescriptor,
        RotationDescriptor
        >::Result PropDescriptorsTypesList;
public:
    VectorT position;
    VectorT speed;
    VectorT acceleration;
    int     rotation;
    
    PhysicalObject(VectorT Position, VectorT Speed, int Mass, int Radius) :
    MatherialObject(Mass, Radius),
    position(Position),
    speed(Speed)
    { }

    void Serialize(std::ostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
    void Serialize(std::wostream& stream) const override { Serializer<PropDescriptorsTypesList>::Do(stream, *this); }
};