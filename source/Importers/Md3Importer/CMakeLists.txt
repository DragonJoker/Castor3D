project( Md3Importer )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	Md3Importer
	PARENT_SCOPE
)

set( MD3_VERSION_MAJOR	0 )
set( MD3_VERSION_MINOR	7 )
set( MD3_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	Md3Importer
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET Md3Importer PROPERTY FOLDER "Importers" )
set( Build "yes (version ${MD3_VERSION_MAJOR}.${MD3_VERSION_MINOR}.${MD3_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( Md3Importer ".h;.hpp;.inl;.cpp" )
