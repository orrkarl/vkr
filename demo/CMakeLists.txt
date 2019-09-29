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

ADD_EXECUTABLE(Cube3d src/cube3d.cpp)
TARGET_LINK_LIBRARIES(Cube3d NrasterDemoUtils)

ADD_EXECUTABLE(Simpelxes src/simplexes.cpp)
TARGET_LINK_LIBRARIES(Simpelxes NrasterDemoUtils)

IF(WIN32)
    # Copy DLL files to output directory. Yes, this is digusting, but nothing else worked :(	
	ADD_CUSTOM_COMMAND(
		TARGET Cube3d
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E copy
		"${PROJECT_BINARY_DIR}/project/Nraster.dll"         
		${CMAKE_CURRENT_BINARY_DIR}
	)
	
	ADD_CUSTOM_COMMAND(
		TARGET Simpelxes
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E copy
		"${PROJECT_BINARY_DIR}/project/Nraster.dll"         
		${CMAKE_CURRENT_BINARY_DIR}
	)

	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		ADD_CUSTOM_COMMAND(
			TARGET Cube3d
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy
			"${PROJECT_BINARY_DIR}/bin/glewd.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)	

		ADD_CUSTOM_COMMAND(
			TARGET Simpelxes
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy
			"${PROJECT_BINARY_DIR}/bin/glewd.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)	
	ELSE()
		ADD_CUSTOM_COMMAND(
			TARGET Cube3d
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy
			"${PROJECT_BINARY_DIR}/bin/glew.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)	

		ADD_CUSTOM_COMMAND(
			TARGET Simpelxes
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E copy
			"${PROJECT_BINARY_DIR}/bin/glew.dll"         
			${CMAKE_CURRENT_BINARY_DIR}
		)
	ENDIF()

ENDIF()
