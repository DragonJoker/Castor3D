project( Castor3DTest )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorTest/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorTest/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/TestRenderSystem/Src )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	9 )
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

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Test" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
add_target_test( ${PROJECT_NAME} )

# Copying test data files
file(
	GLOB
		DataFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.zip
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cscn
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cmsh
)

copy_target_files( ${PROJECT_NAME} "data" ${DataFiles} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
