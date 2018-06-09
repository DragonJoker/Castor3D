project( LoopDivider )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Loop Tessellation Divider" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${Castor3DIncludeDirs} )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
	"LoopPch.hpp"
	"LoopPch.cpp"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Dividers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
