INCLUDE_DIRECTORIES(
	PUBLIC ${NRASTER_INCLUDE_DIRS}
	PUBLIC ${OpenGL_INCLUDE_DIRS}
	PUBLIC ${glew_INCLUDE_DIRS}
	PUBLIC ${glfw_INCLUDE_DIRS})

ADD_LIBRARY(NrasterDemoUtils src/utils.cpp src/linalg.cpp src/app.cpp)

TARGET_LINK_LIBRARIES(NrasterDemoUtils 
	PUBLIC Nraster
	PUBLIC ${OpenGL_LIBRARIES}
	PUBLIC glfw
	PUBLIC glew)

ADD_EXECUTABLE(Triangle3d src/triangle3d.cpp)
TARGET_LINK_LIBRARIES(Triangle3d NrasterDemoUtils)

ADD_EXECUTABLE(Cube4d src/cube4d.cpp)
TARGET_LINK_LIBRARIES(Cube4d NrasterDemoUtils)

ADD_EXECUTABLE(Simpelxes src/simplexes.cpp)
TARGET_LINK_LIBRARIES(Simpelxes NrasterDemoUtils)

SET(ALL_DEMOS "Triangle3d Cube4d Simplexes")

IF(WIN32)
    # Copy DLL files to output directory. Yes, this is digusting, but nothing else worked :(
	
	ADD_CUSTOM_COMMAND(
		TARGET Triangle3d Cube4d Simpelxes 
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${PROJECT_BINARY_DIR}/project/Nraster.dll"         
		${CMAKE_CURRENT_BINARY_DIR}
	)

	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		ADD_CUSTOM_COMMAND(
			TARGET Triangle3d Cube4d Simpelxes
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/bin/glewd.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)	
	ELSE()
		ADD_CUSTOM_COMMAND(
			TARGET Triangle3d Cube4d Simpelxes
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/bin/glew.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)
	ENDIF()

ENDIF()
