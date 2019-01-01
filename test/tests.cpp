#include "includes.h"

#include "unify_buffers_tests.h"
#include "rasterizer_tests.h"

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

