project( Md2Importer )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	Md2Importer
	PARENT_SCOPE
)

set( MD2_VERSION_MAJOR	0 )
set( MD2_VERSION_MINOR	7 )
set( MD2_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	Md2Importer
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET Md2Importer PROPERTY FOLDER "Importers" )
set( Build "yes (version ${MD2_VERSION_MAJOR}.${MD2_VERSION_MINOR}.${MD2_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( Md2Importer ".h;.hpp;.inl;.cpp" )
