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

	src/utils/converters.cpp
	
	pch.cpp)

ADD_PRECOMPILED_HEADER(Nraster pch.h SOURCE_CXX pch.cpp FORCEINCLUDE)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PRIVATE ${OpenCL_INCLUDE_DIRS}
	PRIVATE inc)