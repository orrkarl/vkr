INCLUDE_DIRECTORIES(
	PUBLIC inc
	PUBLIC ${NRASTER_ALL_INCLUDES}
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

ADD_EXECUTABLE(Triangles src/triangles.cpp)
TARGET_LINK_LIBRARIES(Triangles NrasterDemoUtils)

IF(WIN32)
    # Copy DLL files to output directory. Yes, this is digusting, but nothing else worked :(
	
	ADD_CUSTOM_COMMAND(TARGET Triangle3d POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${PROJECT_BINARY_DIR}/lib/Nraster.dll"         
		$<TARGET_FILE_DIR:Triangle3d>
	)
		
	ADD_CUSTOM_COMMAND(TARGET Cube4d POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${PROJECT_BINARY_DIR}/lib/Nraster.dll"         
		$<TARGET_FILE_DIR:Cube4d>
	)

	IF(CMAKE_BUILD_TYPE STREQUAL Debug)
		ADD_CUSTOM_COMMAND(TARGET Triangle3d POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/bin/glewd.dll"         
			$<TARGET_FILE_DIR:Triangle3d>
		)
		ADD_CUSTOM_COMMAND(TARGET Cube4d POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/bin/glewd.dll"         
			$<TARGET_FILE_DIR:Cube4d>
		)	
	ELSE()
		ADD_CUSTOM_COMMAND(TARGET Triangle3d POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${PROJECT_BINARY_DIR}/bin/glew.dll"         
				$<TARGET_FILE_DIR:Triangle3d>
		)
		ADD_CUSTOM_COMMAND(TARGET Cube4d POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${PROJECT_BINARY_DIR}/bin/glew.dll"         
				$<TARGET_FILE_DIR:Cube4d>
		)
	ENDIF()
ENDIF()
