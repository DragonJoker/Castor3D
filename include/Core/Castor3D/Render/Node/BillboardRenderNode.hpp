/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	struct BillboardRenderNode
	{
		using DataType = BillboardBase;
		using InstanceType = BillboardBase;

		C3D_API BillboardRenderNode( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode( BillboardRenderNode && ) = default;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode && ) = default;

		C3D_API BillboardRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer );

		PassRenderNode passNode;
		UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
		UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSetPtr uboDescriptorSet;
		ashes::DescriptorSetPtr texDescriptorSet;
		UniformBufferOffsetT< BillboardUboConfiguration > billboardUbo;
	};
}

#endif
