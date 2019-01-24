project( CastorGui )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D GUI creation" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${Castor3DIncludeDirs} )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${CastorMinLibraries}"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/CastorGuiPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/CastorGuiPch.cpp
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Generic" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
