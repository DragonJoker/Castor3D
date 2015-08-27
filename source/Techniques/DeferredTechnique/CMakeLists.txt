project( DeferredTechnique )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	DeferredTechnique
	PARENT_SCOPE
)

set( DEFR_VERSION_MAJOR	0 )
set( DEFR_VERSION_MINOR	7 )
set( DEFR_VERSION_BUILD	1 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	DeferredTechnique
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
)

set_property( TARGET DeferredTechnique PROPERTY FOLDER "Techniques" )
set( Build "yes (version ${DEFR_VERSION_MAJOR}.${DEFR_VERSION_MINOR}.${DEFR_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( DeferredTechnique ".h;.hpp;.inl;.cpp" )
