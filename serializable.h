#pragma once
#include <ostream>
#include "property.h"

template <class ... Ids>
class Serializable
{
public:
    template <class ... Args>
    Serializable(Args&& ...) {}

    void Put(std::ostream&, int) const {}
    void Put(std::wostream&, int) const {}
};

template <class Id, class ... NextIds>
class Serializable<Id, NextIds ...> : public Serializable<NextIds ...>, protected Property<Id>
{
public:
    template<class ... Args>
    Serializable(Args&& ... args) :
    Property<Id>(Select<Id>(args ..., Id::DefaultValue())),
    Serializable<NextIds ...>(args ...)
    {}

    template <class PropId>
    Property<PropId>& GetProp()
    { return *this; }

    template <class PropId>
    const Property<PropId>& GetProp() const
    { return *this; }

    void Put(std::ostream& stream, int propIndex = 0) const
    {
        if(propIndex != 0)
            stream << ", ";
        stream << GetProp<Id>();
        Serializable<NextIds ...>::Put(stream, propIndex + 1);
    }

    void Put(std::wostream& stream, int propIndex = 0) const
    {
        if(propIndex != 0)
            stream << L", ";
        stream << GetProp<Id>();
        Serializable<NextIds ...>::Put(stream, propIndex + 1);
    }

    template <class ... Ids>
    friend std::ostream& operator <<(std::ostream&, const Serializable<Ids ...>&);

    template <class ... Ids>
    friend std::wostream& operator <<(std::wostream&, const Serializable<Ids ...>&);
};

template <class Id, class ... NextIds>
std::ostream& operator <<(std::ostream& stream, const Serializable<Id, NextIds ...>& serializable)
{
    stream << "{ ";
    serializable.Put(stream);
    stream << " }";
    return stream;
}

template <class Id, class ... NextIds>
std::wostream& operator <<(std::wostream& stream, const Serializable<Id, NextIds ...>& serializable)
{
    stream << L"{ ";
    serializable.Put(stream);
    stream << L" }";
    return stream;
}