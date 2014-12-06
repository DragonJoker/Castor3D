project( CastorUtilsTest )

find_package( GLM )
find_package( OpenCL )

if( GLM_FOUND AND OPENCL_FOUND )
	if( GLM_FOUND )
		include_directories( ${GLM_INCLUDE_DIR} )
		message( STATUS "+ Found GLM" )
	endif()

	if( OPENCL_FOUND )
		include_directories( ${OPENCL_INCLUDE_DIRS} )
		message( STATUS "+ Found OpenCL" )
	endif()

    include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
    include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

	set( OpenCLLibraries "" )
	foreach( Lib ${OPENCL_LIBRARIES} )
		if( OpenCLLibraries )
			set( OpenCLLibraries "${OpenCLLibraries}¤${Lib}" )
		else()
			set( OpenCLLibraries "${Lib}" )
		endif()
	endforeach()

    set( CT_VERSION_MAJOR	0 )
    set( CT_VERSION_MINOR	7 )
    set( CT_VERSION_BUILD	0 )

	add_target(
		CastorUtilsTest
		bin_dos
		"CastorUtils"
		"CastorUtils;${CastorMinLibraries};${OpenCLLibraries}"
	)

	set_property( TARGET CastorUtilsTest PROPERTY FOLDER "Tools" )
	add_target_astyle( CastorUtilsTest ".h;.hpp;.inl;.cpp" )
    add_target_test( CastorUtilsTest )
	set( Build "yes (version ${CT_VERSION_MAJOR}.${CT_VERSION_MINOR}.${CT_VERSION_BUILD})" PARENT_SCOPE )
else()
	set( tmp "no (missing" )
	if( NOT GLM_FOUND )
		set( tmp "${tmp} glm" )
	endif()
	if( NOT OPENCL_FOUND )
		set( tmp "${tmp} opencl" )
	endif()
	set( tmp "${tmp})" )
	set( Build "${tmp}" PARENT_SCOPE )
endif()
