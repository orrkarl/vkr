# main Nraster library

ADD_LIBRARY(Nraster SHARED
	src/base/Buffer.cpp
	src/base/CommandQueue.cpp
	src/base/Context.cpp
	src/base/Device.cpp
	src/base/Event.cpp
	src/base/Kernel.cpp
	src/base/Module.cpp
	src/base/Platform.cpp
	
	src/kernels/base.cl.cpp
	src/kernels/bin_rasterizer.cl.cpp
	src/kernels/fine_rasterizer.cl.cpp
	src/kernels/simplex_reducer.cl.cpp
	src/kernels/vertex_reducer.cl.cpp

	src/pipeline/Pipeline.cpp
	src/pipeline/Source.cpp

	src/utils/converters.cpp
	src/utils/rendermath.cpp)

TARGET_COMPILE_DEFINITIONS(Nraster PRIVATE NR_EXPORT)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PUBLIC ${OpenCL_INCLUDE_DIRS})
	