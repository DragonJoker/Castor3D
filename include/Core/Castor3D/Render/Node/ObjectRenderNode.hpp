/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectRenderNode_H___
#define ___C3D_ObjectRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNode.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	template< typename DataTypeT, typename InstanceTypeT >
	struct ObjectRenderNode
	{
		using DataType = DataTypeT;
		using InstanceType = InstanceTypeT;

		C3D_API ObjectRenderNode( ObjectRenderNode const & ) = delete;
		C3D_API ObjectRenderNode( ObjectRenderNode && ) = default;
		C3D_API ObjectRenderNode & operator=( ObjectRenderNode const & ) = delete;
		C3D_API ObjectRenderNode & operator=( ObjectRenderNode && ) = default;

		C3D_API ObjectRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance );

		PassRenderNode passNode;
		UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
		UniformBufferOffsetT< PickingUboConfiguration > pickingUbo;
		UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSetPtr uboDescriptorSet;
		ashes::DescriptorSetPtr texDescriptorSet;
	};
}

#endif
