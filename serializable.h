#pragma once
#include <ostream>
#include "property.h"

template <class ... Types>
struct TypesList
{
    template <class ... AdditionalTypes>
    struct Concat
    {
        typedef TypesList<Types ..., AdditionalTypes ...> Result;
    };

    template <template <class ...> class T>
    struct Template
    {
        typedef T<Types ...> Result;
    };
};

//-----------------------------------------------------------------------
template <class ... NoMatter>
struct Inherits
{ };

template <class T, class ... Nexts>
struct Inherits<T, Nexts ...> : T, Inherits<Nexts ...>
{ };

//-----------------------------------------------------------------------
template <class ChatR>
struct JsonStrings {};

template <>
struct JsonStrings<char>
{
    static const char* OpenBrace()  { return "{ "; }
    static const char* Separator()  { return ", "; }
    static const char* CloseBrace() { return " }"; }
    static const char* Colon()      { return " : "; }
    static const char* Quot()       { return "\""; }
};

template <>
struct JsonStrings<wchar_t>
{
    static const wchar_t* OpenBrace()  { return L"{ "; }
    static const wchar_t* Separator()  { return L", "; }
    static const wchar_t* CloseBrace() { return L" }"; }
    static const wchar_t* Colon()      { return L" : "; }
    static const wchar_t* Quot()       { return L"\""; }
};

//-----------------------------------------------------------------------
class Serializable
{
protected:
    template <class NoMatter>
    struct Serializer { };

    template <class ... Empty>
    struct Serializer<TypesList<Empty ...>>
    {
        template <class CharT, class U>
        static void Do(std::basic_ostream<CharT>& stream, const U& object, int)
        {
            stream << JsonStrings<CharT>::CloseBrace();
        }
    };

    template <class PropT, class ... Nexts>
    struct Serializer<TypesList<PropT, Nexts ...>>
    {
        template <class CharT, class U>
        static void Do(std::basic_ostream<CharT>& stream, const U& object, int num = 0)
        {
            stream << (num != 0 ? JsonStrings<CharT>::Separator() : JsonStrings<CharT>::OpenBrace());
            stream << JsonStrings<CharT>::Quot() << ((const PropT&)object).Name() << JsonStrings<CharT>::Quot();
            stream << JsonStrings<CharT>::Colon() << (typename PropT::Type&)(PropT&)object;
            Serializer<TypesList<Nexts ...>>::Do(stream, object, num + 1);
        }
    };
public:
    virtual void Serialize(std::ostream&) const = 0;
    virtual void Serialize(std::wostream&) const = 0;
};

//-----------------------------------------------------------------------
std::ostream& operator <<(std::ostream& stream, const Serializable& serializable)
{
    serializable.Serialize(stream);
    return stream;
}

std::wostream& operator <<(std::wostream& stream, const Serializable& serializable)
{
    serializable.Serialize(stream);
    return stream;
}