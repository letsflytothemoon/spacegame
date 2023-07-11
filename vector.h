#pragma once
template <class ... Types>
class Vector
{
public:
    Vector<> operator +(const Vector<>&) { return Vector<>(); }
    Vector<> operator -(const Vector<>&) { return Vector<>(); }
    Vector<> operator *(const Vector<>&) { return Vector<>(); }
    Vector<> operator =(const Vector<>&) { return *this; }
    Vector<> operator +=(const Vector<>&) { return *this; }
    Vector<> operator -=(const Vector<>&) { return *this; }
    Vector<> operator *=(const Vector<>&) { return *this; }
};

template <int i>
struct ValueRefGetter
{
    template <class T, class ... Nexts>
    static auto get(Vector<T, Nexts ...>& vector)
    { return ValueRefGetter<i - 1>::get(vector._nexts); }
};

template <>
struct ValueRefGetter<0>
{
    template <class T, class ... Nexts>
    static T& get(Vector<T, Nexts ...>& vector)
    { return vector._value; }

    template <class T, class ... Nexts>
    static const T& get(const Vector<T, Nexts ...>& vector)
    { return vector._value; }
};



template <class T, class ... Nexts>
class Vector<T, Nexts ...>
{
    template <int> friend class ValueRefGetter;
    template <class ...> friend class Vector;
protected:
    T _value;
    Vector<Nexts ...> _nexts;
public:
    Vector() {}

    Vector(const Vector<T, Nexts ...>& other) : _value(other._value), _nexts(other._nexts) {}

    template <class U, class ... Types>
    Vector(U value, Types ... nexts) : _value(value), _nexts(nexts ...) {}
    
    template <int i>
    auto at()
    { return ValueRefGetter<i>::get(*this); }
    
    template <int i>
    auto at() const
    { return ValueRefGetter<i>::get(*this); }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator +(const Vector<U, Types ...>& other)
    { return Vector<T, Nexts ...>(_value + other._value, _nexts + other._nexts); }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator -(const Vector<U, Types ...>& other)
    { return Vector<T, Nexts ...>(_value - other._value, _nexts - other._nexts); }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator *(const Vector<U, Types ...>& other)
    { return Vector<T, Nexts ...>(_value * other._value, _nexts * other._nexts); }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator =(const Vector<U, Types ...>& other)
    {
        _value = other._value;
        _nexts = other._nexts;
        return *this;
    }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator +=(const Vector<U, Types ...>& other)
    {
        _value += other._value;
        _nexts += other._nexts;
        return *this;
    }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator -=(const Vector<U, Types ...>& other)
    {
        _value -= other._value;
        _nexts -= other._nexts;
        return *this;
    }

    template <class U, class ... Types>
    Vector<T, Nexts ...> operator *=(const Vector<U, Types ...>& other)
    {
        _value *= other._value;
        _nexts *= other._nexts;
        return *this;
    }
};