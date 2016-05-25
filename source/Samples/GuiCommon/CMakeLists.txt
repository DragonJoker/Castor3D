project( GuiCommon )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorMinLibraries}
	${wxWidgetsLibraries}
	${FFMpegLibraries}
	${OpenCVLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Common GUI tools" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	9 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	lib
	"Castor3D"
	"${CastorBinLibraries}"
	"GuiCommonPch.hpp"
	"GuiCommonPch.cpp"
	"${wxWidgets_CFLAGS} ${GUICOMMON_C_FLAGS}"
	"${wxWidgets_CXXFLAGS} ${GUICOMMON_CXX_FLAGS}"
)

file( GLOB ${PROJECT_NAME}_TREE_ITEMS_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*TreeItem*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*TreeItem*.inl
)
file( GLOB ${PROJECT_NAME}_TREE_ITEMS_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*TreeItem*.cpp
)
source_group( "Header Files\\Properties\\Tree Items" FILES ${${PROJECT_NAME}_TREE_ITEMS_HEADER_FILES} )
source_group( "Source Files\\Properties\\Tree Items" FILES ${${PROJECT_NAME}_TREE_ITEMS_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_PROPERTIES_MATH_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.inl
)
file( GLOB ${PROJECT_NAME}_PROPERTIES_MATH_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.cpp
)
source_group( "Header Files\\Properties\\Math" FILES ${${PROJECT_NAME}_PROPERTIES_MATH_HEADER_FILES} )
source_group( "Source Files\\Properties\\Math" FILES ${${PROJECT_NAME}_PROPERTIES_MATH_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SHADER_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
)
file( GLOB ${PROJECT_NAME}_SHADER_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
)
source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_HEADER_FILES} )
source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SYSTEM_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Application*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Application*.inl
)
file( GLOB ${PROJECT_NAME}_SYSTEM_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Application*.cpp
)
source_group( "Header Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_HEADER_FILES} )
source_group( "Source Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_AUI_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Aui*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Aui*.inl
)
file( GLOB ${PROJECT_NAME}_AUI_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Aui*.cpp
)
source_group( "Header Files\\Aui" FILES ${${PROJECT_NAME}_AUI_HEADER_FILES} )
source_group( "Source Files\\Aui" FILES ${${PROJECT_NAME}_AUI_SOURCE_FILES} )

# Copying language files for GLSL and HLSL coloration in ShaderDialog
file(
	GLOB_RECURSE
		Parser_languages tools/resource/${PROJECT_NAME}/languages/*.lang
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

if ( {${CASTOR_PACKAGE_DEB} )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "wxWidgets >= ${wxWidgetsVersion}" )
endif ()

if ( WIN32 )
	if( FFMPEG_FOUND )
		# Preparing FFMPEG dlls installation
		message( STATUS ${FFMPEG_LIBAVCODEC_VERSION_STRING} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVCODEC} Debug -${FFMPEG_AVCODEC_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVCODEC} Release -${FFMPEG_AVCODEC_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVFORMAT} Debug -${FFMPEG_AVFORMAT_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVFORMAT} Release -${FFMPEG_AVFORMAT_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVUTIL} Debug -${FFMPEG_AVUTIL_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBAVUTIL} Release -${FFMPEG_AVUTIL_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWRESAMPLE} Debug -${FFMPEG_SWRESAMPLE_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWRESAMPLE} Release -${FFMPEG_SWRESAMPLE_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWSCALE} Debug -${FFMPEG_SWSCALE_VERSION_MAJOR} )
		copy_dll( ${PROJECT_NAME} ${FFMPEG_LIBSWSCALE} Release -${FFMPEG_SWSCALE_VERSION_MAJOR} )
	elseif( OPENCV_FOUND OR OpenCV_FOUND )
		# Preparing OpenCV dlls installation
		set( OpenCV_DLLS )
		set( OpenCVd_DLLS )
		get_filename_component( OpenCvLibPath "${OpenCV_LIB_DIR_OPT}" PATH )
		string( REPLACE "." "" Version "${OpenCV_VERSION}" )
		foreach ( LIB ${OpenCV_LIBS} )
			get_filename_component( OpenCvLibName ${LIB} NAME_WE )
			set( OpenCvLibPathName ${OpenCvLibPath}/bin/${OpenCvLibName}${Version}.dll )
			if ( EXISTS ${OpenCvLibPathName} )
				add_custom_command(
					TARGET ${PROJECT_NAME}
					POST_BUILD
					COMMAND if 1==$<CONFIG:Release>
						${CMAKE_COMMAND} -E
							copy_if_different
							${OpenCvLibPathName}
							${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
				)
				set( OpenCV_DLLS
					${OpenCV_DLLS}
					${OpenCvLibPathName}
				)
			else ()
				set( OpenCvLibPathName ${OpenCV_CONFIG_PATH}/bin${OpenCV_LIB_DIR_OPT}/${OpenCvLibName}${Version}.dll )
				if ( EXISTS ${OpenCvLibPathName} )
					add_custom_command(
						TARGET ${PROJECT_NAME}
						POST_BUILD
						COMMAND if 1==$<CONFIG:Release>
							${CMAKE_COMMAND} -E
								copy_if_different
								${OpenCvLibPathName}
								${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
					)
					set( OpenCV_DLLS
						${OpenCV_DLLS}
						${OpenCvLibPathName}
					)
				else ()
					if ( ${OpenCvLibName} STREQUAL "debug" )
						set( OpenCV_CONFIG "Debug" )
					elseif ( ${OpenCvLibName} STREQUAL "optimized" )
						set( OpenCV_CONFIG "Release" )
					elseif ( ${OpenCV_CONFIG} STREQUAL "Debug" )
						set( OpenCvLibPathName ${OpenCvLibPath}/bin/Debug/${OpenCvLibName}.dll )
						if ( EXISTS ${OpenCvLibPathName} )
							add_custom_command(
								TARGET ${PROJECT_NAME}
								POST_BUILD
								COMMAND if 1==$<CONFIG:Debug>
									${CMAKE_COMMAND} -E
										copy_if_different
										${OpenCvLibPathName}
										${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
							)
							set( OpenCVd_DLLS
								${OpenCVd_DLLS}
								${OpenCvLibPathName}
							)
						endif ()
					elseif ( ${OpenCV_CONFIG} STREQUAL "Release" )
						set( OpenCvLibPathName ${OpenCvLibPath}/bin/Release/${OpenCvLibName}.dll )
						if ( EXISTS ${OpenCvLibPathName} )
							add_custom_command(
								TARGET ${PROJECT_NAME}
								POST_BUILD
								COMMAND if 1==$<CONFIG:Release>
									${CMAKE_COMMAND} -E
										copy_if_different
										${OpenCvLibPathName}
										${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
							)
							set( OpenCV_DLLS
								${OpenCV_DLLS}
								${OpenCvLibPathName}
							)
						endif ()
					endif ()
				endif ()
			endif ()
			set( OpenCvLibPathNamed ${OpenCvLibPath}/bin/${OpenCvLibName}${Version}d.dll )
			if ( EXISTS ${OpenCvLibPathNamed} )
				add_custom_command(
					TARGET ${PROJECT_NAME}
					POST_BUILD
					COMMAND if 1==$<CONFIG:Debug>
						${CMAKE_COMMAND} -E
							copy_if_different
							${OpenCvLibPathNamed}
							${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
				)
				set( OpenCVd_DLLS
					${OpenCVd_DLLS}
					${OpenCvLibPathNamed}
				)
			else ()
				set( OpenCvLibPathNamed ${OpenCV_CONFIG_PATH}/bin${OpenCV_LIB_DIR_DBG}/${OpenCvLibName}${Version}d.dll )
				if ( EXISTS ${OpenCvLibPathNamed} )
					add_custom_command(
						TARGET ${PROJECT_NAME}
						POST_BUILD
						COMMAND if 1==$<CONFIG:Debug>
							${CMAKE_COMMAND} -E
								copy_if_different
								${OpenCvLibPathNamed}
								${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
					)
					set( OpenCV_DLLS
						${OpenCV_DLLS}
						${OpenCvLibPathNamed}
					)
				endif ()
			endif ()
		endforeach ()
		install(
			FILES ${OpenCV_DLLS}
			DESTINATION bin
			CONFIGURATIONS Release
			COMPONENT ${PROJECT_NAME}
		)
		install(
			FILES ${OpenCVd_DLLS}
			DESTINATION bin
			CONFIGURATIONS Debug
			COMPONENT ${PROJECT_NAME}
		)
	endif ()
endif ()
