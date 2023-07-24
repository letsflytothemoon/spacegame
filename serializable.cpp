#include <vector>
#include "serializable.h"

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