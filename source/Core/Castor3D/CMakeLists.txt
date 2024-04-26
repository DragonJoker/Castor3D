project( Castor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_BRIEF "Multiplatform 3D engine" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 16 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

if ( NOT DEFINED CASTOR_USE_WEIGHED_BLENDED_OIT )
	option( CASTOR_USE_WEIGHED_BLENDED_OIT "Use Weighted Blended OIT for translucent objects" ON )
endif ()
if ( NOT DEFINED CASTOR_USE_VISIBILITY )
	option( CASTOR_USE_VISIBILITY "Use visibility buffer when rendering opaque objects" ON )
endif ()
if ( NOT DEFINED CASTOR_USE_ALLOCATION_CALLBACKS )
	option( CASTOR_USE_ALLOCATION_CALLBACKS "Use Vulkan allocation callbacks" OFF )
endif ()
if ( NOT DEFINED CASTOR_USE_MESH_SHADERS )
	option( CASTOR_USE_MESH_SHADERS "Use mesh shaders" ON )
endif ()
if ( NOT DEFINED CASTOR_USE_TASK_SHADERS )
	option( CASTOR_USE_TASK_SHADERS "Use task shaders along with mesh shaders" ON )
endif ()

set( C3D_USE_GLSLANG 0 )
set( C3D_USE_WBOIT 0 )
set( C3D_USE_ALLOCCB 0 )
set( C3D_USE_MESH 0 )
set( C3D_USE_TASK 0 )
set( C3D_USE_VISIBILITY 0 )

if ( CASTOR_USE_GLSLANG )
	set( C3D_USE_GLSLANG 1 )
endif ()

if ( CASTOR_USE_WEIGHED_BLENDED_OIT )
	set( C3D_USE_WBOIT 1 )
endif ()

if ( CASTOR_USE_VISIBILITY )
	set( C3D_USE_VISIBILITY 1 )
endif ()

if ( CASTOR_USE_ALLOCATION_CALLBACKS )
	set( C3D_USE_ALLOCCB 1 )
endif ()

if ( CASTOR_USE_MESH_SHADERS )
	set( C3D_USE_MESH 1 )
endif ()

if ( CASTOR_USE_TASK_SHADERS )
	set( C3D_USE_TASK 1 )
endif ()

set( ExternalIncludes )

configure_file(
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp.in
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
	NEWLINE_STYLE LF
)
configure_file(
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config.hpp.in
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/Config.hpp
	NEWLINE_STYLE LF
)
set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/Config.hpp
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3DModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3DPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Engine.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/ImporterFile.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Limits.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/DebugDefines.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Castor3DModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Engine.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/ImporterFile.cpp
)
set( ${PROJECT_NAME}_NVS_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3D.natvis
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )
source_group( "Natvis Files" FILES ${${PROJECT_NAME}_NVS_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Animation/AnimationImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Animation/AnimationModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animable.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animable.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animation.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/AnimationImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/AnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/AnimationModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Interpolator.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Animation" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Animation" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryBoneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryChunk.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMeshMorphTarget.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMorphComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySceneNodeAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySceneNodeAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationBone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkinComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySubmesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/CmshImporter.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryBoneNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryChunk.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMeshMorphTarget.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMorphComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryParser.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySceneNodeAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySceneNodeAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeleton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationBone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkinComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySubmesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryWriter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/ChunkData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/ChunkParser.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/ChunkWriter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/CmshImporter.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Binary" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Binary" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/DirectUploadData.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBufferLinearAllocator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBufferPackedAllocator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/ObjectBufferPool.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/PoolUniformBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/StagedUploadData.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UploadData.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UniformBufferBase.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/BufferModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/DirectUploadData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GeometryBuffers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBuffer.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferBuddyAllocator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferLinearAllocator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferOffset.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferPackedAllocator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/InstantUploadData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/ObjectBufferOffset.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/ObjectBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/ObjectBufferPool.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/PoolUniformBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/StagedUploadData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UploadData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBuffer.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferOffset.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.inl
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Buffer" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Buffer" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/AnimatedObjectGroupCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/BillboardCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/CacheModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/GeometryCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/LightCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/MaterialCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/OverlayCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/PluginCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/SceneNodeCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/ShaderCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/TargetCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/TextureCache.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/AnimatedObjectGroupCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/BillboardCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheView.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheView.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/GeometryCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/LightCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/MaterialCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ObjectCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ObjectCacheBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/OverlayCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/PluginCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/SceneNodeCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ShaderCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/TargetCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/TextureCache.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Cache" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Cache" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/CpuFrameEvent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/FrameEventModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/FrameListener.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/GpuFrameEvent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/CpuFrameEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/CpuFunctorEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FrameEventModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FrameListener.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/GpuFrameEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/GpuFunctorEvent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Event\\Frame" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Event\\Frame" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/UserInput/UserInputEventModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/UserInput/UserInputListener.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/EventHandler.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/HandlerEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/KeyboardEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/MouseEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/UserInputEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/UserInputEventModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/UserInput/UserInputListener.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Event\\UserInput" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Event\\UserInput" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Gui_Parsers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/GuiModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/ControlsManager.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Gui_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/GuiModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/ControlsManager.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Gui" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Gui" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlButton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlComboBox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlControl.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlEdit.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlExpandablePanel.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlLayoutControl.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlListBox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlPanel.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlProgress.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlScrollable.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlScrollBar.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlSlider.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Controls/CtrlStatic.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlButton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlComboBox.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlControl.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlEdit.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlExpandablePanel.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlLayoutControl.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlListBox.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlPanel.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlProgress.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlScrollable.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlScrollBar.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlSlider.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Controls/CtrlStatic.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Gui\\Controls" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Gui\\Controls" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Layout/Layout.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Layout/LayoutBox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Layout/LayoutItemFlags.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Layout/Layout.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Layout/LayoutBox.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Gui\\Layout" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Gui\\Layout" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleButton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleComboBox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleControl.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleEdit.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleExpandablePanel.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleListBox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StylePanel.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleProgress.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleScrollable.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleScrollBar.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleSlider.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StyleStatic.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StylesHolder.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/StylesHolder.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Gui/Theme/Theme.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Gui/Theme/StylesHolder.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Gui\\Style" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Gui\\Style" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Material.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/MaterialImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/MaterialModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Material.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/MaterialImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/MaterialModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Pass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PassFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PassModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PbrPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PhongPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/SubsurfaceScattering.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Pass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PassFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PassModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PbrPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PhongPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/SubsurfaceScattering.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/ComponentModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/PassComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/PassComponentRegister.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/PassMapComponent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/BaseDataPassComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/ComponentModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/PassComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/PassComponentRegister.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/PassMapComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Component" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Component" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/BlendComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/FractalMappingComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/NormalComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/PassHeaderComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/PickableComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/TexturesComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/TwoSidedComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Base/UntileMappingComponent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/BlendComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/FractalMappingComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/NormalComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/PassHeaderComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/PickableComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/TexturesComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/TwoSidedComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Base/UntileMappingComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Component\\Base" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Component\\Base" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/AmbientComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/AttenuationComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/ClearcoatComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/EmissiveComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/IridescenceComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/LightingModelComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/MetalnessComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/RoughnessComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SheenComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SpecularComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SpecularFactorComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/ThicknessComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/TransmissionComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/TransmittanceComponent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/AmbientComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/AttenuationComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/ClearcoatComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/EmissiveComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/IridescenceComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/LightingModelComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/MetalnessComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/RoughnessComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SheenComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SpecularComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/ThicknessComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/TransmissionComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Lighting/TransmittanceComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Component\\Lighting" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Component\\Lighting" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/AttenuationMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatNormalMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ColourMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/EmissiveMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/GlossinessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/HeightMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/IridescenceMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/IridescenceThicknessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/MetalnessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/NormalMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/OcclusionMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/OpacityMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/RoughnessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SheenMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SheenRoughnessMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SpecularFactorMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SpecularMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/TransmissionMapComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Map/TransmittanceMapComponent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/AttenuationMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/ColourMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/EmissiveMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/GlossinessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/HeightMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/IridescenceMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/IridescenceThicknessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/MetalnessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/NormalMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/OcclusionMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/OpacityMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/RoughnessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SheenMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SheenRoughnessMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/SpecularMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/TransmissionMapComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Map/TransmittanceMapComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Component\\Map" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Component\\Map" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/AlphaTestComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/ColourComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/DefaultReflRefrComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/HeightComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/OpacityComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/ReflectionComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Component/Other/RefractionComponent.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/AlphaTestComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/ColourComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/DefaultReflRefrComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/HeightComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/OpacityComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/ReflectionComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Component/Other/RefractionComponent.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Component\\Other" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Component\\Other" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Shaders/GlslPbrLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Shaders/GlslPhongLighting.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Shaders/GlslPbrLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Shaders/GlslPhongLighting.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Pass\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Pass\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/Sampler.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureConfiguration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureLayout.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureSourceInfo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureUnit.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureView.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/Sampler.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureConfiguration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureLayout.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureSource.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureSourceInfo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureUnit.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureView.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Texture" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Texture" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/Animation/TextureAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/Animation/TextureAnimationKeyFrame.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/Animation/TextureAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/Animation/TextureAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/Animation/TextureAnimationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Material\\Texture\\Animation" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Material\\Texture\\Animation" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/DebugCallbacks.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/DebugName.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/GpuInformations.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/GpuObjectTracker.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/LoadingScreen.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Logger.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/ProgressBar.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Version.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/VersionException.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/ConfigurationVisitor.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/DebugCallbacks.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/DebugName.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/GpuInformations.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/GpuObjectTracker.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/LoadingScreen.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Logger.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/MiscellaneousModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/makeVkType.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Parameter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Pattern.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/ProgressBar.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Version.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/VersionException.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Miscellaneous" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Miscellaneous" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Vertex.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/ModelModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Vertex.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/VertexGroup.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Mesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshGenerator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshPreparer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Mesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshGenerator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshPreparer.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Mesh" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Mesh" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationSubmesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshMorphTarget.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationSubmesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshMorphTarget.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Mesh\\Animation" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Mesh\\Animation" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cube.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cylinder.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Icosahedron.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Plane.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Projection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Sphere.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/SphereSection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Generator/Torus.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cube.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Cylinder.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Icosahedron.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/MeshGeneratorModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Plane.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Projection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Sphere.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/SphereSection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Generator/Torus.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Mesh\\Generator" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Mesh\\Generator" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Submesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/SubmeshUtils.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Submesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Submesh.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/SubmeshModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/SubmeshUtils.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Mesh\\Submesh" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Mesh\\Submesh" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BaseDataComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/DefaultRenderComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Face.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/IndexMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/InstantiationComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Line.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LinesMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MeshletComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MorphComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/PassMasksComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SkinComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponentRegister.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/TriFaceMapping.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BaseDataComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/ComponentModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Face.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/FaceIndices.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/FaceInfos.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/IndexMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/InstantiationComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Line.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LineIndices.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LinesMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MeshletComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MorphComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/PassMasksComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SkinComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/TriFaceMapping.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Mesh\\Submesh\\Component" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Mesh\\Submesh\\Component" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/BoneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/BonedVertex.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Skeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/SkeletonImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/SkeletonNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/VertexBoneData.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/BoneNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/BonedVertex.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Skeleton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/SkeletonImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/SkeletonNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/SkeletonModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/VertexBoneData.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Skeleton" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Skeleton" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationBone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationObject.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationBone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Animation/SkeletonAnimationObject.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Model\\Skeleton\\Animation" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Model\\Skeleton\\Animation" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/BorderPanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/DebugOverlays.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/FontTexture.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/Overlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayCategory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayVisitor.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/PanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/TextOverlay.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/BorderPanelOverlay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/DebugOverlays.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/FontTexture.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/Overlay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayCategory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayVisitor.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/PanelOverlay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/TextOverlay.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Overlay" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Overlay" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/DividerPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/GeneratorPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/GenericPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/ImporterPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/ParticlePlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/Plugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/PluginException.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/PostFxPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Plugin/ToneMappingPlugin.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/DividerPlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/GeneratorPlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/GenericPlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/ImporterPlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/ParticlePlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/Plugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/PluginException.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/PluginModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/PostFxPlugin.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Plugin/ToneMappingPlugin.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Plugin" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Plugin" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Frustum.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PipelineFlags.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Picking.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ray.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderDevice.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderInfo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoop.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoopAsync.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoopSync.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderNodesPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderPipeline.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderQueue.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderSystemFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderTarget.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderTechnique.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderTechniquePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderWindow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Texture.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Viewport.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Frustum.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PipelineFlags.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Picking.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ray.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderDevice.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderInfo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoop.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoopAsync.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoopSync.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderNodesPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderPipeline.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderQueue.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderSystem.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderSystemFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderTarget.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderTechnique.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderTechniquePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderTechniqueVisitor.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderWindow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Texture.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Viewport.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/AssignLightsToClusters.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/BucketSortLights.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/BuildLightsBVH.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ClustersConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ClusteredModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ClustersMask.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ComputeClustersAABB.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ComputeLightsAABB.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ComputeLightsMortonCode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/FindUniqueClusters.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/FrustumClusters.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/MergeSortLights.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/ReduceLightsAABB.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Clustered/SortAssignedLights.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/AssignLightsToClusters.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/BucketSortLights.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/BuildLightsBVH.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ClustersConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ClusteredModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ClustersMask.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ComputeClustersAABB.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ComputeLightsAABB.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ComputeLightsMortonCode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/FindUniqueClusters.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/FrustumClusters.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/MergeSortLights.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/ReduceLightsAABB.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Clustered/SortAssignedLights.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Clustered" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Clustered" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/DummyCuller.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/FrustumCuller.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/PipelineNodes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/SceneCuller.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/CullingModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/DummyCuller.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/FrustumCuller.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/PipelineNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/SceneCuller.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Culling" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Culling" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Debug/DebugDrawer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Debug/DebugDrawer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Debug/DebugModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Debug" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Debug" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/EnvironmentMap/EnvironmentMap.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/EnvironmentMap/EnvironmentMapPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/EnvironmentMap/EnvironmentMap.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/EnvironmentMap/EnvironmentMapModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/EnvironmentMap/EnvironmentMapPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\EnvironmentMap" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\EnvironmentMap" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/GlobalIlluminationModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/GlobalIlluminationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\GlobalIllumination" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\GlobalIllumination" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\GlobalIllumination\\LightPropagationVolumes" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\GlobalIllumination\\LightPropagationVolumes" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VctConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelSecondaryBounce.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelizePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GlobalIllumination/VoxelConeTracing/VoxelSecondaryBounce.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\GlobalIllumination\\VoxelConeTracing" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\GlobalIllumination\\VoxelConeTracing" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/BillboardRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/QueueRenderNodes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SceneRenderNodes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SubmeshRenderNode.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/BillboardRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/InstantiatedPipelinesNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/PipelinesNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/QueueRenderNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/RenderNodeModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SceneRenderNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SubmeshRenderNode.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Node" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Node" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Opaque/OpaqueRendering.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Opaque/SubsurfaceScatteringPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Opaque/VisibilityReorderPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Opaque/VisibilityResolvePass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Opaque/OpaqueModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Opaque/OpaqueRendering.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Opaque/SubsurfaceScatteringPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Opaque/VisibilityReorderPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Opaque/VisibilityResolvePass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Opaque" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Opaque" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Overlays/OverlayPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Overlays/OverlayPreparer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Overlays/OverlayRenderer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Overlays/OverlayTextBufferPool.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlaysModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayPreparer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayPreparer.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayRenderer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayTextBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayVertexBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Overlays/OverlayVertexBufferPool.inl
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Overlays" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Overlays" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/BackgroundPassBase.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/BackgroundRenderer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/CommandsSemaphore.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/ComputeDepthRange.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/ForwardRenderTechniquePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/GaussianBlur.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/LineariseDepthPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/PickingPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/RenderQuad.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/BackgroundPassBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/BackgroundRenderer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/CommandsSemaphore.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/ComputeDepthRange.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/ForwardRenderTechniquePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/GaussianBlur.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/LineariseDepthPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/PassesModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/PickingPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/RenderQuad.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Passes" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Passes" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PBR/BrdfPrefilter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PBR/EnvironmentPrefilter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PBR/IblTextures.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PBR/RadianceComputer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PBR/BrdfPrefilter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PBR/EnvironmentPrefilter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PBR/IblTextures.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PBR/PbrModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PBR/RadianceComputer.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\PBR" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\PBR" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PostEffect/PostEffect.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectFactory.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffect.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\PostEffect" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\PostEffect" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Prepass/DepthPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Prepass/PrepassRendering.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Prepass/PrepassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Prepass/VisibilityPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Prepass/DepthPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Prepass/PrepassModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Prepass/PrepassRendering.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Prepass/PrepassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Prepass/VisibilityPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Prepass" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Prepass" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMap.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapDirectional.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassDirectional.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassPoint.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassSpot.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPoint.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapSpot.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMap.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapDirectional.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassDirectional.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassPoint.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPassSpot.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapPoint.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ShadowMap/ShadowMapSpot.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\ShadowMap" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\ShadowMap" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoBlurPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoRawAOPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoBlurPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoRawAOPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\SSAO" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\SSAO" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transparent/TransparentPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transparent/TransparentPassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transparent/TransparentRendering.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transparent/WeightedBlendRendering.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transparent/TransparentModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transparent/TransparentPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transparent/TransparentRendering.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transparent/TransparentPassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transparent/WeightedBlendRendering.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Transparent" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Transparent" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/ColourGradingConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/HdrConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMappingFactory.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/ColourGradingConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/HdrConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMappingFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMappingModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMappingVisitor.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\ToneMapping" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\ToneMapping" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/EquirectangularToCube.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/RenderCube.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/Texture3DTo2D.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/EquirectangularToCube.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderCube.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderToTextureModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/Texture3DTo2D.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\ToTexture" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\ToTexture" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/MeshletBoundsTransformPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/MeshletBoundsTransformingPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/TransformPipeline.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/VertexTransformPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/VertexTransforming.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Transform/VertexTransformingPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/MeshletBoundsTransformPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/MeshletBoundsTransformingPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/TransformModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/TransformPipeline.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/VertexTransformPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/VertexTransforming.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Transform/VertexTransformingPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Transform" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Transform" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/BillboardList.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Camera.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/CameraImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Geometry.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/MovableObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/RenderedObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Scene.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneFileParser.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneFileParser_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneNodeImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Shadow.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/BillboardList.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Camera.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/CameraImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Fog.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Geometry.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/MovableObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/RenderedObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Scene.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneFileParser.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneFileParserData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneFileParser_Parsers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneNodeImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Shadow.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedMesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObjectGroup.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedTexture.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSceneNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSkeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimationInstance.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/SceneNodeAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/SceneNodeAnimationKeyFrame.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedMesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObjectGroup.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSceneNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSkeleton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedTexture.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimationModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/SceneNodeAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/SceneNodeAnimationKeyFrame.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Animation" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Animation" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Mesh/MeshAnimationInstance.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Mesh/MeshAnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Mesh/MeshAnimationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Animation\\Mesh" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Animation\\Mesh" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/SceneNode/SceneNodeAnimationInstance.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/SceneNode/SceneNodeAnimationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Animation\\SceneNode" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Animation\\SceneNode" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstance.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Animation\\Skeleton" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Animation\\Skeleton" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/Texture/TextureAnimationInstance.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Texture/TextureAnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/Texture/TextureAnimationModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Animation\\Texture" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Animation\\Texture" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Background.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/BackgroundModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Colour.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Image.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/PipelineCreator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Skybox.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Visitor.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Background.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/BackgroundModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Colour.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Image.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Skybox.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Visitor.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Background" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Background" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslIblBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslImgBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslNoIblBackground.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslIblBackground.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslImgBackground.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Shaders/GlslNoIblBackground.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Background\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Background\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/DirectionalLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/Light.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/LightImporter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/LightModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/LightCategory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/LightFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/PointLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/SpotLight.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/DirectionalLight.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/Light.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/LightCategory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/LightFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/LightImporter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/LightModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/PointLight.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Light/SpotLight.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\Light" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\Light" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ComputeParticleSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/CpuParticleSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/Particle.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleEmitter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleDeclaration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleSystemImpl.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleUpdater.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ComputeParticleSystem.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/CpuParticleSystem.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/Particle.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/Particle.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleEmitter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleDeclaration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleElementDeclaration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleSystem.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleSystemImpl.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/ParticleSystem/ParticleUpdater.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Scene\\ParticleSystem" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Scene\\ParticleSystem" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/GlslToSpv.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/LightingModelFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Program.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderAppendBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/GlslToSpv.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/LightingModelFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Program.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderAppendBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/StructuredShaderBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/StructuredShaderBuffer.inl
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/FontGlyphBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/LightBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/PassBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/ShadowBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/SssProfileBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/TextureAnimationBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffers/TextureConfigurationBuffer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/FontGlyphBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/LightBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/PassBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/ShaderBuffersModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/ShadowBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/SssProfileBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/TextureAnimationBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader\\ShaderBuffers" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader\\ShaderBuffers" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslAABB.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslAppendBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslBlendComponents.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslBRDFHelpers.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslClusteredLights.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslCookTorranceBRDF.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslCullData.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslDebugOutput.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslDerivativeValue.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslFog.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslFont.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslGlobalIllumination.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslLightSurface.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMaterial.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMeshlet.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMeshVertex.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputComponents.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslPassShaders.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslRay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslReflection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslShadow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSheenBRDF.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssProfile.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssTransmittance.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSubmeshShaders.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSurface.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureConfiguration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslUtils.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslVoxel.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslAABB.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslAppendBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBackground.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBaseIO.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBitonicSort.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBlendComponents.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslBRDFHelpers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslClusteredLights.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslCookTorranceBRDF.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslCullData.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslDebugOutput.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslDerivativeValue.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslDerivativeValue.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslFog.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslFont.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslGlobalIllumination.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslLight.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslLightSurface.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMaterial.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMeshlet.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMeshVertex.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMeshVertex.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslObjectIds.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputComponents.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputs.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputs.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslOverlaySurface.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslPassShaders.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslRadixSort.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslRay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslReflection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSheenBRDF.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssProfile.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssTransmittance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSubmeshShaders.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSurface.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSurface.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTaskPayload.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTaskPayload.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureConfiguration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureConfiguration.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureTransform.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslUtils.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslVoxel.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/SdwModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader\\Shaders" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/BillboardUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/CameraUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ColourGradingUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ClustersUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/HdrConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/LayeredLpvGridConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/LpvGridConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/LpvLightConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ModelDataUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/MorphingUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ObjectIdsUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/OverlayUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/SceneUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ShadowMapUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/SkinningUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/SsaoConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/VoxelizerUbo.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/BillboardUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/CameraUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ColourGradingUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ClustersUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/HdrConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/LayeredLpvGridConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/LpvGridConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/LpvLightConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ModelDataUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/MorphingUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ObjectIdsUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/OverlayUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/SceneUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ShadowMapUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/SkinningUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/SsaoConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/UbosModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/VoxelizerUbo.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader\\Ubos" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader\\Ubos" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)
set( Castor3D_MATERIAL_DIRS
	${Castor3D_MATERIAL_DIRS}
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass
	PARENT_SCOPE
)
add_target_min(
	${PROJECT_NAME}
	api_dll
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
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
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_BINARY_DIR}/include/Core
		${CASTOR_SOURCE_DIR}/source/Core
		${CASTOR_BINARY_DIR}/source/Core
)
if ( VCPKG_BUILD OR CASTOR_FORCE_VCPKG_SUBMODULES )
else ()
	target_include_directories( ${PROJECT_NAME}
		PUBLIC
			$<BUILD_INTERFACE:${ASHES_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${ASHES_CONFIG_DIR}>
	)
endif ()
target_include_directories( ${PROJECT_NAME} SYSTEM
	PRIVATE
		${glslang_INCLUDE_DIR}
		${ExternalIncludes}
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		${Castor3DDefinitions}
)
if ( APPLE )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			VK_USE_PLATFORM_MACOS_MVK
	)
elseif ( WIN32 )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			VK_USE_PLATFORM_WIN32_KHR
	)
elseif ( ANDROID )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			VK_USE_PLATFORM_ANDROID_KHR
	)
else ()
	find_package( X11 )
	if ( X11_FOUND )
		target_compile_definitions( ${PROJECT_NAME}
			PUBLIC
				VK_USE_PLATFORM_XLIB_KHR
		)
	endif ()
	find_package( XCB REQUIRED COMPONENTS xcb )
	if ( XCB_FOUND )
		target_compile_definitions( ${PROJECT_NAME}
			PUBLIC
				VK_USE_PLATFORM_XCB_KHR
		)
	endif ()
	find_file( WAYLAND_CLIENT_HDR NAMES wayland-client.h )
	if ( WAYLAND_CLIENT_HDR )
		target_compile_definitions( ${PROJECT_NAME}
			PUBLIC
				VK_USE_PLATFORM_WAYLAND_KHR
		)
	endif ()
endif ()

set( mikktspace_DIR ${VCPKG_SHARE_DIR}/mikktspace )
find_package( mikktspace CONFIG REQUIRED )

target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::CastorUtils
		sdw::ShaderAST
		sdw::ShaderWriter
		crg::RenderGraph
	PRIVATE
		sdw::CompilerSpirV
		${glslang_LIBRARIES}
		meshoptimizer::meshoptimizer
		mikktspace::mikktspace
)
if ( CASTOR_C3D_NEEDS_GLSL )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			C3D_HasGLSL
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			sdw::CompilerGlsl
	)
endif ()

target_compile_options( ${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/bigobj>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		FOLDER "Core/${MAIN_PROJECT_NAME}"
)
c3d_release_pdbs( ${PROJECT_NAME} )
install(
	FILES
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/Config.hpp
	DESTINATION include/${PROJECT_NAME}
	COMPONENT Castor3D
	CONFIGURATIONS Release
)
target_install_headers( ${PROJECT_NAME}
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}
)
install_target_ex( ${PROJECT_NAME}
	Castor3D
	Castor3D
	api_dll
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc_ex( ${PROJECT_NAME} "French" "*.h *.hpp" ${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME} )
target_add_doc_ex( ${PROJECT_NAME} "English" "*.h *.hpp" ${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME} )

# Copying core zip data files
file( GLOB CoreZipFiles
	${CASTOR_SOURCE_DIR}/data/${PROJECT_NAME}/*.zip
)
copy_target_files_ex( ${PROJECT_NAME}
	Castor3D
	""
	${CoreZipFiles}
)

# Copying shader languages files
file( GLOB ShaderLanguagesFiles
	${CASTOR_SOURCE_DIR}/languages/${PROJECT_NAME}/*.lang
)
copy_target_files_ex( ${PROJECT_NAME}
	Castor3D
	""
	${ShaderLanguagesFiles}
)

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
