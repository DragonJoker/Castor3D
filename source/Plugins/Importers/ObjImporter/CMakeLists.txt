set( FOLDER_NAME ObjImporter )
project( castor3d${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Wavefront Object Importer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 5 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/ObjGroup.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/ObjImporter.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/ObjImporterPrerequisites.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/ObjImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/ObjImporterPlugin.cpp
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
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/${FOLDER_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/${FOLDER_NAME}Pch.cpp
)
add_library( castor::${FOLDER_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${ImporterIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
install_target( ${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Plugins/Importers"
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
