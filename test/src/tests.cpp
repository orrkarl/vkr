#include "includes.h"
#include "modules/all.h"
#include "pipeline/all.h"

#include <iostream>

int main(int argc, char **argv) 
{
    cl_status initStatus = CL_SUCCESS;
    if ((initStatus = init()) != CL_SUCCESS) 
    {
        std::cerr << "init failed: " << nr::utils::stringFromCLError(initStatus) << std::endl;
        return initStatus;
    }
	
    ::testing::InitGoogleTest(&argc, argv);
	
	auto ret = RUN_ALL_TESTS();
	
	destroy();
    return ret;
}

