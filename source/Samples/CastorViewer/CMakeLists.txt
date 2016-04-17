project( CastorViewer )

include_directories( ../GuiCommon/Src )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Scene Viewer for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	8 )
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
	"${wxWidgets_CFLAGS} ${GUICOMMON_C_FLAGS}"
	"${wxWidgets_CXXFLAGS} ${GUICOMMON_CXX_FLAGS}"
)
# Copying internationalisation files (french, for now) for CastorViewer
copy_languages( ${PROJECT_NAME} ../internationalisation ${PROJECT_NAME} "fr" )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )


if ( WIN32 )
	if( FFMPEG_FOUND )
		# Preparing FFMPEG dlls installation
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