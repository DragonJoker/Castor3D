project( MsaaTechnique )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	MsaaTechnique
	PARENT_SCOPE
)

set( MSAA_VERSION_MAJOR	0 )
set( MSAA_VERSION_MINOR	7 )
set( MSAA_VERSION_BUILD	1 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	MsaaTechnique
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET MsaaTechnique PROPERTY FOLDER "Techniques" )
set( Build "yes (version ${MSAA_VERSION_MAJOR}.${MSAA_VERSION_MINOR}.${MSAA_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( MsaaTechnique ".h;.hpp;.inl;.cpp" )
