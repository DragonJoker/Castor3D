project( CCastor3D )

set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/Castor3D.h
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/Castor3DEnums.h
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/Castor3DPlatformTypes.h
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/Castor3DTypes.h
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3DCommon.h
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CBorderPanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CCamera.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CDirectionalLight.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CEngine.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CGeometry.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CLight.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CLightGroup.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CLineMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CMaterial.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CMesh.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/COverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CPanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CPass.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CPointLight.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CRenderTarget.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CRenderWindow.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CSampler.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CScene.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CSceneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CShadow.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CSkyboxBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CSpotLight.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CSubmesh.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CTextOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/Castor3D/CTriFaceMapping.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
source_group( "Source Files\\Castor3D" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CFont.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CGlyph.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CImage.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CLogger.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CMat4.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CPixelBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CQuat.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CVec2.cpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/CastorUtils/CVec3.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
source_group( "Source Files\\CastorUtils" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

find_rsc_file( ${PROJECT_NAME} dll )

set( PROJECTS_USE_PRECOMPILED_HEADERS OFF )
add_target_min(
	${PROJECT_NAME}
	dll
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Castor3DIncludeDirs}
		${CASTOR_SOURCE_DIR}/include/Interop
		${CASTOR_SOURCE_DIR}/source/Interop
	PUBLIC
		$<BUILD_INTERFACE:${CASTOR_SOURCE_DIR}/include/Interop>
		$<INSTALL_INTERFACE:include>
)
c3d_release_pdbs( ${PROJECT_NAME} )
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		castor::Castor3D
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Interop" )
add_target_astyle( ${PROJECT_NAME} ".hpp;.inl;.cpp" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
