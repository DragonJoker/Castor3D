project( CastorViewer )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Scene Viewer for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 8 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorViewer.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/KeyboardEvent.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MainFrame.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MouseNodeEvent.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RenderPanel.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RotateNodeEvent.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/TranslateNodeEvent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorViewer.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/KeyboardEvent.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MainFrame.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MouseNodeEvent.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RenderPanel.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RotateNodeEvent.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/TranslateNodeEvent.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
if ( WIN32 )
	find_rsc_file( ${PROJECT_NAME} bin )
endif ()
add_target_min(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/PrecompiledHeader.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/PrecompiledHeader.cpp
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
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		FOLDER "Samples"
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}
)
# Copying internationalisation files (french, for now) for CastorViewer
copy_languages_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/internationalisation/${PROJECT_NAME}
	${PROJECT_NAME}
	"fr"
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

if ( WIN32 )
	if( FFMPEG_FOUND )
		# Preparing FFMPEG dlls installation
		copy_dll_ex( ${PROJECT_NAME} ${PROJECT_NAME} ${FFMPEG_LIBAVCODEC} ${FFMPEG_LIBAVCODEC} -${FFMPEG_AVCODEC_VERSION_MAJOR} )
		copy_dll_ex( ${PROJECT_NAME} ${PROJECT_NAME} ${FFMPEG_LIBAVFORMAT} ${FFMPEG_LIBAVFORMAT} -${FFMPEG_AVFORMAT_VERSION_MAJOR} )
		copy_dll_ex( ${PROJECT_NAME} ${PROJECT_NAME} ${FFMPEG_LIBAVUTIL} ${FFMPEG_LIBAVUTIL} -${FFMPEG_AVUTIL_VERSION_MAJOR} )
		copy_dll_ex( ${PROJECT_NAME} ${PROJECT_NAME} ${FFMPEG_LIBSWSCALE} ${FFMPEG_LIBSWSCALE} -${FFMPEG_SWSCALE_VERSION_MAJOR} )
		copy_dll_ex( ${PROJECT_NAME} ${PROJECT_NAME} ${FFMPEG_LIBSWRESAMPLE} ${FFMPEG_LIBSWRESAMPLE} -${FFMPEG_SWRESAMPLE_VERSION_MAJOR} )
	endif ()
endif ()
