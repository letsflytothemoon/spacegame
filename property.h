#pragma once
#include <ostream>
#include <codecvt>
#include "guid.h"

template <class T>
struct JsonFormat
{
    static void PutFormatted(std::ostream& stream, const T& value)
    { stream << value; }

    static void PutFormatted(std::wostream& stream, const T& value)
    { stream << value; }
};

template <>
struct JsonFormat<Guid>
{
    static void PutFormatted(std::ostream& stream, const Guid& value)
    { stream << "\"" << value << "\""; }

    static void PutFormatted(std::wostream& stream, const Guid& value)
    { stream << L"\"" << value << L"\""; }
};

template <>
struct JsonFormat<std::string>
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>& Converter()
    {
        static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter;
    }

    static void PutFormatted(std::ostream& stream, const std::string& value)
    { stream << "\"" << value << "\""; }

    static void PutFormatted(std::wostream& stream, const std::string& value)
    { stream << L"\"" << Converter().from_bytes(value) << L"\""; }
};

template <>
struct JsonFormat<std::wstring>
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>& Converter()
    {
        static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter;
    }

    static void PutFormatted(std::ostream& stream, const std::wstring& value)
    { stream << "\"" << Converter().to_bytes(value) << "\""; }

    static void PutFormatted(std::wostream& stream, const std::wstring& value)
    { stream << L"\"" << value << L"\""; }
};

template <class Id>
class Property
{
    typedef typename Id::Type Type;
    Type _value;
public:
    Property(){}
    Property(const Type& value) : _value(value) {}
    operator Type&() { return _value; }
    operator const Type&() const { return _value; }
    virtual Property<Id> operator=(const Type& value)
    {
        _value = value;
        return *this;
    }
    Type& Value()
    { return _value; }

    const Type& Value() const
    { return _value; }

    template <class>
    friend std::ostream& operator << (std::ostream&, const Property<Id>&);

    template <class>
    friend std::wostream& operator << (std::wostream&, const Property<Id>&);
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
    stream << "\"" << Id::Name() << "\" : ";
    JsonFormat<typename Id::Type>::PutFormatted(stream, prop.Value());
    return stream;
}

template <class Id>
std::wostream& operator <<(std::wostream& stream, const Property<Id>& prop)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    stream << L"\"" << converter.from_bytes(Id::WName()) << L"\" : ";
    JsonFormat<typename Id::Type>::PutFormatted(stream, prop.Value());
    return stream;
}