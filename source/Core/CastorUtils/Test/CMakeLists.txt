project( CastorUtilsTest )

find_package( GLM )
find_package( OpenCL )

set( CUT_C_FLAGS "" )
set( CUT_CXX_FLAGS "" )

set( OpenCLLibraries "" )

if( GLM_FOUND )
	include_directories( ${GLM_INCLUDE_DIR} )
	message( STATUS "+ Found GLM" )
	set( CUT_C_FLAGS "${CUT_C_FLAGS} -DCASTOR_USE_GLM" )
	set( CUT_CXX_FLAGS "${CUT_CXX_FLAGS} -DCASTOR_USE_GLM" )
endif()

if( OPENCL_FOUND )
	include_directories( ${OPENCL_INCLUDE_DIRS} )
	message( STATUS "+ Found OpenCL" )
	set( CUT_C_FLAGS "${CUT_C_FLAGS} -DCASTOR_USE_OCL" )
	set( CUT_CXX_FLAGS "${CUT_CXX_FLAGS} -DCASTOR_USE_OCL" )
	foreach( Lib ${OPENCL_LIBRARIES} )
		if( OpenCLLibraries )
			set( OpenCLLibraries "${OpenCLLibraries}¤${Lib}" )
		else()
			set( OpenCLLibraries "${Lib}" )
		endif()
	endforeach()
endif()

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorTest/Src )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	bin_dos
	"CastorUtils;CastorTest"
	"CastorUtils;CastorTest;${CastorMinLibraries};${OpenCLLibraries}"
	"" ""
	"${CUT_C_FLAGS}"
	"${CUT_CXX_FLAGS}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Test" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
copy_target_files( ${PROJECT_NAME} "" ${CMAKE_CURRENT_SOURCE_DIR}/Src/mtx_ope.cl )

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
