CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# main Nraster library

ADD_LIBRARY(Nraster SHARED
	src/base/Module.cpp
	src/base/Buffer.cpp

	src/general/predefs.cpp
	
	src/pipeline/BinRasterizer.cpp
	src/pipeline/FineRasterizer.cpp

	src/utils/converters.cpp)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PRIVATE ${OpenCL_INCLUDE_DIRS}
	PRIVATE inc)