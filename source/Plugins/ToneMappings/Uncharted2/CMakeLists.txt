project( Uncharted2ToneMapping )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Uncharted 2 Tone Mapping" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_definitions( ${ToneMappingDefinitions} )
include_directories( ${ToneMappingIncludes} )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/ToneMappings/Uncharted2
	${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings/Uncharted2
	"Castor3D"
	"${ToneMappingLibraries}"
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/ToneMappings" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
