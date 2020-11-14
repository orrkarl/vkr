# main Nraster library

ADD_LIBRARY(Nraster SHARED
		base/Buffer.cpp
		base/CommandQueue.cpp
		base/Context.cpp
		base/Event.cpp
		base/Exceptions.cpp
		base/Kernel.cpp
		base/Module.cpp
		base/Platform.cpp
		base/RootDevice.cpp
		base/Views.cpp)

TARGET_LINK_LIBRARIES(Nraster
	PUBLIC ${OpenCL_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(Nraster
	PUBLIC ${OpenCL_INCLUDE_DIRS})
	