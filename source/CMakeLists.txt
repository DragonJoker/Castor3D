get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

include_directories(
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_BINARY_DIR}/include/Core
)

set( Castor3DIncludeDirs
	${CASTOR_SOURCE_DIR}/${ASHES_DIR}/include
	${CASTOR_BINARY_DIR}/${ASHES_DIR}/source
	${CASTOR_SOURCE_DIR}/${SHADERWRITER_DIR}/include
	${CASTOR_BINARY_DIR}/${SHADERWRITER_DIR}/source
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_BINARY_DIR}/include/Core
	${CASTOR_BINARY_DIR}/source/Core
)

add_subdirectory( Core )
if ( ${CASTOR_BUILD_CASTOR3D} )
	add_subdirectory( Plugins )
	add_subdirectory( Samples )
	add_subdirectory( Interop )
endif ()
add_subdirectory( Tools )

CoreInit()
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsInit()
	SamplesInit()
	InteropInit()
endif ()
ToolsInit()

CoreBuild()
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsBuild()
	SamplesBuild()
	InteropBuild()
endif ()
ToolsBuild()

set( msg "\nCastor following projects will be built" )
CoreSummary( ${msg} )
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsSummary( ${msg} )
	SamplesSummary( ${msg} )
	InteropSummary( ${msg} )
endif ()
ToolsSummary( ${msg} )
message( STATUS "${msg}\n" )
