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

	auto ret = RUN_ALL_TESTS();

	if ((initStatus = destroy()) != CL_SUCCESS)
	{
		std::cerr << "Destroy failed: " << utils::stringFromCLError(initStatus) << std::endl;
	}

    return ret;
}

