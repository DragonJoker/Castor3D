project( CastorMeshUpgrader )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D mesh file upgrader." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/CastorMeshUpgrader.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}/CastorMeshUpgrader.cpp
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
	bin_dos
	""
	""
)
target_include_directories( ${PROJECT_NAME} PRIVATE
	${Castor3DIncludeDirs}
	${CASTOR_SOURCE_DIR}/tools
	${CASTOR_BINARY_DIR}/tools
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	castor::Castor3D
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${Castor3DDefinitions}
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Tools"
)
install_target( ${PROJECT_NAME}
	bin_dos
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
