option( CASTOR_USE_UNICODE "Use UNICODE strings (not recommended)" FALSE)

project( CastorUtils )

include( Doxygen )

find_package( Freetype )
find_package( ZLIB 1.2.7 )
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
	option( CASTOR_USE_LOG_TRACK "Enable function tracking to be written in log file" FALSE )
	
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
	set( ${PROJECT_NAME}_VERSION_MINOR	8 )
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
	if( CASTOR_USE_LOG_TRACK )
		set( CASTOR_USE_LOG_TRACK 1 )
	else()
		set( CASTOR_USE_LOG_TRACK 0 )
	endif()
	if( CASTOR_USE_UNICODE )
		set( CASTOR_USE_UNICODE 1 )
	else()
		set( CASTOR_USE_UNICODE 0 )
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
	)
	
	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME} Dbghelp )
	else ()
		target_link_libraries( ${PROJECT_NAME} dl )
	endif ()
	
	file( GLOB CASTOR_Math_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Angle*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Angle*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Coords*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Coords*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Line*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Line*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Matrix*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Matrix*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Point*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Point*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Quaternion*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Quaternion*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Math*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Math*.inl
	)
	file( GLOB CASTOR_Math_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Angle*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Coords*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Line*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Matrix*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Point*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Quaternion*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Math*.cpp
	)
	source_group( "Header Files\\Math" FILES ${CASTOR_Math_HEADER_FILES} )
	source_group( "Source Files\\Math" FILES ${CASTOR_Math_SOURCE_FILES} )

	file( GLOB CASTOR_Design_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Collection*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Collection*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Factory*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Factory*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Named*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Named*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*NonCopyable*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*NonCopyable*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Unique*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Unique*.inl
	)
	file( GLOB CASTOR_Design_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Collection*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Factory*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Named*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*NonCopyable*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Unique*.cpp
	)
	source_group( "Header Files\\Design" FILES ${CASTOR_Design_HEADER_FILES} )
	source_group( "Source Files\\Design" FILES ${CASTOR_Design_SOURCE_FILES} )

	file( GLOB CASTOR_Exception_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Exception*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Exception*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Assertion*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Assertion*.inl
	)
	file( GLOB CASTOR_Exception_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Exception*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Assertion*.cpp
	)
	source_group( "Header Files\\Exception" FILES ${CASTOR_Exception_HEADER_FILES} )
	source_group( "Source Files\\Exception" FILES ${CASTOR_Exception_SOURCE_FILES} )

	file( GLOB CASTOR_Graphics_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Colour*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Colour*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Image*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Image*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Size*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Size*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Position*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Position*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Rectangle*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Rectangle*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Box*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Box*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Glyph*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Glyph*.inl
	)
	file( GLOB CASTOR_Graphics_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Colour*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Image*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Size*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Position*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Rectangle*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Box*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Glyph*.cpp
	)
	source_group( "Header Files\\Graphics" FILES ${CASTOR_Graphics_HEADER_FILES} )
	source_group( "Source Files\\Graphics" FILES ${CASTOR_Graphics_SOURCE_FILES} )

	file( GLOB CASTOR_Data_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*File*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*File*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parser*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parser*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Path*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Path*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Archive*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Archive*.inl
	)
	file( GLOB CASTOR_Data_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*File*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parser*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Path*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Archive*.cpp
	)
	source_group( "Header Files\\Data" FILES ${CASTOR_Data_HEADER_FILES} )
	source_group( "Source Files\\Data" FILES ${CASTOR_Data_SOURCE_FILES} )

	file( GLOB CASTOR_Stream_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stream*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stream*.inl
	)
	file( GLOB CASTOR_Stream_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stream*.cpp
	)
	source_group( "Header Files\\Stream" FILES ${CASTOR_Stream_HEADER_FILES} )
	source_group( "Source Files\\Stream" FILES ${CASTOR_Stream_SOURCE_FILES} )

	file( GLOB CASTOR_Miscellaneous_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Block*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Block*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Console*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Console*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*DynamicLibrary*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*DynamicLibrary*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Logger*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Logger*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*PreciseTimer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*PreciseTimer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Resource*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Resource*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*String*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*String*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Templates*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Templates*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Utils.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Utils.inl
	)
	file( GLOB CASTOR_Miscellaneous_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Block*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Console*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*DynamicLibrary*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Logger*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*PreciseTimer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Resource*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*String*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Templates*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Utils.cpp
	)
	source_group( "Header Files\\Miscellaneous" FILES ${CASTOR_Miscellaneous_HEADER_FILES} )
	source_group( "Source Files\\Miscellaneous" FILES ${CASTOR_Miscellaneous_SOURCE_FILES} )

	file( GLOB CASTOR_Config_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*config*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*config*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Config*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Config*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Macros*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Macros*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*SmartPtr*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*SmartPtr*.inl
	)
	file( GLOB CASTOR_Config_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*config*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Config*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Macros*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*SmartPtr*.cpp
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
		get_filename_component( ZlibPath ${ZLIB_LIBRARY} PATH)
		get_filename_component( ZlibPath ${ZlibPath} PATH)
		get_filename_component( LibZipPath ${LIBZIP_LIBRARY} PATH)
		get_filename_component( LibZipPath ${LibZipPath} PATH)
		install(
			FILES
				${FreeImage_LIBRARY_DIR}/FreeImage.dll
				${LibZipPath}/bin/zip.dll
			DESTINATION bin
			COMPONENT ${PROJECT_NAME}
		)
		if ( EXISTS ${FreeTypePath}/bin/freetype6.dll )
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND
					${CMAKE_COMMAND} -E
						copy_if_different
						${FreeTypePath}/bin/freetype6.dll
						${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
			)
			install(
				FILES ${FreeTypePath}/bin/freetype6.dll
				DESTINATION bin
				COMPONENT ${PROJECT_NAME}
			)
		endif ()
		if ( EXISTS ${ZlibPath}/bin/zlib1.dll )
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND
					${CMAKE_COMMAND} -E
						copy_if_different
						${ZlibPath}/bin/zlib1.dll
						${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
			)
			install(
				FILES ${FreeTypePath}/bin/zlib1.dll
				DESTINATION bin
				COMPONENT ${PROJECT_NAME}
			)
		elseif ( EXISTS ${ZlibPath}/bin/zlibwapi.dll )
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND
					${CMAKE_COMMAND} -E
						copy_if_different
						${ZlibPath}/bin/zlibwapi.dll
						${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
			)
			install(
				FILES ${ZlibPath}/bin/zlibwapi.dll
				DESTINATION bin
				COMPONENT ${PROJECT_NAME}
			)
		endif ()
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND
				${CMAKE_COMMAND} -E
					copy_if_different
					${FreeImage_LIBRARY_DIR}/FreeImage.dll
					${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
		)
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND
				${CMAKE_COMMAND} -E
					copy_if_different
					${LibZipPath}/bin/zip.dll
					${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
		)
	endif ()
	
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core/CastorUtils" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	add_target_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
	add_target_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )
	
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

if( ${CASTOR_BUILD_TEST} )
	add_subdirectory( Test )
endif()
