project( ImgConverter )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Converter from image files to ICO/XPM files" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
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
	${CASTOR_SOURCE_DIR}/tools
	${CASTOR_BINARY_DIR}/tools
	${wxWidgets_INCLUDE_DIRS}
	${GTK2_INCLUDE_DIRS}
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	${wxWidgets_LIBRARIES}
	${GTK2_LIBRARIES}
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${wxWidgets_DEFINITIONS}
)
# Copying internationalisation files (french, for now) for ImgConverter
copy_languages( ${PROJECT_NAME} ${CASTOR_SOURCE_DIR}/internationalisation/${PROJECT_NAME} ${PROJECT_NAME} "fr" )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
