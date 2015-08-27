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

set( CT_VERSION_MAJOR	0 )
set( CT_VERSION_MINOR	7 )
set( CT_VERSION_BUILD	1 )

add_target(
    CastorUtilsTest
    bin_dos
    "CastorUtils"
    "CastorUtils;${CastorMinLibraries};${OpenCLLibraries}"
    "" ""
    "${CUT_C_FLAGS}"
    "${CUT_CXX_FLAGS}"
)

set_property( TARGET CastorUtilsTest PROPERTY FOLDER "Tools" )
add_target_astyle( CastorUtilsTest ".h;.hpp;.inl;.cpp" )
add_target_test( CastorUtilsTest )
set( Build "yes (version ${CT_VERSION_MAJOR}.${CT_VERSION_MINOR}.${CT_VERSION_BUILD})" PARENT_SCOPE )
