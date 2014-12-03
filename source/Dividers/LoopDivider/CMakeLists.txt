project( LoopDivider )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	LoopDivider
	PARENT_SCOPE
)

set( LOOP_VERSION_MAJOR	0 )
set( LOOP_VERSION_MINOR	7 )
set( LOOP_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	LoopDivider
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
	"LoopPch.hpp"
	"LoopPch.cpp"
)

set_property( TARGET LoopDivider PROPERTY FOLDER "Dividers" )
set( Build "yes (version ${LOOP_VERSION_MAJOR}.${LOOP_VERSION_MINOR}.${LOOP_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( LoopDivider ".h;.hpp;.inl;.cpp" )
