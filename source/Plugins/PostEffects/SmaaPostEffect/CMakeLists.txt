set( FOLDER_NAME SmaaPostEffect )
project( castor3d${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "SMAA Post Effect" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/AreaTex.h
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SearchTex.h
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/BlendingWeightCalculation.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/ColourEdgeDetection.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DepthEdgeDetection.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/EdgeDetection.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/LumaEdgeDetection.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/NeighbourhoodBlending.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/Reproject.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/Smaa_Parsers.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaPostEffect.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaUbo.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/BlendingWeightCalculation.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/ColourEdgeDetection.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/DepthEdgeDetection.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/EdgeDetection.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/LumaEdgeDetection.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/NeighbourhoodBlending.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/Reproject.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/Smaa_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaPostEffect.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaPostEffectPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}/SmaaUbo.cpp
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
install_target( ${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects/${FOLDER_NAME}
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Plugins/PostEffects"
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
