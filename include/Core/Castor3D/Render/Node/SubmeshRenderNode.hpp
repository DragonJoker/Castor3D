/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshRenderNode_H___
#define ___C3D_SubmeshRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	struct SubmeshRenderNode
	{
		using DataType = Submesh;
		using InstanceType = Geometry;

		C3D_API SubmeshRenderNode( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode( SubmeshRenderNode && ) = delete;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode && ) = delete;

		C3D_API SubmeshRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance );

		PassRenderNode passNode;
		UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
		UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSetPtr uboDescriptorSet;
		ashes::DescriptorSetPtr texDescriptorSet;
		// Morphing node
		AnimatedMesh * mesh{};
		UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo{};
		// Skinning node
		AnimatedSkeleton * skeleton{};
		UniformBufferOffsetT< SkinningUboConfiguration > skinningUbo{};
	};
}

#endif
