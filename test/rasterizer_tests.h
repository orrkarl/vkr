#include "includes.h"

#include <pipeline/Stage.h>
#include <pipeline/Rasterizer.h>

TEST(RasterizerTest, LoaderSanityTest)
{
    nr::Error err;
    nr::Rasterizer rasterizer(3, err);
    fprintf(stderr, "%d\n", err);
    ASSERT_TRUE(nr::error::isSuccess(err));
}
