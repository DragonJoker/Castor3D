project( ImgConverter )

set( IC_VERSION_MAJOR	1 )
set( IC_VERSION_MINOR	1 )
set( IC_VERSION_BUILD	0 )

add_target(
	ImgConverter
	bin
	""
	"${wxWidgetsLibraries}"
	"PrecompiledHeader.hpp"
	"PrecompiledHeader.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
# Copying internationalisation files (french, for now) for ImgConverter
copy_languages( ImgConverter ../internationalisation ImgConverter "fr" )

set_property( TARGET ImgConverter PROPERTY FOLDER "Tools" )
set( Build "yes (version ${IC_VERSION_MAJOR}.${IC_VERSION_MINOR}.${IC_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ImgConverter ".h;.hpp;.inl;.cpp" )
