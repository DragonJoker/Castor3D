project( CastorTestLauncher )

include_directories(
	${CASTOR_SOURCE_DIR}/source/Samples/GuiCommon/xpms
	${CASTOR_SOURCE_DIR}/include/Tools
	${CASTOR_BINARY_DIR}/include/Tools
)

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Test Scene Launcher for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${Castor3DIncludeDirs} )

add_target(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/include/Tools/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Tools/${PROJECT_NAME}
	"${Castor3DBinsDependencies}"
	"${CastorMinLibraries};${wxWidgetsLibraries}"
	""
	""
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
