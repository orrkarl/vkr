#pragma once

#include "../general/predefs.h"
#include "Buffer.h"


#define CTOR(DataType)                                          \
    Uniform(DataType* data, const NRuint n, const NRuint m)     \
        : Buffer(data, n * m), n(n)                             \
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

    std::pair<NRuint, NRuint> getDimensions() 
    {
        return { n, getSize() / n };
    }

private:
    const NRuint n;
};

}


#undef CTOR

