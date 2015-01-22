project( PhongDivider )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	PhongDivider
	PARENT_SCOPE
)

set( PHONG_VERSION_MAJOR	0 )
set( PHONG_VERSION_MINOR	7 )
set( PHONG_VERSION_BUILD	1 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	PhongDivider
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET PhongDivider PROPERTY FOLDER "Dividers" )
set( Build "yes (version ${PHONG_VERSION_MAJOR}.${PHONG_VERSION_MINOR}.${PHONG_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( PhongDivider ".h;.hpp;.inl;.cpp" )
