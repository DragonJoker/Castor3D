cmake_minimum_required( VERSION 2.8 )

set( CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_SOURCE_DIR}/CMake/Modules
)
set( CMAKE_TEMPLATES_DIR ${CMAKE_SOURCE_DIR}/CMake/Templates )

project( CastorDvpTD )

include( ExternalDependencies )
include( Project )

find_package( Castor3D )
find_package( wxWidgets COMPONENTS core base stc aui adv propgrid )

if( NOT WIN32 )
	find_package( GTK2 )
else()
	set( GTK2_FOUND TRUE )
endif()

if ( Castor3D_FOUND AND wxWidgets_FOUND AND GTK2_FOUND )
	include_directories(
		${Castor3D_INCLUDE_DIRS}
		${GTK2_INCLUDE_DIRS}
		${wxWidgets_INCLUDE_DIRS}
	)
	if ( MSVC )
		add_definitions( -D_CRT_SECURE_NO_WARNINGS )
	endif ()
	file( GLOB ${PROJECT_NAME}_SOURCE_FILES Src/*.cpp )
	file( GLOB ${PROJECT_NAME}_HEADER_FILES Src/*.hpp Src/*.inl )
	add_executable( ${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCE_FILES} ${${PROJECT_NAME}_HEADER_FILES} )
	target_link_libraries( ${PROJECT_NAME}
		${Castor3D_LIBRARIES}
		${GTK2_LIBRARIES}
		${wxWidgets_LIBRARIES}
	)
	copy_dll( ${PROJECT_NAME} ${Castor3D_GlslWriter_LIBRARY_DEBUG} ${Castor3D_GlslWriter_LIBRARY_RELEASE} )
	copy_dll( ${PROJECT_NAME} ${Castor3D_CastorUtils_LIBRARY_DEBUG} ${Castor3D_CastorUtils_LIBRARY_RELEASE} )
	copy_dll( ${PROJECT_NAME} ${Castor3D_Castor3D_LIBRARY_DEBUG} ${Castor3D_Castor3D_LIBRARY_RELEASE} )
	copy_dll( ${PROJECT_NAME} ${Castor3D_GuiCommon_LIBRARY_DEBUG} ${Castor3D_GuiCommon_LIBRARY_RELEASE} )
else ()
	if ( NOT wxWidgets_FOUND )
		set( Msg "${Msg}wxWidgets libraries not found.\n" )
	endif ()
	if ( NOT GTK2_FOUND )
		set( Msg "${Msg}GTK2 libraries not found.\n" )
	endif ()
	if ( NOT Castor3D_FOUND )
		set( Msg "${Msg}Castor3D libraries not found.\n" )
	endif ()
	message( SEND_ERROR ${Msg} )
endif ()
