project( CastorMeshConverter )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D mesh file converter." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	bin_dos
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/tools/${PROJECT_NAME}
	""
	""
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

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
