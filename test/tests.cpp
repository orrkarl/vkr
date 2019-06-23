#include "inc/includes.h"

#include "inc/base/all.h"
#include "inc/modules/all_module_tests.h"

#include <iostream>

int main(int argc, char **argv) 
{
    cl_status initStatus = CL_SUCCESS;
    if ((initStatus = init()) != CL_SUCCESS) 
    {
        std::cerr << "init failed: " << utils::stringFromCLError(initStatus) << std::endl;
        return initStatus;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

