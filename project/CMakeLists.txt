# main Nraster library

ADD_LIBRARY(Nraster SHARED
		src/base/Buffer.cpp
		src/base/Context.cpp
		src/base/Event.cpp
		src/base/Exceptions.cpp
		src/base/Kernel.cpp
		src/base/MemoryView.cpp
        src/base/Platform.cpp
		src/base/RootDevice.cpp)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PUBLIC ${OpenCL_INCLUDE_DIRS})
	