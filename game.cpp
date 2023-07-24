#include "game.h"

std::ostream& operator <<(std::ostream& stream, const VectorT& vector)
{
    stream << "{ \"x\" : " << vector.at<0>() << ", \"y\" : " << vector.at<1>() << " }";
    return stream;
}

std::wostream& operator <<(std::wostream& stream, const VectorT& vector)
{
    stream << L"{ \"x\" : " << vector.at<0>() << L", \"y\" : " << vector.at<1>() << L" }";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const Guid& guid)
{
    stream << guid.operator std::string();
    return stream;
}

std::wostream& operator <<(std::wostream& stream, const Guid& guid)
{
    stream << guid.operator std::wstring();
    return stream;
}