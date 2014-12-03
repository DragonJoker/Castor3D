project( ObjImporter )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	ObjImporter
	PARENT_SCOPE
)

set( OBJ_VERSION_MAJOR	0 )
set( OBJ_VERSION_MINOR	7 )
set( OBJ_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	ObjImporter
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET ObjImporter PROPERTY FOLDER "Importers" )
set( Build "yes (version ${OBJ_VERSION_MAJOR}.${OBJ_VERSION_MINOR}.${OBJ_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ObjImporter ".h;.hpp;.inl;.cpp" )
