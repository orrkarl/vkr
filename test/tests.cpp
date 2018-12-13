#include <vector>
#include <unordered_map>
#include <memory>

#include <gtest/gtest.h>

#include "../lib/inc/buffers/Buffer.h"
#include "../lib/inc/utils/memory.h"

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

const float TOLERANCE = 0.001f;

TEST(UnifiyBuffersTest, SanityTest)
{
    std::vector<NRfloat> originalData = { 0.1f, 0.2f, 0.3f };
    void* data = originalData.data();

    auto buf = nr::Buffer(originalData);
    auto bufptr = &buf;

    std::unordered_map<NRuint, nr::Buffer*> map = { { 0, bufptr } };
    auto res = nr::unifyBuffers(map);

    ASSERT_EQ(res.first, originalData.size());
    
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

    auto buffers = std::vector<std::unique_ptr<nr::Buffer>>(originalData.size());
    auto map = std::unordered_map<NRuint, nr::Buffer*>();

    for (NRuint i = 0; i < originalData.size(); ++i)
    {
        buffers[i] = std::unique_ptr<nr::Buffer>(new nr::Buffer(originalData[i]));
        map.insert({ i, buffers[i].get() });
    }    

    auto res = nr::unifyBuffers(map);

    NRuint size = 0;
    for (const auto& vec : originalData)
    {
        size += vec.size();
    }

    ASSERT_EQ(res.first, size);

    size = 0;
    NRuint i = 0;
    for (NRuint j = 0; j < originalData.size(); ++j)
    {
        for (i = 0; i < originalData[j].size(); ++j)
        {
            ASSERT_NEAR(((NRfloat*) res.second)[size + i], originalData[j][i], TOLERANCE);
        }
        size += i;
    }
}

