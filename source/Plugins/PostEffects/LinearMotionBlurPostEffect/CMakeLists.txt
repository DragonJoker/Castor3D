project( LinearMotionBlurPostEffect )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Linear Motion Blur Post Effect" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_definitions( ${PostEffectDefinitions} )
include_directories( ${PostEffectIncludes} )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${PostEffectLibraries}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/PostEffects" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
