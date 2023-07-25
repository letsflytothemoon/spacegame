#pragma once
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

template <class CharT>
struct JsonSymbol
{};

template <>
struct JsonSymbol<char>
{
    static constexpr const char* OpenBrace  = "{";
    static constexpr const char* Comma      = ",";
    static constexpr const char* CloseBrace = "}";
    static constexpr const char* Colon      = ":";
    static constexpr const char* Quot       = "\"";
};

template <>
struct JsonSymbol<wchar_t>
{
    static constexpr const wchar_t* OpenBrace  = L"{";
    static constexpr const wchar_t* Comma      = L",";
    static constexpr const wchar_t* CloseBrace = L"}";
    static constexpr const wchar_t* Colon      = L":";
    static constexpr const wchar_t* Quot       = L"\"";
};

template <class SourceCharT, class TargetCharT>
struct StringsConverter
{ };

template <class Same>
struct StringsConverter<Same, Same>
{
    static const std::basic_string<Same>& Convert(const std::basic_string<Same>& srcString)
    { return srcString; }
};

template <>
struct StringsConverter<char, wchar_t>
{
    static std::wstring Convert(const std::string& srcString)
    { return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(srcString); }
};

template <>
struct StringsConverter<wchar_t, char>
{
    static std::string Convert(const std::wstring& srcString)
    { return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(srcString); }
};

template <class T>
struct JsonFormatter
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const T& value)
    { return stream << value; }
};

template <>
struct JsonFormatter<char*>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const char* value)
    {
        //todo: escaping quotes and backslashes
        return stream << JsonSymbol<CharT>::Quot << StringsConverter<char, CharT>::Convert(value) << JsonSymbol<CharT>::Quot;
    };
};

template <>
struct JsonFormatter<wchar_t*>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const wchar_t* value)
    {
        //todo: escaping quotes and backslashes
        return stream << JsonSymbol<CharT>::Quot << StringsConverter<wchar_t, CharT>::Convert(value) << JsonSymbol<CharT>::Quot;
    };
};

template <class StringCharT>
struct JsonFormatter<std::basic_string<StringCharT>>
{
    template <class CharT>
    static std::basic_ostream<CharT>& PutToStream(std::basic_ostream<CharT>& stream, const std::basic_string<StringCharT>& string)
    {
        //todo: escaping quotes and backslashes and convertig to char*
        return stream << JsonSymbol<CharT>::Quot << StringsConverter<StringCharT, CharT>::Convert(string) << JsonSymbol<CharT>::Quot;
    };

    template <class CharT>
    static std::basic_ostream<CharT>* PutToStream(std::basic_ostream<CharT>& stream, const StringCharT* string)
    {
        return stream << JsonSymbol<CharT>::Quot << StringsConverter<StringCharT, CharT>::Convert(string) << JsonSymbol<CharT>::Quot;
    };
};

template <class ... Types>
struct SerializablePutter
{
    template <class ... Args>
    static void PutToStream(const Args& ... args) { }
};

template <class T, class ... Types>
struct SerializablePutter<T, Types ...>
{
    template <class U, class CharT>
    static void PutToStream(std::basic_ostream<CharT>& stream, const U& object, int num = 0)
    {
        if(num > 0) stream << JsonSymbol<CharT>::Comma;
        JsonFormatter<T>::PutToStream(stream, (const T&)object);
        SerializablePutter<Types ...>::PutToStream(stream, object, num + 1);
    };
};

class Serializable
{
public:
    virtual void PutToStream(std:: ostream& stream) const { }
    virtual void PutToStream(std::wostream& stream) const { }

    template <class ... Types, class CharT, class T>
    static void PutPropsToStream(std::basic_ostream<CharT>& stream, const T& object)
    {
        SerializablePutter<Types ...>::PutToStream(stream, object);
    }
};

template <class CharT>
std::basic_ostream<CharT>& operator <<(std::basic_ostream<CharT>& stream, const Serializable& serializable)
{
    serializable.PutToStream(stream);
    return stream;
}





