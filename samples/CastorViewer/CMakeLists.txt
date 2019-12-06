project( CastorViewer )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Scene Viewer for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/PrecompiledHeader.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/PrecompiledHeader.cpp
	"${wxWidgets_CFLAGS} ${GUICOMMON_C_FLAGS}"
	"${wxWidgets_CXXFLAGS} ${GUICOMMON_CXX_FLAGS}"
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Castor3DIncludeDirs}
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${Castor3DBinsDependencies}
		castor::GuiCommon
)
# Copying internationalisation files (french, for now) for CastorViewer
copy_languages( ${PROJECT_NAME} ${CASTOR_SOURCE_DIR}/internationalisation/${PROJECT_NAME} ${PROJECT_NAME} "fr" )

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
