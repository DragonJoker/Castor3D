project( SMaxImporter )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	SMaxImporter
	PARENT_SCOPE
)

set( 3DS_VERSION_MAJOR	0 )
set( 3DS_VERSION_MINOR	7 )
set( 3DS_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	SMaxImporter
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET SMaxImporter PROPERTY FOLDER "Importers" )
set( Build "yes (version ${3DS_VERSION_MAJOR}.${3DS_VERSION_MINOR}.${3DS_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( SMaxImporter ".h;.hpp;.inl;.cpp" )
