#pragma once
#include <ostream>

template <class Id>
class Property
{
    typedef typename Id::Type Type;
    Type _value;
public:
    Property(){}
    Property(const Type value) : _value(value) {}
    operator Type&() { return _value; }
    operator const Type&() const { return _value; }
    virtual Property<Id> operator=(const Type& value)
    {
        _value = value;
        return *this;
    }
};

template <class Id, class T, class ... Args>
struct Selector
{
    static typename Id::Type Select(const T&, const Args& ... args)
    { return Selector<Id, Args ...>::Select(args ...); }
};

template <class Id, class ... Args>
struct Selector<Id, Property<Id>, Args ...>
{
    static typename Id::Type Select(const Property<Id>& prop, const Args& ... args)
    { return prop; }
};

template <class Id>
struct Selector<Id, typename Id::Type>
{
    static typename Id::Type Select(const typename Id::Type& defaultValue)
    { return defaultValue; }
};

template <class Id, class Some>
struct Selector<Id, Some>
{
    static typename Id::Type Select(const Some& some)
    { return Id::Type(some); }
};

template <class Id, class ... Args>
typename Id::Type Select(const Args& ... args)
{
    return Selector<Id, Args ...>::Select(args ...);
}

template <class Id>
std::ostream& operator <<(std::ostream& stream, const Property<Id>& prop)
{
    stream << "\"" << Id::name() << "\" : " << prop.operator typename Id::Type &();
    return stream;
}