/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshRenderNode_H___
#define ___C3D_SubmeshRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	struct SubmeshRenderNode
	{
		using DataType = Submesh;
		using InstanceType = Geometry;

		C3D_API SubmeshRenderNode( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode( SubmeshRenderNode && )noexcept = default;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode const & ) = delete;
		C3D_API SubmeshRenderNode & operator=( SubmeshRenderNode && )noexcept = delete;
		C3D_API ~SubmeshRenderNode()noexcept = default;

		C3D_API SubmeshRenderNode( Pass & pass
			, DataType & data
			, InstanceType & instance
			, ModelBufferConfiguration & modelData );

		C3D_API uint32_t getId()const;
		C3D_API uint32_t getInstanceCount()const;
		C3D_API MaterialObs getMaterial()const;
		C3D_API ObjectBufferOffset const & getSourceBufferOffsets()const;
		C3D_API ObjectBufferOffset const & getFinalBufferOffsets()const;
		C3D_API SubmeshComponentCombineID getComponentCombineID()const;
		C3D_API MorphFlags getMorphFlags()const;
		C3D_API ProgramFlags getProgramFlags()const;
		C3D_API VkPrimitiveTopology getPrimitiveTopology()const;
		C3D_API GeometryBuffers const & getGeometryBuffers( PipelineFlags const & flags )const;
		C3D_API void createMeshletDescriptorSet()const;
		C3D_API ashes::DescriptorSetLayout const * getMeshletDescriptorLayout()const;
		C3D_API ashes::DescriptorSet const & getMeshletDescriptorSet()const;
		C3D_API GpuBufferOffsetT< MeshletCullData > const & getSourceMeshletsBounds()const;
		C3D_API GpuBufferOffsetT< MeshletCullData > const & getFinalMeshletsBounds()const;
		C3D_API GpuBufferOffsetT< castor::Point4f > const & getMorphTargets()const;
		C3D_API SceneNode & getSceneNode()const;
		C3D_API SubmeshRenderData * getRenderData()const;
		C3D_API bool isInstanced()const;

		Pass * pass;
		DataType & data;
		InstanceType & instance;
		ModelBufferConfiguration & modelData;
		// Morphing node
		AnimatedMesh * mesh{};
		// Skinning node
		AnimatedSkeleton * skeleton{};
	};
}

#endif
