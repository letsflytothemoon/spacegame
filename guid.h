#pragma once
#include <cstdint>
#include <string>
#include <cassert>
#include <regex>
#include <random>

class Guid
{
    template <class T>
    bool inRange(T value, T min, T max)
    { return !(value < min || max < value); }

    uint32_t part[4];
public:
    Guid() { }

    Guid(uint32_t part0, uint32_t part1, uint32_t part2, uint32_t part3)
    {
        part[0] = part0;
        part[1] = part1;
        part[2] = part2;
        part[3] = part3;
    }

    Guid(const std::string& string)
    {
        assert(regex_match(string, std::regex("[0-9a-fA-F]{8}-([0-9a-fA-F]{4}-){3}[0-9a-fA-F]{12}")));
        int i = 0;
        while(i < 4) part[i++] = 0;
        i = 0;
        for(int symbolIndex = 0; symbolIndex < string.size(); symbolIndex ++)
        {
            char symbol = string[symbolIndex];
            if(symbol == '-') continue;

            char bits = ((inRange(symbol, '0', '9') ? symbol - '0' :  
                inRange(symbol, 'a', 'f') ? symbol - 'a' + 10 :
                symbol - 'A' + 10));

            part[3 - (i >> 3)] |= bits << ((7 - (i % 8)) * 4);
            ++i;
        }
    }

    static Guid New()
    {
        static std::mt19937 gen((std::random_device())());
        return Guid(gen(), gen(), gen(), gen());
    }

    bool operator ==(const Guid& other)
    {
        return part[0] == other.part[0] && part[1] == other.part[1] &&
                part[2] == other.part[2] && part[3] == other.part[3];
    }

    bool operator !=(const Guid& other)
    {
        return part[0] != other.part[0] || part[1] != other.part[1] ||
                part[2] != other.part[2] || part[3] != other.part[3];
    }

    operator std::string() const
    {
        std::stringstream resultStream;
        for(int i = 0; i < 32; i++)
        {
            if(i == 8 || i == 12 || i == 16 || i == 20) resultStream << '-';
            resultStream << "0123456789abcdef"[(part[3 - (i >> 3)] >> ((7 - (i % 8)) * 4)) & 0xf];
        }
        return resultStream.str();
    }
};