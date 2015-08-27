project( SsaoPostEffect )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	SsaoPostEffect
	PARENT_SCOPE
)

set( SSAO_VERSION_MAJOR	0 )
set( SSAO_VERSION_MINOR	1 )
set( SSAO_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	SsaoPostEffect
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET SsaoPostEffect PROPERTY FOLDER "PostEffects" )
set( Build "yes (version ${SSAO_VERSION_MAJOR}.${SSAO_VERSION_MINOR}.${SSAO_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( SsaoPostEffect ".h;.hpp;.inl;.cpp" )
