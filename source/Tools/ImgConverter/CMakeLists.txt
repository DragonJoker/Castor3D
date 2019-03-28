project( ImgConverter )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Converter from image files to ICO/XPM files" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories(
	${CASTOR_SOURCE_DIR}/include/Tools
	${CASTOR_BINARY_DIR}/include/Tools
)

add_target(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/include/Tools/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Tools/${PROJECT_NAME}
	""
	"${wxWidgetsLibraries}"
	""
	""
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
# Copying internationalisation files (french, for now) for ImgConverter
copy_languages( ${PROJECT_NAME} ${CASTOR_SOURCE_DIR}/internationalisation/${PROJECT_NAME} ${PROJECT_NAME} "fr" )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
