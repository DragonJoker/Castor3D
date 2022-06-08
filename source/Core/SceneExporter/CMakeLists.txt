project( SceneExporter )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} scene exporter" )
set( ${PROJECT_NAME}_BRIEF "Scene exporter for Castor3D" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-dark-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/CscnExporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/SceneExporter.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/CscnExporter.cpp
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextAnimatedObjectGroup.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBillboardList.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBorderPanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextCamera.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextColourBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextDirectionalLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextFog.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextGeometry.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextHdrConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextImageBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextLpvConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextMaterial.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextMesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextOverlayCategory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextParticleSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPointLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextProgram.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRenderTarget.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRenderWindow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRsmConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSampler.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextScene.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSceneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextShadow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSkeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSkyboxBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSpotLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSsaoConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSubsurfaceScattering.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureConfiguration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureUnit.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureView.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextViewport.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextVoxelSceneData.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextAnimatedObjectGroup.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBackground.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBillboardList.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextBorderPanelOverlay.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextCamera.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextColourBackground.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextDirectionalLight.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextFog.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextGeometry.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextHdrConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextImageBackground.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextLight.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextLpvConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextMaterial.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextMesh.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextOverlay.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextOverlayCategory.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPanelOverlay.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextParticleSystem.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPass.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextPointLight.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextProgram.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRenderTarget.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRenderWindow.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextRsmConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSampler.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextScene.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSceneNode.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextShadow.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSkeleton.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSkyboxBackground.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSpotLight.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSsaoConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextSubsurfaceScattering.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextOverlay.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureConfiguration.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureUnit.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextTextureView.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextViewport.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Text/TextVoxelSceneData.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Text" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Text" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

add_target_min(
	${PROJECT_NAME}
	api_dll
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_compile_definitions( ${PROJECT_NAME}
	PRIVATE
		${${PROJECT_NAME}_DEFINITIONS}
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CASTOR_SOURCE_DIR}/include/Core>
		$<BUILD_INTERFACE:${CASTOR_BINARY_DIR}/include/Core>
		$<INSTALL_INTERFACE:include>
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
install_target_ex( ${PROJECT_NAME}
	Castor3D
	api_dll
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		FOLDER "Core/${MAIN_PROJECT_NAME}"
		PREFIX ${C3D_LIB_PREFIX}
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
