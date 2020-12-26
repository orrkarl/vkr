#ifndef _COMPATIBILITY_H_
#define _COMPATIBILITY_H_

#ifdef VKR_DEVICE_SIDE
#include "device_compatibility.h"
#else
#include "host_compatibility.h"
#endif // #ifdef VKR_DEVICE_SIDE

struct RasterConfig {
    u32 packedViewport;
    u32 multiSampleBitCount;
};

#endif // #ifndef _COMPATIBILITY_H_
