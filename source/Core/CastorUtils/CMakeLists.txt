option( CASTOR_USE_SSE2 "Use SSE2 instructions for Point4f and Matrix4x4f operations" TRUE)

project( CastorUtils )

include( Doxygen )

find_package( Freetype )
if ( MSW )
	find_package( ZLIB 1.2.7 )
else ()
	find_package( ZLIB )
endif ()
find_package( FreeImage )
find_package( LibZip )

if( NOT WIN32 )
	find_package( X11 )
endif()

if( FREETYPE_FOUND )
	message( STATUS "+ Found FreeType (version ${FREETYPE_VERSION_STRING})" )
endif()

if( ZLIB_FOUND )
	message( STATUS "+ Found Zlib (version ${ZLIB_VERSION_STRING})" )
endif()

if( FREEIMAGE_FOUND )
	message( STATUS "+ Found FreeImage (version ${FreeImage_VERSION_STRING})" )
endif()

if( LIBZIP_FOUND )
	message( STATUS "+ Found libzip" )
endif()

if( FREEIMAGE_FOUND AND FREETYPE_FOUND AND ZLIB_FOUND AND LIBZIP_FOUND )
	include_directories( ${FREETYPE_INCLUDE_DIRS} )
	include_directories( ${FreeImage_INCLUDE_DIR} )
	include_directories( ${ZLIB_INCLUDE_DIRS} )
	include_directories( ${LIBZIP_INCLUDE_DIRS} )

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

	#libzip Libs
	set( LibZipLibraries "")
	foreach(Lib ${LIBZIP_LIBRARY})
		if (LibZipLibraries)
			set(LibZipLibraries "${LibZipLibraries}|${Lib}")
		else()
			set(LibZipLibraries "${Lib}")
		endif()
	endforeach()

	#X11 Libs
	set( X11Libraries "" )
	foreach( Lib ${X11_LIBRARIES} )
		if( X11Libraries )
			set( X11Libraries "${X11Libraries}|${Lib}" )
		else()
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
		${LibZipLibraries}
		${X11Libraries}
	)

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	9 )
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
		"${CastorMinLibraries};${x11Libraries}"
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

	parse_subdir_files( Align "Align" )
	parse_subdir_files( Config "Config" )
	parse_subdir_files( Data "Data" )
	parse_subdir_files( Design "Design" )
	parse_subdir_files( Exception "Exception" )
	parse_subdir_files( FileParser "FileParser" )
	parse_subdir_files( Graphics "Graphics" )
	parse_subdir_files( Log "Log" )
	parse_subdir_files( Math "Math" )
	parse_subdir_files( Miscellaneous "Miscellaneous" )
	parse_subdir_files( Pool "Pool" )
	parse_subdir_files( Stream "Stream" )

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
		# Preparing FreeImage and freetype dlls installation
		get_filename_component( FreeTypePath ${FREETYPE_LIBRARY} PATH)
		get_filename_component( FreeTypePath ${FreeTypePath} PATH)
		get_filename_component( LibZipPath ${LIBZIP_LIBRARY} PATH)
		get_filename_component( LibZipPath ${LibZipPath} PATH)

		if ( ZLIB_LIBRARY_RELEASE OR ZLIB_LIBRARY_DEBUG )
			if ( ZLIB_LIBRARY_RELEASE )
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE} Release )
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE} RelWithDebInfo )
			endif ()
			if ( ZLIB_LIBRARY_DEBUG )
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_DEBUG} Debug )
			endif ()
		else ()
			copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY} Release )
			copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY} RelWithDebInfo )
			copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY} Debug )
		endif ()

		copy_dll( ${PROJECT_NAME} ${LIBZIP_LIBRARY} Release )
		copy_dll( ${PROJECT_NAME} ${LIBZIP_LIBRARY} RelWithDebInfo )
		copy_dll( ${PROJECT_NAME} ${LIBZIP_LIBRARY} Debug )
		copy_dll( ${PROJECT_NAME} ${FREETYPE_LIBRARY} Release 6 )
		copy_dll( ${PROJECT_NAME} ${FREETYPE_LIBRARY} RelWithDebInfo 6 )
		copy_dll( ${PROJECT_NAME} ${FREETYPE_LIBRARY} Debug 6 )
		copy_dll( ${PROJECT_NAME} ${FreeImage_LIBRARIES} Release )
		copy_dll( ${PROJECT_NAME} ${FreeImage_LIBRARIES} RelWithDebInfo )
		copy_dll( ${PROJECT_NAME} ${FreeImage_LIBRARIES} Debug )
	endif ()

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
	add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
else ()
	set( msg "no (missing :" )
	if( NOT FREEIMAGE_FOUND )
		set( msg "${msg} FreeImage" )
	endif()
	if( NOT FREETYPE_FOUND )
		set( msg "${msg} Freetype" )
	endif()
	if( NOT ZLIB_FOUND )
		set( msg "${msg} Zlib" )
	endif()
	if( NOT LIBZIP_FOUND )
		set( msg "${msg} libzip" )
	endif()
	set( Build "${msg})" PARENT_SCOPE )
	set( Error TRUE PARENT_SCOPE )
endif ()

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

if( ${CASTOR_BUILDGRP_TEST} )
	add_subdirectory( Test )
endif()
