project( PlyImporter )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	PlyImporter
	PARENT_SCOPE
)

set( PLY_VERSION_MAJOR	0 )
set( PLY_VERSION_MINOR	7 )
set( PLY_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	PlyImporter
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET PlyImporter PROPERTY FOLDER "Importers" )
set( Build "yes (version ${PLY_VERSION_MAJOR}.${PLY_VERSION_MINOR}.${PLY_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( PlyImporter ".h;.hpp;.inl;.cpp" )
