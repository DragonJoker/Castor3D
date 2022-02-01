set( FOLDER_NAME DrawEdgesPostEffect )
project( ${FOLDER_NAME} )

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Edges drawing Post Effect" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DepthNormalEdgeDetection.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdgesPostEffect.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdgesUbo.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdges_Parsers.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/ObjectIDEdgeDetection.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DepthNormalEdgeDetection.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdgesPostEffect.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdgesPostEffectPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdgesUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DrawEdges_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/ObjectIDEdgeDetection.cpp
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
		${PostEffectIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
install_target_ex( ${PROJECT_NAME}
	Plugins
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SOVERSION}
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		PREFIX ${C3D_LIB_PREFIX}
		FOLDER "Plugins/PostEffects"
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
