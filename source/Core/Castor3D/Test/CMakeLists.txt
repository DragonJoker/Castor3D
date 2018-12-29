project( Castor3DTest )

include_directories( ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/Src
	${Castor3DIncludeDirs}
	${CMAKE_SOURCE_DIR}/Core/CastorTest/Src
	${CMAKE_BINARY_DIR}/Core/CastorTest/Src
	${CMAKE_SOURCE_DIR}/${ASHES_DIR}/TestRenderer/Src
	${CMAKE_BINARY_DIR}/${ASHES_DIR}/TestRenderer/Src
	${CMAKE_SOURCE_DIR}/Plugins/Renderers/TestRenderSystem/Src
	${CMAKE_BINARY_DIR}/Plugins/Renderers/TestRenderSystem/Src
)

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
    ${PROJECT_NAME}
    bin_dos
    "CastorUtils;Castor3D;CastorTest;TestRenderSystem"
    "CastorUtils;Castor3D;CastorTest;TestRenderSystem;${CastorMinLibraries}"
)

if ( MSVC )
	set_property( TARGET ${PROJECT_NAME}
		PROPERTY COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
endif ()

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tests/Castor" )
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
