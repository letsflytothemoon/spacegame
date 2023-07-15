#pragma once

namespace vector
{
    template <class ... Types>
    class Vector
    {
    public:
        Vector<> operator +(const Vector<>&) const { return Vector<>(); }
        Vector<> operator -(const Vector<>&) const { return Vector<>(); }
        Vector<> operator *(const Vector<>&) const { return Vector<>(); }
        Vector<> operator =(const Vector<>&) { return *this; }
        Vector<> operator +=(const Vector<>&) { return *this; }
        Vector<> operator -=(const Vector<>&) { return *this; }
        Vector<> operator *=(const Vector<>&) { return *this; }
        
        template <class T> Vector<> operator *=(const T&) { return *this; }
        template <class T> Vector<> operator /=(const T&) { return *this; }
    };

    template <int I, class T, class ... Nexts>
    struct TypeGetter
    {
        typedef typename TypeGetter<I - 1, Nexts ...>::ResultT ResultT;
    };

    template <class T, class ... Nexts>
    struct TypeGetter<0, T, Nexts ...>
    {
        typedef T ResultT;
    };

    template <int i>
    struct ValueRefGetter
    {
        template <class T, class ... Nexts>
        static T& get(Vector<T, Nexts ...>& vector)
        { return ValueRefGetter<i - 1>::get(vector._nexts); }

        template <class T, class ... Nexts>
        static const T& get(const Vector<T, Nexts ...>& vector)
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
        
        template <int I>
        typename TypeGetter<I, T, Nexts ...>::ResultT& at()
        { return ValueRefGetter<I>::get(*this); }
        
        template <int I>
        const typename TypeGetter<I, T, Nexts ...>::ResultT& at() const
        { return ValueRefGetter<I>::get(*this); }

        template <class U, class ... Types>
        Vector<T, Nexts ...> operator +(const Vector<U, Types ...>& other) const
        { return Vector<T, Nexts ...>(_value + other._value, _nexts + other._nexts); }

        template <class U, class ... Types>
        Vector<T, Nexts ...> operator -(const Vector<U, Types ...>& other) const
        { return Vector<T, Nexts ...>(_value - other._value, _nexts - other._nexts); }

        template <class U, class ... Types>
        Vector<T, Nexts ...> operator *(const Vector<U, Types ...>& other) const
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

        template <class U>
        Vector<T, Nexts ...> operator *(const U& multiplier) const
        { return Vector<T, Nexts ...>(_value * multiplier, _nexts * multiplier); };

        template <class U>
        Vector<T, Nexts ...> operator /(const U& divider) const
        { return Vector<T, Nexts ...>(_value / divider, _nexts / divider); };

        template <class U>
        Vector<T, Nexts ...>& operator *=(const U& multiplier)
        {
            _value *= multiplier;
            _nexts *= multiplier;
            return *this;
        }

        template <class U>
        Vector<T, Nexts ...>& operator /=(const U& divider)
        {
            _value /= divider;
            _nexts /= divider;
            return *this;
        }
    };

    template <class L, class R>
    Vector<L, R> operator ,(const L& l, const R& r)
    { return Vector<L, R>(l, r); }

    template <class ... Args>
    Vector<Args ...> MakeVector(const Args& ... args)
    { return Vector<Args ...>(args ...); }
}