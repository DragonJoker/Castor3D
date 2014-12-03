project( AseImporter )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	AseImporter
	PARENT_SCOPE
)

set( ASE_VERSION_MAJOR	0 )
set( ASE_VERSION_MINOR	7 )
set( ASE_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	AseImporter
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
    "AseImporterPch.hpp"
    "AseImporterPch.cpp"
)

set_property( TARGET AseImporter PROPERTY FOLDER "Importers" )
set( Build "yes (version ${ASE_VERSION_MAJOR}.${ASE_VERSION_MINOR}.${ASE_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( AseImporter ".h;.hpp;.inl;.cpp" )
