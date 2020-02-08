project( CastorUtils )

include( Doxygen )
include( AndroidNdkModules )

if ( CASTOR_USE_SYSTEM_LIBS OR APPLE )
	if ( VCPKG_TOOLCHAIN )
		find_package( freetype CONFIG )
		set( Freetype_LIBRARIES freetype )
		set( FreeType_FOUND ( freetype_FOUND OR FREETYPE_FOUND ) )
	else ()
		find_package( Freetype )
		set( Freetype_LIBRARIES Freetype::Freetype )
	endif ()
else ()
	set( FreeType_FOUND ON )
	set( Freetype_LIBRARIES Freetype::Freetype )
endif ()
if ( FreeType_FOUND OR FREETYPE_FOUND )
	set( FreeType_FOUND FREETYPE_FOUND )
endif ()
if ( CASTOR_USE_SYSTEM_LIBS )
	find_package( ZLIB 1.2.7 )
else ()
	set( ZLIB_FOUND ON )
endif ()

if ( ZLIB_FOUND AND FreeType_FOUND )
	if ( ANDROID )
		include_directories( ${ANDROID_NDK}/sources )
		android_ndk_import_module_cpufeatures()
		set( PLATFORM_LIBRARIES
			m
			EGL
			cpufeatures
		)
	elseif( APPLE )
		set( PLATFORM_LIBRARIES
			"-framework CoreGraphics"
		)
	elseif( WIN32 )
	else ()
		find_package( X11 )

		set( CASTOR_HAS_XINERAMA 0 )
		if ( EXISTS "/usr/lib/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
				Xinerama
			)
			set( ${PROJECT_NAME}_DEFINITIONS
				${${PROJECT_NAME}_DEFINITIONS}
				CU_HasXinerama
			)
		elseif ( EXISTS "/usr/include/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
				Xinerama
			)
			set( ${PROJECT_NAME}_DEFINITIONS
				${${PROJECT_NAME}_DEFINITIONS}
				CU_HasXinerama
			)
		endif ()

		set( PLATFORM_LIBRARIES
			${PLATFORM_LIBRARIES}
			${X11_LIBRARIES}
		)
	endif ()

	option( CASTOR_USE_TRACK "Enable function tracking" FALSE )

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
	set( ${PROJECT_NAME}_BRIEF "Multiplatform utilities library" )
	set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/../data/Images/castor-dark-small.png" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 11 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	if( PROJECTS_USE_PRECOMPILED_HEADERS )
		set( CASTOR_USE_PCH 1 )
	else()
		set( CASTOR_USE_PCH 0 )
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

	set( ADDITIONAL_SOURCE_FILES
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
	)

	if ( APPLE )
		set( ADDITIONAL_SOURCE_FILES
			${ADDITIONAL_SOURCE_FILES}
			Platform/MacOS/DisplaySize.mm
		)
	endif ()

	add_target(
		${PROJECT_NAME}
		api_dll
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}
		""
		""
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
		""
		""
		""
		"${ADDITIONAL_SOURCE_FILES}"
	)
	add_library( castor::${PROJECT_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			_UNICODE
			UNICODE
		PRIVATE
			${${PROJECT_NAME}_DEFINITIONS}
	)
	target_include_directories( ${PROJECT_NAME}
		PUBLIC
			${CASTOR_SOURCE_DIR}/include
			${CASTOR_BINARY_DIR}/include
		PRIVATE
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Core
			${CASTOR_BINARY_DIR}/source/Core
	)

	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				Dbghelp
			PRIVATE
				${PLATFORM_LIBRARIES}
				${Freetype_LIBRARIES}
				ZLIB::ZLIB
		)
	else ()
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				${CMAKE_DL_LIBRARY}
			PRIVATE
				${PLATFORM_LIBRARIES}
				${Freetype_LIBRARIES}
				ZLIB::ZLIB
		)
	endif ()

	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Align "Align" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Config "Config" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Data "Data" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Data/MiniZip "Data\\\\\\\\MiniZip" )
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
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Win32 "Platform\\\\\\\\Win32" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Android "Platform\\\\\\\\Android" )
	c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Platform/Linux "Platform\\\\\\\\Linux" )

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
	if( NOT FreeType_FOUND )
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
