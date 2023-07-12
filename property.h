#pragma once

template <class Id>
class Prop
{
    typedef typename Id::Type Type;
    Type _value;
public:
    Prop(){}
    Prop(const Type value) : _value(value) {}
    operator Type&() { return _value; }
    operator const Type&() const { return _value; }
    virtual Prop<Id> operator=(const Type& value)
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
struct Selector<Id, Prop<Id>, Args ...>
{
    static typename Id::Type Select(const Prop<Id>& prop, const Args& ... args)
    { return prop; }
};

template <class Id>
struct Selector<Id, typename Id::Type>
{
    static typename Id::Type Select(const typename Id::Type& defaultValue)
    { return defaultValue; }
};

template <class Id, class ... Args>
typename Id::Type Select(const Args& ... args)
{
    return Selector<Id, Args ...>::Select(args ...);
}