CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# main Nraster library

ADD_LIBRARY(Nraster SHARED
	src/utils/converters.cpp
	src/utils/files.cpp

	src/base/Module.cpp
	src/base/Buffer.cpp

	src/pipeline/BinRasterizer.cpp

	src/general/predefs.cpp)

TARGET_LINK_LIBRARIES(Nraster
	PRIVATE ${OPENGL_LIBRARIES}
	PRIVATE ${GLEW_LIBRARIES}
	PRIVATE OpenCL::OpenCL)

TARGET_INCLUDE_DIRECTORIES(Nraster
	PRIVATE ${OPENGL_INCLUDE_DIRS}
	PRIVATE ${GLEW_INCLUDE_DIRS}
	PRIVATE ${OpenCL_INCLUDE_DIRS}
	PRIVATE inc)

SET(NRASTER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/public CACHE INTERNAL "")