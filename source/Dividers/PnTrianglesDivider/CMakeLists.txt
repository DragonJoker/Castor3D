project( PnTrianglesDivider )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	PnTrianglesDivider
	PARENT_SCOPE
)

set( PNTRI_VERSION_MAJOR	0 )
set( PNTRI_VERSION_MINOR	7 )
set( PNTRI_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	PnTrianglesDivider
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET PnTrianglesDivider PROPERTY FOLDER "Dividers" )
set( Build "yes (version ${PNTRI_VERSION_MAJOR}.${PNTRI_VERSION_MINOR}.${PNTRI_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( PnTrianglesDivider ".h;.hpp;.inl;.cpp" )
