# main Nraster library

ADD_LIBRARY(NrasterBackend STATIC
		base/Buffer.cpp
		base/CommandQueue.cpp
		base/Context.cpp
		base/Event.cpp
		base/Exceptions.cpp
		base/Kernel.cpp
		base/Module.cpp
		base/Platform.cpp
		base/Device.cpp
		base/Views.cpp)

TARGET_LINK_LIBRARIES(NrasterBackend PUBLIC OpenCL::OpenCL)
TARGET_INCLUDE_DIRECTORIES(NrasterBackend INTERFACE "../src")
