#include <limits>
#include <memory>
#include <vector>

#include "includes.h"

#include <buffers/Buffer.h>
#include <utils/memory.h>

#pragma once

TEST(UnifiyBuffersTest, SanityTest)
{
    std::vector<NRfloat> originalData = { 0.1f, 0.2f, 0.3f };
    void* data = originalData.data();

    auto buf = nr::Buffer(originalData);
    auto bufptr = &buf;

    auto res = nr::utils::unifyBuffers({ bufptr });

    ASSERT_EQ(res.first, nr::utils::vectorByteSize(originalData));
    
    for (NRuint i = 0; i < originalData.size(); ++i)
    {
        ASSERT_NEAR(((NRfloat*) res.second)[i], originalData[i], TOLERANCE);
    }
}

TEST(UnifyBuffersTest, SameTypeTest)
{
    std::vector<std::vector<NRfloat>> originalData = {
        { 0.1f, -1.1f, 2.1f },
        { 0.2f, -1.2f, 2.2f, -3.2f, 4.2f },
        { -0.3f, 1.3f },
        { 0.4f },
        { -0.5f, -1.5f, 2.5f, -3.5f },
        { 0.6f, 1.6f, 2.6f, -3.6f, -4.6f, 5.6f, -6.6f }
    };

    auto buffers = std::vector<nr::Buffer*>(originalData.size());

    for (NRuint i = 0; i < originalData.size(); ++i)
    {
        buffers[i] = new nr::Buffer(originalData[i]);
    }    

    auto res = nr::utils::unifyBuffers(buffers);

    NRuint size = 0;
    for (const auto& vec : originalData)
    {
        size += nr::utils::vectorByteSize(vec);
    }

    ASSERT_EQ(res.first, size);

    size = 0;
    NRuint i = 0;
    auto data = (NRfloat*) res.second;
    for (NRuint j = 0; j < originalData.size(); ++j)
    {
        for (i = 0; i < originalData[j].size(); ++i)
        {
            ASSERT_NEAR(data[i + size], originalData[j][i], TOLERANCE);
        }
        size += originalData[j].size();
    }

    free(data);
    
    for (auto& buf : buffers)
    {
        delete buf;
    }
}

TEST(UnifiyBuffersTest, MultiTypeTest)
{
    std::vector<NRint> ints = { 1, 2, 3 };

    std::vector<NRchar> hello = { 'h', 'e', 'l', 'l', 'o', '\x00' };

    std::vector<NRulong> ulongs = 
        {
            0ul, 54886484ul, 15165035ul, 
            std::numeric_limits<NRulong>::max() - 1, std::numeric_limits<NRulong>::min() 
        };

    NRbool bools[] = { true, false, false, true, true };
    NRuint boolsSize = 5;

    std::vector<NRdouble> doubles =
        {
            0.1, -0.2354, 0.123456, 48645.12305,
            std::numeric_limits<NRdouble>::max() / 50, std::numeric_limits<NRdouble>::min() 
        };

    auto intsBuf    = nr::Buffer(ints);
    auto helloBuf   = nr::Buffer(hello);
    auto ulongsBuf  = nr::Buffer(ulongs);
    auto boolsBuf   = nr::Buffer(bools, boolsSize);
    auto doublesBuf = nr::Buffer(doubles);

    auto buffersTotalSize = 
        intsBuf.getByteSize() + helloBuf.getByteSize() + ulongsBuf.getByteSize() +
        boolsBuf.getByteSize() + doublesBuf.getByteSize();

    std::vector<nr::Buffer*> buffers = 
        {
            &intsBuf, &helloBuf, &ulongsBuf, &boolsBuf, &doublesBuf
        };


    auto res = nr::utils::unifyBuffers(buffers);
    auto byteSize = res.first;
    auto data = res.second;

    ASSERT_EQ(byteSize, buffersTotalSize);

    auto totalSize = 0u;

    NRint* intsRes = (NRint*)((NRchar*)data + totalSize);
    totalSize += intsBuf.getByteSize();

    NRchar* helloRes = (NRchar*)((NRchar*)data + totalSize);
    totalSize += helloBuf.getByteSize();

    NRulong* ulongsRes = (NRulong*)((NRchar*)data + totalSize);
    totalSize += ulongsBuf.getByteSize();

    NRbool* boolsRes = (NRbool*)((NRchar*)data + totalSize);
    totalSize += boolsBuf.getByteSize();

    NRdouble* doublesRes = (NRdouble*)((NRchar*)data + totalSize);

    NRuint i;

    for (i = 0; i < ints.size(); ++i)
    {
        ASSERT_EQ(intsRes[i], ints[i]);
    }

    for (i = 0; i < hello.size(); ++i)
    {
        ASSERT_EQ(helloRes[i], hello[i]);
    }

    for (i = 0; i < ulongs.size(); ++i)
    {
        ASSERT_EQ(ulongsRes[i], ulongs[i]);
    }

    for (i = 0; i < boolsSize; ++i)
    {
        ASSERT_EQ(boolsRes[i], bools[i]);
    }

    for (i = 0; i < doubles.size(); ++i)
    {
        ASSERT_NEAR(doublesRes[i], doubles[i], DOUBLE_TOLERANCE);
    }

    free(data);
}
