#pragma once
#include <iostream>

template <class Needed, class ... Types>
struct SelectFrom
{ };

template <class Needed, class First, class ... Nexts>
struct SelectFrom<Needed, First, Nexts ...>
{
    static const Needed& Get(const First&, const Nexts& ... nexts)
    { return SelectFrom<Needed, Nexts ...>::Get(nexts ...); }
};

template <class Needed, class ... Nexts>
struct SelectFrom<Needed, Needed, Nexts ...>
{
    static const Needed& Get(const Needed& needed, const Nexts& ... nexts)
    { return needed; }
};

template <class Id>
class Property
{
public:
    typedef typename Id::ValueType ValueType;
    typedef ValueType& ValueRefType;
protected:
    ValueType value;
public:
    Property(const Property<Id>& other) :
    value(other.value)
    { }

    Property(ValueType& _value) : value(_value) {}

    Property(const ValueType& _value) : value(_value) {}

    operator ValueType&() { return value; }

    operator const ValueType&() const { return value; }

    static ValueType& RefFrom(Property<Id>& prop)
    { return prop.value; }

    static const ValueType& RefFrom(const Property<Id>& prop)
    { return prop.value; }

    template <class ... Args>
    const Property<Id>& Select(const Args& ... args)
    { return SelectFrom<Property<Id>, Args ...>::Get(args ...); }
};