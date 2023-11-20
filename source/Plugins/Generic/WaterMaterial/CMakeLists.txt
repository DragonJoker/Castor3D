set( FOLDER_NAME WaterMaterial )
project( ${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Water rendering for Castor3D" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterComponent.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterFoamMapComponent.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNormal1MapComponent.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNormal2MapComponent.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNoiseMapComponent.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterReflRefrComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterFoamMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNormal1MapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNormal2MapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterNoiseMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterMaterialPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WaterReflRefrComponent.cpp
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${PROJECT_NAME}/Shaders/GlslWaterProfile.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${PROJECT_NAME}/Shaders/GlslWaterProfile.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

add_target_min(
	${PROJECT_NAME}
	api_plugin
	""
	""
)
target_sources( ${PROJECT_NAME} 
	PRIVATE
		${CASTOR_EDITORCONFIG_FILE}
)
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
endif ()
add_library( castor::${FOLDER_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${GenericIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SOVERSION}
		PREFIX ${C3D_LIB_PREFIX}
		FOLDER "Plugins/Generic"
)
c3d_release_pdbs( ${PROJECT_NAME} )
install_target_ex( ${PROJECT_NAME}
	Castor3D
	Plugins
	api_plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}
)
set( CASTOR_INSTEXP_PLUGINS
	${CASTOR_INSTEXP_PLUGINS}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
