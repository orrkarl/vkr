# main Nraster library

ADD_LIBRARY(Nraster SHARED
	src/base/Buffer.cpp
	src/base/Context.cpp
	src/base/Exceptions.cpp)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PUBLIC ${OpenCL_INCLUDE_DIRS})
	