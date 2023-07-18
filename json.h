#pragma once
#include <map>
#include <string>

class Json
{
public:
    virtual std::string Serialize() = 0;
    static Json Deserialize(const std::string&)
    {
        
    }
};

class JsonValue
{
    std::string value;
public:
};