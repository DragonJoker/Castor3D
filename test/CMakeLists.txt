get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

include_directories(
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_BINARY_DIR}/include/Core
)

set( Castor3DIncludeDirs
	${Castor3DIncludeDirs}
	${CASTOR_BINARY_DIR}/include/Core
	${CASTOR_SOURCE_DIR}/include/Core
)

set( msgtest "Test projects :" )

set( Build ${CT} )
add_subdirectory( CastorTest )
set( CT ${Build} )

if ( CASTOR_BUILD_TEST_CASTORUTILS )
	set( Build ${CUtlT} )
	add_subdirectory( CastorUtils )
	set( CUtlT ${Build} )
endif ()

if ( CASTOR_BUILD_CASTOR3D AND CASTOR_BUILD_TEST_CASTOR3D )
	set( Build ${C3DT} )
	add_subdirectory( Castor3D )
	set( C3DT ${Build} )
endif()

if( CASTOR_BUILD_TEST_INTEROP_COM )
	set( Build ${ComC3DT} )
	add_subdirectory( ComCastor3D )
	set( ComC3DT ${Build} )
endif()

set( msgtest_tmp "${msgtest_tmp}\n    CastorTest           ${CT}" )
set( msgtest_tmp "${msgtest_tmp}\n    CastorUtilsTest      ${CUtlT}" )
set( msgtest_tmp "${msgtest_tmp}\n    Castor3DTest         ${C3DT}" )
set( msgtest_tmp "${msgtest_tmp}\n    ComCastor3DTest      ${ComC3DT}" )
set( msgtest "${msgtest}${msgtest_tmp}" )
message( STATUS "${msgtest}\n" )
