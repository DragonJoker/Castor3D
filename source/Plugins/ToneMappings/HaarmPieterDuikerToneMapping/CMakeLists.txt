set( FOLDER_NAME HaarmPieterDuikerToneMapping )
project( castor3d${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Haarm Pieter Duiker Tone Mapping" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings/${FOLDER_NAME}/HaarmPieterDuikerToneMapping.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings/${FOLDER_NAME}/HaarmPieterDuikerToneMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings/${FOLDER_NAME}/HaarmPieterDuikerToneMappingPlugin.cpp
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
	""
	""
)
add_library( castor::${FOLDER_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${ToneMappingIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Plugins/ToneMappings"
)
install_target( ${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/ToneMappings/${FOLDER_NAME}
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
