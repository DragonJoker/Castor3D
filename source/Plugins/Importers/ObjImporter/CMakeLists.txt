project( ObjImporter )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Wavefront Object Importer" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${ImporterIncludes} )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Importers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${PROJECT_NAME}
	"Castor3D"
	"${ImporterLibraries}"
	${CASTOR_SOURCE_DIR}/include/Plugins/Importers/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Importers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
