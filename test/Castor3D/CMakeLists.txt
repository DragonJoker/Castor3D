project( Castor3DTest )

include_directories(
	${Castor3DIncludeDirs}
	${CMAKE_CURRENT_BINARY_DIR}
	${CASTOR_SOURCE_DIR}/test/CastorTest
	${CASTOR_BINARY_DIR}/test/CastorTest
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers
	${CASTOR_BINARY_DIR}/include/Plugins/Renderers
)

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_target(
	${PROJECT_NAME}
	bin_dos
	${CASTOR_SOURCE_DIR}/test/Castor3D
	${CASTOR_SOURCE_DIR}/test/Castor3D
	"CastorUtils;Castor3D;CastorTest"
	"CastorUtils;Castor3D;CastorTest;${CastorMinLibraries};${CastorBinsDependencies}"
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
