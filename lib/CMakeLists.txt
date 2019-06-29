CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# main Nraster library

ADD_LIBRARY(Nraster SHARED
	src/base/CommandQueue.cpp
	src/base/Context.cpp
	src/base/Device.cpp
	src/base/Event.cpp
	src/base/Kernel.cpp
	src/base/Module.cpp
	src/base/Platform.cpp

	src/general/predefs.cpp
	
	src/pipeline/BinRasterizer.cpp
	src/pipeline/FineRasterizer.cpp
	src/pipeline/VertexShader.cpp

	src/utils/converters.cpp)

TARGET_COMPILE_DEFINITIONS(Nraster PRIVATE NR_EXPORT)

MESSAGE(${CMAKE_CXX_FLAGS})
MESSAGE(${CMAKE_CXX_FLAGS_DEBUG})

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PUBLIC ${OpenCL_INCLUDE_DIRS}
	PUBLIC inc)