/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshRenderNode_H___
#define ___C3D_SubmeshRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	struct SubmeshRenderNode
	{
		using DataType = Submesh;
		using InstanceType = Geometry;
		using VertexType = InterleavedVertex;

		C3D_API SubmeshRenderNode( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode( SubmeshRenderNode && ) = default;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode && ) = delete;

		C3D_API SubmeshRenderNode( PassRenderNode passNode
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance );

		C3D_API uint32_t getId()const;
		C3D_API uint32_t getInstanceMult( uint32_t instanceMult )const;

		PassRenderNode passNode;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSet * uboDescriptorSet{};
		ashes::DescriptorSet * texDescriptorSet{};
		// Morphing node
		AnimatedMesh * mesh{};
		// Skinning node
		AnimatedSkeleton * skeleton{};
		GpuBufferOffsetT< SkinningUboConfiguration > skinningSsbo{};
	};
}

#endif
