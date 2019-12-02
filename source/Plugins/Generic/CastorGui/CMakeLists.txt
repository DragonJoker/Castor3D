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

include_directories( ${GenericIncludes} )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Generic/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${PROJECT_NAME}
	"Castor3D"
	"${GenericLibraries}"
	${CASTOR_SOURCE_DIR}/include/Plugins/Generic/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Generic" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
