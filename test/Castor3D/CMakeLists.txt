project( Castor3DTest )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/BinaryExportTest.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Castor3DTestCommon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Castor3DTestPrerequisites.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/SceneExportTest.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/BinaryExportTest.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Castor3DTestCommon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SceneExportTest.cpp
)
add_target_min(
	${PROJECT_NAME}
	bin_dos
	""
	""
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Castor3DIncludeDirs}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	castor::Castor3D
	castor::CastorTest
)

if ( MSVC )
	set_property( TARGET ${PROJECT_NAME}
		PROPERTY COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
endif ()

set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		FOLDER "Tests/Castor"
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

# Copying test data files
file(
	GLOB
		DataFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.zip
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cscn
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cmsh
)

copy_target_files( ${PROJECT_NAME} "data" ${DataFiles} )

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
