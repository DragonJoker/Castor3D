option( CASTOR_USE_SSE2 "Use SSE2 instructions for Point4f and Matrix4x4f operations" TRUE)

project( CastorUtils )

include( Doxygen )
include( AndroidNdkModules )

if ( WIN32 )
	find_package( ZLIB 1.2.7 )
	set( FREETYPE_LIBRARIES FreeType )
	set( FreeImage_LIBRARIES FreeImage )
	set( FreeImage_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/FreeImage/Source )
elseif ( NOT ANDROID )
	find_package( ZLIB )
	find_package( FreeImage )
	find_package( Freetype )
	if ( FREETYPE_FOUND )
		message( STATUS "+ Found Freetype (version ${FREETYPE_VERSION_STRING})" )
	endif()
	if ( FREEIMAGE_FOUND )
		message( STATUS "+ Found FreeImage (version ${FreeImage_VERSION_STRING})" )
	endif()
else ()
	set( FREETYPE_LIBRARIES FreeType )
	set( FreeImage_LIBRARIES FreeImage )
endif ()

if( NOT WIN32 )
	find_package( X11 )
endif()

if( ZLIB_FOUND )
	message( STATUS "+ Found Zlib (version ${ZLIB_VERSION_STRING})" )
endif()

if( ZLIB_FOUND )
	if ( ANDROID )
		include_directories( ${ANDROID_NDK}/sources )
		android_ndk_import_module_cpufeatures()
		set( CastorMinLibraries
			${CastorMinLibraries}
			m
			EGL
			cpufeatures
		)
	else ()
		include_directories( ${FreeImage_INCLUDE_DIR} )
	endif ()

	include_directories( ${ZLIB_INCLUDE_DIRS} )
  
	set( CASTOR_HAS_XINERAMA 0 )
	if ( EXISTS "/usr/lib/X11/extensions/Xinerama.h" )
		set( CASTOR_HAS_XINERAMA 1 )
	elseif ( EXISTS "/usr/include/X11/extensions/Xinerama.h" )
		set( CASTOR_HAS_XINERAMA 1 )
	endif ()

	option( CASTOR_USE_DOUBLE "Use double precision floats for Castor::real type" FALSE )
	option( CASTOR_USE_TRACK "Enable function tracking" FALSE )

	#FreeImage Libs
	set( FreeImageLibraries "")
	foreach(Lib ${FreeImage_LIBRARIES})
		if (FreeImageLibraries)
			set(FreeImageLibraries "${FreeImageLibraries}|${Lib}")
		else()
			set(FreeImageLibraries "${Lib}")
		endif()
	endforeach()

	#FreeType Libs
	set( FreeTypeLibraries "")
	foreach(Lib ${FREETYPE_LIBRARIES})
		if (FreeTypeLibraries)
			set(FreeTypeLibraries "${FreeTypeLibraries}|${Lib}")
		else()
			set(FreeTypeLibraries "${Lib}")
		endif()
	endforeach()

	#Zlib Libs
	set( ZlibLibraries "")
	foreach(Lib ${ZLIB_LIBRARIES})
		if (ZlibLibraries)
			set(ZlibLibraries "${ZlibLibraries}|${Lib}")
		else()
			set(ZlibLibraries "${Lib}")
		endif()
	endforeach()

	#X11 Libs
	set( X11Libraries "" )
	foreach( Lib ${X11_LIBRARIES} )
		if( X11Libraries )
			set( X11Libraries "${X11Libraries}|${Lib}" )
		elseif (${CASTOR_HAS_XINERAMA} EQUAL 1)
			set( X11Libraries "Xinerama ${Lib}" )
		else ()
			set( X11Libraries "${Lib}" )
		endif()
	endforeach()

	set( CastorBinsDependencies 
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( CastorMinLibraries
		${CastorMinLibraries}
		${FreeImageLibraries}
		${FreeTypeLibraries}
		${ZlibLibraries}
		${X11Libraries}
	)

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 11 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	if( PROJECTS_USE_PRECOMPILED_HEADERS )
		set( CASTOR_USE_PCH 1 )
	else()
		set( CASTOR_USE_PCH 0 )
	endif()
	if( CASTOR_USE_DOUBLE )
		set( CASTOR_USE_DOUBLE 1 )
	else()
		set( CASTOR_USE_DOUBLE 0 )
	endif()
	if( CASTOR_USE_TRACK )
		set( CASTOR_USE_TRACK 1 )
	else()
		set( CASTOR_USE_TRACK 0 )
	endif()
	if( CASTOR_USE_SSE2 )
		set( CASTOR_USE_SSE2 1 )
	else()
		set( CASTOR_USE_SSE2 0 )
	endif()

	configure_file( 
		${CMAKE_CURRENT_SOURCE_DIR}/Src/config.hpp.in
		${CMAKE_CURRENT_BINARY_DIR}/Src/config.hpp
		@ONLY
		NEWLINE_STYLE LF
	)

	include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
	include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )

	add_target(
		${PROJECT_NAME}
		api_dll
		""
		"${CastorMinLibraries};${X11Libraries}"
		"CastorUtilsPch.hpp"
		"CastorUtilsPch.cpp"
		"${FreeImage_FLAGS}"
		""
		""
		"${CMAKE_CURRENT_BINARY_DIR}/Src/config.hpp"
	)

	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME} Dbghelp )
	else ()
		target_link_libraries( ${PROJECT_NAME} dl )
	endif ()

	parse_subdir_files( Src/Align "Align" )
	parse_subdir_files( Src/Config "Config" )
	parse_subdir_files( Src/Data "Data" )
	parse_subdir_files( Src/Data/MiniZip "Data\\\\MiniZip" )
	parse_subdir_files( Src/Design "Design" )
	parse_subdir_files( Src/Exception "Exception" )
	parse_subdir_files( Src/FileParser "FileParser" )
	parse_subdir_files( Src/Graphics "Graphics" )
	parse_subdir_files( Src/Log "Log" )
	parse_subdir_files( Src/Math "Math" )
	parse_subdir_files( Src/Miscellaneous "Miscellaneous" )
	parse_subdir_files( Src/Multithreading "Multithreading" )
	parse_subdir_files( Src/Pool "Pool" )
	parse_subdir_files( Src/Stream "Stream" )
	parse_subdir_files( Src/Platform/Win32 "Platform\\\\Win32" )
	parse_subdir_files( Src/Platform/Android "Platform\\\\Android" )
	parse_subdir_files( Src/Platform/Linux "Platform\\\\Linux" )

	file( GLOB CASTOR_Config_HEADER_FILES
		${CMAKE_CURRENT_BINARY_DIR}/Src/*config*.hpp
		${CMAKE_CURRENT_BINARY_DIR}/Src/*config*.inl
	)
	file( GLOB CASTOR_Config_SOURCE_FILES
		${CMAKE_CURRENT_BINARY_DIR}/Src/*config*.cpp
	)
	source_group( "Header Files\\Config" FILES ${CASTOR_Config_HEADER_FILES} )
	source_group( "Source Files\\Config" FILES ${CASTOR_Config_SOURCE_FILES} )

	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/Src/config.hpp
		DESTINATION include/${PROJECT_NAME}
		COMPONENT ${PROJECT_NAME}_dev
	)

	if ( WIN32 )
		include( InstallRequiredSystemLibraries )
		if ( ${PROJECTS_PLATFORM} STREQUAL "x86" )
			get_filename_component( ZLibLibPath ${ZLIB_LIBRARY} PATH )
			set( ZLibLibPath ${ZLibLibPath}/zlib1.dll )
			copy_dll( ${PROJECT_NAME} ${ZLibLibPath} ${ZLibLibPath} )
		elseif ( ZLIB_LIBRARY_RELEASE OR ZLIB_LIBRARY_DEBUG )
			if ( ZLIB_LIBRARY_RELEASE AND ZLIB_LIBRARY_DEBUG )
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_DEBUG} ${ZLIB_LIBRARY_RELEASE} )
			elseif ( ZLIB_LIBRARY_RELEASE )
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE} ${ZLIB_LIBRARY_RELEASE} )
			else ()
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_DEBUG} ${ZLIB_LIBRARY_DEBUG} )
			endif ()
		else ()
			copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY} ${ZLIB_LIBRARY} )
		endif ()
		copy_dll( ${PROJECT_NAME} ${FreeImage_LIBRARIES} ${FreeImage_LIBRARIES} )
	endif ()

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
	target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
else ()
	set( msg "no (missing :" )
	if( NOT ZLIB_FOUND )
		set( msg "${msg} Zlib" )
	endif()
	set( Build "${msg})" PARENT_SCOPE )
	set( Error TRUE PARENT_SCOPE )
endif ()

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

