#pragma once
#include <ostream>

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

template <class T>
struct JsonFormatter
{
    const T& _value;
    JsonFormatter(const T& value) : _value(value) { }
    template <class CharT>
    void Serialize(std::basic_ostream<CharT>& stream) const
    { stream << _value; }
};

template <class T, class CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const JsonFormatter<T>& formatter)
{ 
    formatter.Serialize(stream);
    return stream;
}

template <>
struct JsonFormatter<Guid>
{
    const Guid& _value;
    JsonFormatter(const Guid& value) : _value(value) { }
    template <class CharT>
    void Serialize(std::basic_ostream<CharT>& stream) const
    { stream << JsonStrings<CharT>::Quot() << _value << JsonStrings<CharT>::Quot(); }
};

template <class CharT>
struct JsonFormatter<std::basic_string<CharT>>
{
    const std::basic_string<CharT>& _value;
    JsonFormatter(const std::basic_string<CharT>& value) : _value(value) { }
    
    //todo: need to add escaping
    void Serialize(std::basic_ostream<CharT>& stream) const
    { stream << JsonStrings<CharT>::Quot() << _value << JsonStrings<CharT>::Quot(); }
};

template <class T>
JsonFormatter<T> JsonFormat(const T& object)
{
    return JsonFormatter<T>(object);
}


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

    template <class PropDescriptorT, class ... Nexts>
    struct Serializer<TypesList<PropDescriptorT, Nexts ...>>
    {
        template <class CharT, class U>
        static void Do(std::basic_ostream<CharT>& stream, const U& object, int num = 0)
        {
            stream << (num != 0 ? JsonStrings<CharT>::Separator() : JsonStrings<CharT>::OpenBrace());
            stream << JsonStrings<CharT>::Quot() << (const CharT*)PropDescriptorT() << JsonStrings<CharT>::Quot();
            stream << JsonStrings<CharT>::Colon() << JsonFormat(PropDescriptorT::Value(object));
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