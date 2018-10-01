project( CastorUtils )

include( Doxygen )
include( AndroidNdkModules )

include_directories(
	${CMAKE_SOURCE_DIR}/external/zlib
	${CMAKE_BINARY_DIR}/external/zlib
	${CMAKE_SOURCE_DIR}/external/FreeType/include
	${CMAKE_SOURCE_DIR}/external/FreeType/include/freetype
	${CMAKE_SOURCE_DIR}/external/FreeImage/Source
	${CMAKE_BINARY_DIR}/external/FreeImage/Source
)

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

set( CastorMinLibraries
	${CastorMinLibraries}
	zlib
	FreeImage
	FreeType
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
	set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
	set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
	include( InstallRequiredSystemLibraries )
endif ()

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

