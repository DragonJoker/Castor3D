/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
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
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode && ) = delete;

		C3D_API BillboardRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelIndexUboConfiguration > modelIndexBuffer
			, GpuDataBufferOffset * modelDataBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer );

		PassRenderNode passNode;
		UniformBufferOffsetT< ModelIndexUboConfiguration > modelIndexUbo;
		GpuDataBufferOffset * modelDataUbo;
		UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSet * uboDescriptorSet{};
		ashes::DescriptorSet * texDescriptorSet{};
		UniformBufferOffsetT< BillboardUboConfiguration > billboardUbo;
	};
}

#endif
