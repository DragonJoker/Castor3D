project( SceneExporter )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} scene exporter" )
set( ${PROJECT_NAME}_BRIEF "Scene exporter for Castor3D" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-dark-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/CscnExporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/SceneExporter.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/CscnExporter.cpp
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

add_target_min(
	${PROJECT_NAME}
	api_dll
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_compile_definitions( ${PROJECT_NAME}
	PRIVATE
		${${PROJECT_NAME}_DEFINITIONS}
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CASTOR_SOURCE_DIR}/include/Core>
		$<BUILD_INTERFACE:${CASTOR_BINARY_DIR}/include/Core>
		$<INSTALL_INTERFACE:include>
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
install_target_ex( ${PROJECT_NAME}
	Castor3D
	api_dll
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		FOLDER "Core/${MAIN_PROJECT_NAME}"
		PREFIX ${C3D_LIB_PREFIX}
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)
