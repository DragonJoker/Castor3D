project( SsaaTechnique )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	SsaaTechnique
	PARENT_SCOPE
)

set( SSAA_VERSION_MAJOR	0 )
set( SSAA_VERSION_MINOR	7 )
set( SSAA_VERSION_BUILD	1 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	SsaaTechnique
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET SsaaTechnique PROPERTY FOLDER "Techniques" )
set( Build "yes (version ${SSAA_VERSION_MAJOR}.${SSAA_VERSION_MINOR}.${SSAA_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( SsaaTechnique ".h;.hpp;.inl;.cpp" )
