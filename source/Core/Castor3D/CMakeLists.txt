project( Castor3D )

option( CASTOR_DEBUG_TARGETS "Allows displaying intermediate render targets results" OFF )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_BRIEF "Multiplatform 3D engine" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( ExternalIncludes )

if ( CASTOR_USE_GLSLANG )
	set( Castor3DDefinitions
		${Castor3DDefinitions}
		C3D_HasGlslang
	)
endif ()

configure_file(
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp.in
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
	NEWLINE_STYLE LF
)
set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3DModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3DPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Engine.hpp
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Castor3DModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Engine.cpp
)
set( ${PROJECT_NAME}_NVS_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Castor3D.natvis
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )
source_group( "Natvis Files" FILES ${${PROJECT_NAME}_NVS_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Animation/AnimationModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animable.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animable.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Animation/Animation.inl
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryBone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryBonesComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryChunk.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimation.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationBone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/BinarySubmesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Binary/CmshImporter.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryBone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryBonesComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryChunk.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryMeshAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinaryParser.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeleton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationBone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Binary/BinarySkeletonAnimationObject.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/PoolUniformBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UniformBufferBase.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Buffer/UniformBufferPools.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/BufferModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GeometryBuffers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferBuddyAllocator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferOffset.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/GpuBufferPool.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/PoolUniformBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBuffer.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferOffset.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPool.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPools.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Buffer/UniformBufferPools.inl
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/BillboardUboPools.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/CameraCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/GeometryCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/LightCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/ListenerCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/MaterialCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/MeshCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/OverlayCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/ParticleSystemCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/PluginCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/SamplerCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/SceneCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/SceneNodeCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/ShaderCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/TargetCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/TechniqueCache.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Cache/WindowCache.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/AnimatedObjectGroupCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/BillboardCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/BillboardUboPools.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/Cache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheView.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CacheView.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/CameraCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ElementProducer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/GeometryCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/LightCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ListenerCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/MaterialCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/MeshCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ObjectCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ObjectCacheBase.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/OverlayCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ParticleSystemCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/PluginCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/SamplerCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/SceneCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/SceneNodeCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/ShaderCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/TargetCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/TechniqueCache.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Cache/WindowCache.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/FrameEvent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/FrameEventModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Event/Frame/FrameListener.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/CleanupEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FrameEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FrameEventModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FrameListener.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/FunctorEvent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Event/Frame/InitialiseEvent.hpp
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

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Material.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/MaterialModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Material.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/MetallicRoughnessPbrPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/Pass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PassModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/PhongPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/SpecularGlossinessPbrPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Pass/SubsurfaceScattering.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/MetallicRoughnessPbrPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/Pass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PassModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/PhongPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Pass/SpecularGlossinessPbrPass.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/Sampler.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureConfiguration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureLayout.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureUnit.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Material/Texture/TextureView.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/Sampler.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureConfiguration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureLayout.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Material/Texture/TextureSource.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/DebugCallbacks.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/DebugName.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/GpuInformations.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/GpuObjectTracker.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Logger.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Parameter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Version.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/VersionException.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/DebugCallbacks.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/DebugName.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/GpuInformations.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/GpuObjectTracker.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Logger.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/MiscellaneousModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/makeVkType.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Parameter.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Pattern.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/PipelineVisitor.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Importer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/ImporterFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Mesh.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/MeshGenerator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Subdivider.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Importer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/ImporterFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Mesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshGenerator.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/MeshModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Subdivider.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationKeyFrame.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationSubmesh.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimation.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Animation/MeshAnimationSubmesh.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BonesComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BonesInstantiationComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Face.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/IndexMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/InstantiationComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Line.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LinesMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MorphComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponent.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/TriFaceMapping.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BonesComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/BonesInstantiationComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/ComponentModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Face.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/FaceIndices.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/FaceInfos.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/IndexMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/InstantiationComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/Line.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LineIndices.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/LinesMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/MorphComponent.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Mesh/Submesh/Component/SubmeshComponent.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Bone.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/BonedVertex.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/Skeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Model/Skeleton/VertexBoneData.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Bone.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/BonedVertex.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Model/Skeleton/Skeleton.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayRenderer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/OverlayVisitor.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/PanelOverlay.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Overlay/TextOverlay.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/BorderPanelOverlay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/DebugOverlays.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/DebugOverlays.inl
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/FontTexture.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/Overlay.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayCategory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Overlay/OverlayRenderer.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PickingPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ray.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderDevice.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderInfo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoop.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoopAsync.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderLoopSync.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderPassTimer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderPipeline.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderQueue.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderSystem.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderSystemFactory.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderTarget.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/RenderWindow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Viewport.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Frustum.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/GBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PickingPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ray.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderDevice.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderInfo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoop.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoopAsync.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderLoopSync.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderPassTimer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderPipeline.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderQueue.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderSystem.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderSystemFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderTarget.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/RenderWindow.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/DummyCuller.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/FrustumCuller.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Culling/SceneCuller.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/CullingModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/DummyCuller.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Culling/FrustumCuller.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/BillboardRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/DistanceRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/MorphingRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/ObjectRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/PassRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/RenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SceneCulledRenderNodes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SceneRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SceneRenderNodes.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/SkinningRenderNode.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Node/StaticRenderNode.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/BillboardRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/DistanceRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/MorphingRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/ObjectRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/PassRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/RenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/RenderNodeModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/RenderNode_Render.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SceneCulledRenderNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SceneRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SceneRenderNodes.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/SkinningRenderNode.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Node/StaticRenderNode.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/CombinePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/DepthPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/DownscalePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/GaussianBlur.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/LineariseDepthPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Passes/StencilPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/CombinePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/DepthPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/CommandsSemaphore.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/DownscalePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/GaussianBlur.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/LineariseDepthPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/PassesModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Passes/StencilPass.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectSurface.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffect.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectFactory.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/PostEffect/PostEffectSurface.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Ssao/SsaoRawAOPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoBlurPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Ssao/SsaoModule.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/ForwardRenderTechniquePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/RenderTechnique.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/RenderTechniquePass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/ForwardRenderTechniquePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/RenderTechnique.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/RenderTechniquePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/RenderTechniqueVisitor.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/TechniqueModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/DeferredRendering.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaquePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaquePassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaqueResolvePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/SsaoPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/DeferredRendering.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaqueModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaquePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaquePassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/OpaqueResolvePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/SsaoPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Opaque" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Opaque" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/DirectionalLightPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightingPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightPassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/MeshLightPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/PointLightPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/SpotLightPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightingPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightPassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightPassShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/MeshLightPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/LightingModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/PointLightPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/SpotLightPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Opaque\\Lighting" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Opaque\\Lighting" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/GeometryInjectionPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/GeometryInjectionPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LayeredLightVolumeGIPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightPassLayeredVolumePropagationShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightPassVolumePropagationShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightPropagationPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/LightVolumeGI/LightVolumeGIPass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Opaque\\LightVolumeGI" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Opaque\\LightVolumeGI" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGi/ReflectiveShadowMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGi/RsmConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGi/RsmGIPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGi/RsmInterpolatePass.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/LightPassReflectiveShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/ReflectiveShadowMapGIModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/ReflectiveShadowMapping.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmGIPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmInterpolatePass.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Opaque\\ReflectiveShadowMapGI" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Opaque\\ReflectiveShadowMapGI" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentPass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentPassResult.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentResolvePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Transparent/WeightedBlendRendering.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentPass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentPassResult.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Transparent/TransparentResolvePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Transparent/WeightedBlendRendering.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Transparent" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Transparent" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Voxelize/VoxelizePass.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/Technique/Voxelize/Voxelizer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Voxelize/VoxelizeModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Voxelize/VoxelizePass.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/Technique/Voxelize/Voxelizer.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Render\\Technique\\Voxelize" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Render\\Technique\\Voxelize" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/HdrConfig.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMapping.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToneMapping/ToneMappingFactory.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/RenderDepthQuad.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/RenderQuad.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/TextureProjection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Render/ToTexture/TextureProjectionToCube.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/EquirectangularToCube.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderCube.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderDepthQuad.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderQuad.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/RenderToTextureModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/TextureProjection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Render/ToTexture/TextureProjectionToCube.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/BillboardList.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Camera.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Geometry.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/MovableObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/RenderedObject.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Scene.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneFileParser.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneFileParser_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/SceneNode.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/BillboardList.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Camera.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Fog.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Geometry.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/MovableObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/RenderedObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Scene.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneFileParser.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneFileParser_Parsers.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/SceneNode.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSkeleton.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Animation/AnimationInstance.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedMesh.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObject.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedObjectGroup.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimatedSkeleton.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimationInstance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Animation/AnimationModule.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Background.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/BackgroundModule.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/BackgroundTextWriter.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Colour.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Image.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/PipelineCreator.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Skybox.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Background/Visitor.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/Background.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/BackgroundModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Scene/Background/BackgroundTextWriter.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/DirectionalLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Scene/Light/Light.cpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Program.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/ShaderModule.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/GlslToSpv.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Program.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/PassBuffer/MetallicRoughnessPassBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/PassBuffer/PassBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/PassBuffer/PhongPassBuffer.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/PassBuffer/SpecularGlossinessPassBuffer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/PassBuffer/MetallicRoughnessPassBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/PassBuffer/PassBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/PassBuffer/PassBufferModule.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/PassBuffer/PhongPassBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader\\PassBuffer" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader\\PassBuffer" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslCookTorranceBRDF.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslFog.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslLight.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMaterial.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMetallicBrdfLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslMetallicPbrReflection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputComponents.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslPhongLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslPhongReflection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslShadow.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSpecularBrdfLighting.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSpecularPbrReflection.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssTransmittance.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureConfiguration.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Shaders/GlslUtils.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslCookTorranceBRDF.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslFog.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslLight.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMaterial.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMetallicBrdfLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslMetallicPbrReflection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslOutputComponents.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslPhongLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslPhongReflection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslShadow.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSpecularBrdfLighting.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSpecularPbrReflection.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslSssTransmittance.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslTextureConfiguration.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Shaders/GlslUtils.hpp
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
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/TextureConfigurationBuffer/TextureConfigurationBufferModule.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Header Files\\Shader\\TextureConfigurationBuffer" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
source_group( "Source Files\\Shader\\TextureConfigurationBuffer" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/BillboardUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/GpInfoUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/HdrConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/LayeredLpvConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/LpvConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/MatrixUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ModelMatrixUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ModelUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/MorphingUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/OverlayUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/PickingUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/RsmConfigUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/SceneUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/ShadowMapUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/SkinningUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Shader/Ubos/TexturesUbo.cpp
)
set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/BillboardUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/DebugConfig.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/GpInfoUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/HdrConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/LayeredLpvConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/LpvConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/MatrixUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ModelMatrixUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ModelUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/MorphingUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/OverlayUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/PickingUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/RsmConfigUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/SceneUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/ShadowMapUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/SkinningUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/TexturesUbo.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Shader/Ubos/UbosModule.hpp
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
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${ASHES_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${ASHES_CONFIG_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_BINARY_DIR}/include/Core
		${CASTOR_SOURCE_DIR}/source/Core
		${CASTOR_BINARY_DIR}/source/Core
		${glslang_INCLUDE_DIR}
		${ExternalIncludes}
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		${Castor3DDefinitions}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::CastorUtils
		sdw::ShaderWriter
	PRIVATE
		sdw::CompilerSpirV
		${glslang_LIBRARIES}
)
if ( CASTOR_DEBUG_TARGETS )
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			C3D_DebugQuads
	)
endif ()
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

install_target( ${PROJECT_NAME} api_dll ${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME} )
set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp *.dox" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp *.dox" )

# Copying core zip data files
file(
	GLOB
		CoreZipFiles
		${CASTOR_SOURCE_DIR}/data/${PROJECT_NAME}/*.zip
)
copy_target_files( ${PROJECT_NAME} "" ${CoreZipFiles} )

# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CASTOR_SOURCE_DIR}/languages/${PROJECT_NAME}/*.lang
)
copy_target_files( ${PROJECT_NAME} "" ${ShaderLanguagesFiles} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

