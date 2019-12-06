project( LoopDivider )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Loop Tessellation Divider" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Dividers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Dividers/${PROJECT_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/include/Plugins/Dividers/${PROJECT_NAME}/LoopPch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Dividers/${PROJECT_NAME}/LoopPch.cpp
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${DividerIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Dividers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
