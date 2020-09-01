project( GuiCommon )

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorMinLibraries}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Common GUI tools" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	GuiCommonPrerequisites.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	GuiCommonPrerequisites.cpp
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Aui/AuiDockArt.cpp
	Aui/AuiTabArt.cpp
	Aui/AuiToolBarArt.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	Aui/AuiDockArt.hpp
	Aui/AuiTabArt.hpp
	Aui/AuiToolBarArt.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Aui" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Aui" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Properties/AdditionalProperties.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	Properties/AdditionalProperties.hpp
	Properties/AdditionalProperties.inl
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Properties" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Properties" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Properties/Math/CubeBoxProperties.cpp
	Properties/Math/MatrixProperties.cpp
	Properties/Math/PointProperties.cpp
	Properties/Math/PositionProperties.cpp
	Properties/Math/PropertiesContainer.cpp
	Properties/Math/PropertiesHolder.cpp
	Properties/Math/QuaternionProperties.cpp
	Properties/Math/RectangleProperties.cpp
	Properties/Math/SizeProperties.cpp
	Properties/Math/SphereBoxProperties.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	Properties/Math/CubeBoxProperties.hpp
	Properties/Math/MatrixProperties.hpp
	Properties/Math/MatrixProperties.inl
	Properties/Math/PointProperties.hpp
	Properties/Math/PointProperties.inl
	Properties/Math/PositionProperties.hpp
	Properties/Math/PropertiesContainer.hpp
	Properties/Math/PropertiesHolder.hpp
	Properties/Math/QuaternionProperties.hpp
	Properties/Math/RectangleProperties.hpp
	Properties/Math/SizeProperties.hpp
	Properties/Math/SphereBoxProperties.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Properties\\Math" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Properties\\Math" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.cpp
	Properties/TreeItems/AnimatedObjectTreeItemProperty.cpp
	Properties/TreeItems/AnimationTreeItemProperty.cpp
	Properties/TreeItems/BackgroundTreeItemProperty.cpp
	Properties/TreeItems/BillboardTreeItemProperty.cpp
	Properties/TreeItems/BoneTreeItemProperty.cpp
	Properties/TreeItems/CameraTreeItemProperty.cpp
	Properties/TreeItems/FrameVariableBufferTreeItemProperty.cpp
	Properties/TreeItems/FrameVariableTreeItemProperty.cpp
	Properties/TreeItems/GeometryTreeItemProperty.cpp
	Properties/TreeItems/LightTreeItemProperty.cpp
	Properties/TreeItems/MaterialTreeItemProperty.cpp
	Properties/TreeItems/NodeTreeItemProperty.cpp
	Properties/TreeItems/OverlayTreeItemProperty.cpp
	Properties/TreeItems/PassTreeItemProperty.cpp
	Properties/TreeItems/PostEffectTreeItemProperty.cpp
	Properties/TreeItems/RenderTargetTreeItemProperty.cpp
	Properties/TreeItems/RenderWindowTreeItemProperty.cpp
	Properties/TreeItems/SceneTreeItemProperty.cpp
	Properties/TreeItems/SkeletonAnimationTreeItemProperty.cpp
	Properties/TreeItems/SkeletonTreeItemProperty.cpp
	Properties/TreeItems/SubmeshTreeItemProperty.cpp
	Properties/TreeItems/TextureTreeItemProperty.cpp
	Properties/TreeItems/ToneMappingTreeItemProperty.cpp
	Properties/TreeItems/TreeHolder.cpp
	Properties/TreeItems/TreeItemProperty.cpp
	Properties/TreeItems/ViewportTreeItemProperty.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp
	Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp
	Properties/TreeItems/AnimationTreeItemProperty.hpp
	Properties/TreeItems/BackgroundTreeItemProperty.hpp
	Properties/TreeItems/BillboardTreeItemProperty.hpp
	Properties/TreeItems/BoneTreeItemProperty.hpp
	Properties/TreeItems/CameraTreeItemProperty.hpp
	Properties/TreeItems/FrameVariableBufferTreeItemProperty.hpp
	Properties/TreeItems/FrameVariableTreeItemProperty.hpp
	Properties/TreeItems/GeometryTreeItemProperty.hpp
	Properties/TreeItems/LightTreeItemProperty.hpp
	Properties/TreeItems/MaterialTreeItemProperty.hpp
	Properties/TreeItems/NodeTreeItemProperty.hpp
	Properties/TreeItems/OverlayTreeItemProperty.hpp
	Properties/TreeItems/PassTreeItemProperty.hpp
	Properties/TreeItems/PostEffectTreeItemProperty.hpp
	Properties/TreeItems/RenderTargetTreeItemProperty.hpp
	Properties/TreeItems/RenderWindowTreeItemProperty.hpp
	Properties/TreeItems/SceneTreeItemProperty.hpp
	Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp
	Properties/TreeItems/SkeletonTreeItemProperty.hpp
	Properties/TreeItems/SubmeshTreeItemProperty.hpp
	Properties/TreeItems/TextureTreeItemProperty.hpp
	Properties/TreeItems/ToneMappingTreeItemProperty.hpp
	Properties/TreeItems/TreeHolder.hpp
	Properties/TreeItems/TreeItemProperty.hpp
	Properties/TreeItems/ViewportTreeItemProperty.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Properties\\TreeItems" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Properties\\TreeItems" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Shader/FrameVariablesList.cpp
	Shader/LanguageFileContext.cpp
	Shader/LanguageFileParser.cpp
	Shader/LanguageInfo.cpp
	Shader/ShaderDialog.cpp
	Shader/ShaderEditor.cpp
	Shader/ShaderProgramPage.cpp
	Shader/ShaderSource.cpp
	Shader/StcContext.cpp
	Shader/StcTextEditor.cpp
	Shader/StyleInfo.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	Shader/FrameVariablesList.hpp
	Shader/LanguageFileContext.hpp
	Shader/LanguageFileParser.hpp
	Shader/LanguageInfo.hpp
	Shader/ShaderDialog.hpp
	Shader/ShaderEditor.hpp
	Shader/ShaderProgramPage.hpp
	Shader/ShaderSource.hpp
	Shader/StcContext.hpp
	Shader/StcTextEditor.hpp
	Shader/StyleInfo.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	System/CastorApplication.cpp
	System/CubeBoxManager.cpp
	System/GradientButton.cpp
	System/ImagesLoader.cpp
	System/MaterialsList.cpp
	System/NodeState.cpp
	System/Recorder.cpp
	System/RendererSelector.cpp
	System/SceneExporter.cpp
	System/SceneObjectsList.cpp
	System/SplashScreen.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	System/CastorApplication.hpp
	System/CubeBoxManager.hpp
	System/GradientButton.hpp
	System/ImagesLoader.hpp
	System/MaterialsList.hpp
	System/NodeState.hpp
	System/Recorder.hpp
	System/RendererSelector.hpp
	System/SceneExporter.hpp
	System/SceneObjectsList.hpp
	System/SplashScreen.hpp
)
if ( APPLE )
	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
		System/MetalLayer.mm
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
		System/MetalLayer.h
	)
endif ()
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\System" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\System" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

add_target_min(
	${PROJECT_NAME}
	lib
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		${CASTOR_SOURCE_DIR}/samples
		${CASTOR_BINARY_DIR}/samples
		${wxWidgets_INCLUDE_DIRS}
		${GTK2_INCLUDE_DIRS}
	PRIVATE
		${Castor3DIncludeDirs}
		${FFMPEG_INCLUDE_DIR}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
		${wxWidgets_LIBRARIES}
		${GTK2_LIBRARIES}
		${X11_LIBRARIES}
		sdw::CompilerGlsl
		sdw::CompilerHlsl
		sdw::CompilerSpirV
	PRIVATE
		${FFMPEG_LIBRARIES}
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		${wxWidgets_DEFINITIONS}
)
if ( CASTOR_SAMPLE_CASTOR_VIEWER_RECORDS )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			GUICOMMON_RECORDS
	)
endif ()
install_target( ${PROJECT_NAME}
	lib
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Samples"
)

# Copying language files for GLSL and HLSL coloration in ShaderDialog
file(
	GLOB_RECURSE
		Parser_languages tools/resource/${PROJECT_NAME}/languages/*.lang
)

add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

if ( {${CASTOR_PACKAGE_DEB} )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "wxWidgets >= ${wxWidgetsVersion}" )
endif ()
