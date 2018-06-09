project( CastorMeshUpgrader )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D mesh file upgrader." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${Castor3DIncludeDirs} )

add_target(
	${PROJECT_NAME}
	bin_dos
	"Castor3D"
	"Castor3D"
	""
	""
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
