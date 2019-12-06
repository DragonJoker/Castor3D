project( CastorTestLauncher )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Test Scene Launcher for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
	""
	""
	""
	""
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
target_include_directories( ${PROJECT_NAME} PRIVATE
	${CASTOR_SOURCE_DIR}/samples/GuiCommon/xpms
	${Castor3DIncludeDirs}
	${CASTOR_SOURCE_DIR}/tools
	${CASTOR_BINARY_DIR}/tools
	${wxWidgets_INCLUDE_DIRS}
	${GTK2_INCLUDE_DIRS}
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	${CastorMinLibraries}
	${Castor3DBinsDependencies}
	${wxWidgets_LIBRARIES}
	${GTK2_LIBRARIES}
	castor::Castor3D
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${wxWidgets_DEFINITIONS}
	${Castor3DDefinitions}
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
