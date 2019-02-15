project( CastorUtils )

include( Doxygen )
include( AndroidNdkModules )

if ( CASTOR_USE_SYSTEM_LIBS )
	find_package( ZLIB 1.2.7 )
	find_package( Freetype )
	set( msg "no (missing: " )
	if ( FREETYPE_FOUND )
		message( STATUS "+ Found Freetype (version ${FREETYPE_VERSION_STRING})" )
	else ()
		set( msg "${msg} Freetype" )
	endif()
	if( ZLIB_FOUND )
		message( STATUS "+ Found zlib (version ${ZLIB_VERSION_STRING})" )
	else ()
		set( msg "${msg} zlib" )
	endif()

	include_directories( ${ZLIB_INCLUDE_DIR} )
	include_directories( ${FREETYPE_INCLUDE_DIR_freetype2} )
	include_directories( ${FREETYPE_INCLUDE_DIR_ft2build} )

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
else ()
	set( FREETYPE_FOUND TRUE )
	set( ZLIB_FOUND TRUE )
	include_directories(
		${CMAKE_SOURCE_DIR}/external/zlib
		${CMAKE_BINARY_DIR}/external/zlib
		${CMAKE_SOURCE_DIR}/external/FreeType/include
		${CMAKE_SOURCE_DIR}/external/FreeType/include/freetype
	)
endif ()

if ( ZLIB_FOUND AND FREETYPE_FOUND )
	if( NOT WIN32 )
		find_package( X11 )
	endif()

	if ( ANDROID )
		include_directories( ${ANDROID_NDK}/sources )
		android_ndk_import_module_cpufeatures()
		set( CastorMinLibraries
			${CastorMinLibraries}
			m
			EGL
			cpufeatures
		)
	endif ()

	set( CASTOR_HAS_XINERAMA 0 )
	if ( EXISTS "/usr/lib/X11/extensions/Xinerama.h" )
		set( CASTOR_HAS_XINERAMA 1 )
	elseif ( EXISTS "/usr/include/X11/extensions/Xinerama.h" )
		set( CASTOR_HAS_XINERAMA 1 )
	endif ()

	option( CASTOR_USE_DOUBLE "Use double precision floats for Castor::real type" FALSE )
	option( CASTOR_USE_TRACK "Enable function tracking" FALSE )

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

	if ( CASTOR_USE_SYSTEM_LIBS )
		set( CastorMinLibraries
			${CastorMinLibraries}
			${FreeTypeLibraries}
			${ZlibLibraries}
			${X11Libraries}
		)
	else ()
		set( CastorMinLibraries
			${CastorMinLibraries}
			zlib
			FreeType
			${X11Libraries}
		)
	endif ()

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
	set( ${PROJECT_NAME}_BRIEF "Multiplatform utilities library" )
	set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/../data/Images/castor-dark-small.png" )
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

	configure_file( 
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/config.hpp.in
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		@ONLY
		NEWLINE_STYLE LF
	)

	include_directories( ${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME} )

	add_target(
		${PROJECT_NAME}
		api_dll
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}
		""
		"${CastorMinLibraries};${X11Libraries}"
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
		""
		""
		""
		"${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp"
	)

	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME} Dbghelp )
	else ()
		target_link_libraries( ${PROJECT_NAME} dl )
	endif ()

	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Align "Align" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Config "Config" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Data "Data" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Data/MiniZip "Data\\\\MiniZip" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Design "Design" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Exception "Exception" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core FileParser "FileParser" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Graphics "Graphics" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Log "Log" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Math "Math" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Miscellaneous "Miscellaneous" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Multithreading "Multithreading" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Pool "Pool" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Stream "Stream" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Win32 "Platform\\\\Win32" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Android "Platform\\\\Android" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Linux "Platform\\\\Linux" )

	file( GLOB CASTOR_Config_HEADER_FILES
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/*config*.hpp
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/*config*.inl
	)
	file( GLOB CASTOR_Config_SOURCE_FILES
		${CASTOR_BINARY_DIR}/source/Core/${PROJECT_NAME}/*config*.cpp
	)
	source_group( "Header Files\\Config" FILES ${CASTOR_Config_HEADER_FILES} )
	source_group( "Source Files\\Config" FILES ${CASTOR_Config_SOURCE_FILES} )

	install(
		FILES ${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		DESTINATION include/${PROJECT_NAME}
		COMPONENT ${PROJECT_NAME}_dev
	)

	if ( WIN32 )
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
		include( InstallRequiredSystemLibraries )
		if ( CASTOR_USE_SYSTEM_LIBS )
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
		endif ()
	endif ()

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
	target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
else ()
	set( msg "no (missing :" )
	if( NOT FREETYPE_FOUND )
		set( msg "${msg} Freetype" )
	endif()
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
