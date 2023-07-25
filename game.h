#pragma once
#include "guid.h"
#include "vector.h"
#include "property.h"
#include "serializable.h"

typedef double NumT;
typedef SpaceGameMath::Vector<NumT, NumT> VectorT;

template <>
struct JsonFormatter<Guid>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const Guid& value)
    { return stream << JsonSymbol<CharT>::Quot << std::basic_string<CharT>(value) << JsonSymbol<CharT>::Quot; }
};

template <class ClassId>
struct JsonFormatter<Property<ClassId>>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const Property<ClassId>& property)
    {
        JsonFormatter<std::basic_string<CharT>>::PutToStream(stream, StringsConverter<char, CharT>::Convert(ClassId::Name));
        stream << JsonSymbol<CharT>::Colon;
        JsonFormatter<typename Property<ClassId>::ValueType>::PutToStream(stream, (const typename Property<ClassId>::ValueType &)property);
        return stream;
    }
};

template <>
struct JsonFormatter<VectorT>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const VectorT& vector)
    {
        stream << StringsConverter<char, CharT>::Convert("{x:");
        stream << vector.at<0>();
        stream << StringsConverter<char, CharT>::Convert(",y:");
        stream << vector.at<1>();
        stream << StringsConverter<char, CharT>::Convert("}");
        return stream;
    }
};

template <class T>
struct ClassId
{ typedef T ValueType; };

struct IdId : ClassId<Guid>
{ static constexpr const char* Name = "id"; };

struct MassId : ClassId<NumT>
{ static constexpr const char* Name = "mass"; };

struct RadiusId : ClassId<NumT>
{ static constexpr const char* Name = "radius"; };

struct PositionId : ClassId<VectorT>
{ static constexpr const char* Name = "position"; };

struct SpeedId : ClassId<VectorT>
{ static constexpr const char* Name = "speed"; };

struct AccelerationId : ClassId<VectorT>
{ static constexpr const char* Name = "acceleration"; };

struct RotationId : ClassId<NumT>
{ static constexpr const char* Name = "rotation"; };



typedef Property<IdId>           Id;
typedef Property<MassId>         Mass;
typedef Property<RadiusId>       Radius;
typedef Property<PositionId>     Position;
typedef Property<SpeedId>        Speed;
typedef Property<AccelerationId> Acceleration;
typedef Property<RotationId>     Rotation;

//----------------------------------------------------------------------------
class Object :
public Id,
public Serializable
{
public:
    typename Id::ValueRefType id = Id::RefFrom(*this);
    Object() :
    Id(Guid::New())
    { }

    template <class ... Args>
    Object(Args ... args) :
    Id(Id::Select(args ..., Id(Guid::New())))
    { }

    void PutToStream(std::ostream& stream) const override
    { PutPropsToStream<Id>(stream, *this); }

    void PutToStream(std::wostream& stream) const override
    { PutPropsToStream<Id>(stream, *this); }

    virtual ~Object() {}
};

class MatherialObject :
public Mass,
public Radius,
public Object
{
public:
    typename Mass  ::ValueRefType mass   = Mass  ::RefFrom(*this);
    typename Radius::ValueRefType radius = Radius::RefFrom(*this);

    template <class ... Args>
    MatherialObject(Args ... args) :
    Object(args ...),
    Mass  (Mass  ::Select(args ..., Mass  (1))),
    Radius(Radius::Select(args ..., Radius(1)))
    { }

    void PutToStream(std::ostream& stream) const override
    { PutPropsToStream<Id, Mass, Radius>(stream, *this); }

    void PutToStream(std::wostream& stream) const override
    { PutPropsToStream<Id, Mass, Radius>(stream, *this); }
};

class PhysicalObject :
public Position,
public Speed,
public Acceleration,
public Rotation,
public MatherialObject
{
public:
    typename Position    ::ValueRefType position     = Position    ::RefFrom(*this);
    typename Speed       ::ValueRefType speed        = Speed       ::RefFrom(*this);
    typename Acceleration::ValueRefType acceleration = Acceleration::RefFrom(*this);
    typename Rotation    ::ValueRefType rotation     = Rotation    ::RefFrom(*this);

    template <class ... Args>
    PhysicalObject(Args ... args) :
    MatherialObject(args ...),
    Position    (Position    ::Select(args ..., Position    (VectorT(0, 0)))),
    Speed       (Speed       ::Select(args ..., Speed       (VectorT(0, 0)))),
    Acceleration(Acceleration::Select(args ..., Acceleration(VectorT(0, 0)))),
    Rotation    (Rotation    ::Select(args ..., Rotation    (0)))
    { }


    void PutToStream(std::ostream& stream) const override
    { PutPropsToStream<Id, Mass, Radius, Position, Speed, Acceleration, Rotation>(stream, *this); }

    void PutToStream(std::wostream& stream) const override
    { PutPropsToStream<Id, Mass, Radius, Position, Speed, Acceleration, Rotation>(stream, *this); }
};