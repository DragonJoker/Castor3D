project( CastorShape )

include_directories( ../GuiCommon/Src )

set( CS_VERSION_MAJOR	0 )
set( CS_VERSION_MINOR	6 )
set( CS_VERSION_BUILD	5 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	CastorShape
	bin
	"${CastorBinsDependencies}"
	"${CastorBinLibraries}"
	"PrecompiledHeader.hpp"
	"PrecompiledHeader.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
# Copying internationalisation files (french, for now) for CastorShape
copy_languages( CastorShape ../internationalisation CastorShape "fr" )

set_property( TARGET CastorShape PROPERTY FOLDER "Samples" )
set( Build "yes (version ${CS_VERSION_MAJOR}.${CS_VERSION_MINOR}.${CS_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( CastorShape ".h;.hpp;.inl;.cpp" )
