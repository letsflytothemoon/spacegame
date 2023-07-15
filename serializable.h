#pragma once
#include <ostream>
#include "property.h"

template <class T>
struct Format
{
    static void format(std::ostream& stream, const T& value)
    { stream << value; }

    static void format(std::wostream& wstream, const T& value)
    { wstream << value; }
};

template <class T>
void format(std::ostream& stream, const T& value)
{
    Format<T>::format(stream, value);    
}

template <class T>
void format(std::wostream& wstream, const T& value)
{
    Format<T>::format(wstream, value);    
}

template <class ... Ids>
class Serializable
{ };

template <class Id, class ... NextIds>
class Serializable<Id, NextIds ...> : public Serializable<NextIds ...>, protected Property<Id>
{
    template<class ... Props>
    struct JoinProps
    {
        template <class T>
        static void get(const T&, const std::stringstream&)
        { }
    };

    template <class T, class ... Nexts>
    struct JoinProps<T, Nexts ...>
    {
        static void get(const Serializable<T, Nexts ...>& object, std::stringstream& stream)
        {
            JoinProps<T>::get(object, stream);
            stream << ", ";
            JoinProps<Nexts ...>::get(object, stream);
        }
    };

    template <class T>
    struct JoinProps<T>
    {
        template <class ... NoMatter>
        static void get(const Serializable<T, NoMatter ...>& object, std::stringstream& stream)
        {
            stream << "\"" << T::name() << "\" : ";
            format(stream, (typename T::Type&)(Property<T>&)object);
        }
    };
public:
    std::string Serialize()
    {
        std::stringstream resultStream;
        resultStream << "{ ";
        JoinProps<Id, NextIds ...>::get(*this, resultStream);
        resultStream << " }";
        return resultStream.str();
    }
};

template<>
struct Format<std::string>
{
    static void format(std::ostream& stream, const std::string& string)
    { stream << "\"" << string << "\""; } //need to add escaping some characters
};

template <>
struct Format<std::wstring>
{
    static void format(std::wostream& wstream, const std::wstring& wstring)
    { wstream << L"\"" << wstring << L"\""; }//same
};