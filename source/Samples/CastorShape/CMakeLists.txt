project( CastorShape )

include_directories( ../GuiCommon/Src )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Scene Editor for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	bin
	"${CastorBinsDependencies}"
	"${CastorBinLibraries}"
	"PrecompiledHeader.hpp"
	"PrecompiledHeader.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
# Copying internationalisation files (french, for now) for CastorShape
copy_languages( ${PROJECT_NAME} ../internationalisation ${PROJECT_NAME} "fr" )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

if ( WIN32 )
	if( FFMPEG_FOUND )
		# Preparing FFMPEG dlls installation
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVCODEC} ${FFMPEG_LIBAVCODEC} -${FFMPEG_AVCODEC_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVFORMAT} ${FFMPEG_LIBAVFORMAT} -${FFMPEG_AVFORMAT_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVUTIL} ${FFMPEG_LIBAVUTIL} -${FFMPEG_AVUTIL_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWSCALE} ${FFMPEG_LIBSWSCALE} -${FFMPEG_SWSCALE_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWRESAMPLE} ${FFMPEG_LIBSWRESAMPLE} -${FFMPEG_SWRESAMPLE_VERSION_MAJOR} )
	endif ()
endif ()
