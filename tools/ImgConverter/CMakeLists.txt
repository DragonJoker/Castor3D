project( ImgConverter )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Converter from image files to ICO/XPM files" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/ImgConverter.hpp
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/MainFrame.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/ImgConverter.cpp
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/MainFrame.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
add_target_min(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/PrecompiledHeader.hpp
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/PrecompiledHeader.cpp
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
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Tools"
)
install_target( ${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
)
# Copying internationalisation files (french, for now) for ImgConverter
copy_languages( ${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/internationalisation/${PROJECT_NAME}
	${PROJECT_NAME}
	"fr"
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
