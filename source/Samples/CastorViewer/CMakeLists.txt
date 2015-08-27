project( CastorViewer )

include_directories( ../GuiCommon/Src )

set( CV_VERSION_MAJOR	0 )
set( CV_VERSION_MINOR	7 )
set( CV_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	CastorViewer
	bin
	"${CastorBinsDependencies}"
	"${CastorBinLibraries}"
	"PrecompiledHeader.hpp"
	"PrecompiledHeader.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
# Copying internationalisation files (french, for now) for CastorViewer
copy_languages( CastorViewer ../internationalisation CastorViewer "fr" )

set_property( TARGET CastorViewer PROPERTY FOLDER "Samples" )
set( Build "yes (version ${CV_VERSION_MAJOR}.${CV_VERSION_MINOR}.${CV_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( CastorViewer ".h;.hpp;.inl;.cpp" )