project( GuiCommon )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	GuiCommon
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorMinLibraries}
	${wxWidgetsLibraries}
	GuiCommon
	PARENT_SCOPE
)

set( GC_VERSION_MAJOR	0 )
set( GC_VERSION_MINOR	7 )
set( GC_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	GuiCommon
	lib
	"Castor3D"
	"${CastorBinLibraries}"
	"GuiCommonPch.hpp"
	"GuiCommonPch.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)

# Copying language files for GLSL and HLSL coloration in wxShaderDialog
file(
	GLOB_RECURSE
		Parser_languages tools/resource/GuiCommon/languages/*.lang
)

set_property( TARGET GuiCommon PROPERTY FOLDER "Samples" )
add_target_astyle( GuiCommon ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${GC_VERSION_MAJOR}.${GC_VERSION_MINOR}.${GC_VERSION_BUILD})" PARENT_SCOPE )

if ( {${CASTOR_PACKAGE_DEB} )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "wxWidgets >= ${wxWidgetsVersion}" )
endif ()