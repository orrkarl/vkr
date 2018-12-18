#pragma once

#include "../general/predefs.h"
#include "Buffer.h"


#define CTOR(DataType)                                          \
    Uniform(                                                    \
        DataType* data,                                         \
        const NRuint n,                                         \
        const NRuint m,                                         \
        const NRuint count = 1)                                 \
        : Buffer(data, n * m * count), n(n), count(count)       \
    {                                                           \
    }


namespace nr
{

class Uniform : public Buffer
{
public:
    
    CTOR(NRfloat);

    CTOR(NRint);

    CTOR(NRuint);

    std::pair<NRuint, NRuint> getDimensions() const
    {
        return { n, getSize() / (n * count) };
    }

    NRuint getCount() const { return count; }
    
private:
    const NRuint n;
    const NRuint count;
};

}


#undef CTOR

