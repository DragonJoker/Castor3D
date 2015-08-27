project( LwoImporter )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	LwoImporter
	PARENT_SCOPE
)

set( LWO_VERSION_MAJOR	0 )
set( LWO_VERSION_MINOR	7 )
set( LWO_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	LwoImporter
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
    "LwoImporterPch.hpp"
    "LwoImporterPch.cpp"
)

set_property( TARGET LwoImporter PROPERTY FOLDER "Importers" )
set( Build "yes (version ${LWO_VERSION_MAJOR}.${LWO_VERSION_MINOR}.${LWO_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( LwoImporter ".h;.hpp;.inl;.cpp" )
