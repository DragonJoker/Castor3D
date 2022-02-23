/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	struct BillboardRenderNode
	{
		using DataType = BillboardBase;
		using InstanceType = BillboardBase;
		using VertexType = BillboardVertex;

		C3D_API BillboardRenderNode( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode( BillboardRenderNode && ) = default;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode const & ) = delete;
		C3D_API BillboardRenderNode & operator=( BillboardRenderNode && ) = delete;

		C3D_API BillboardRenderNode( PassRenderNode passNode
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data );

		C3D_API uint32_t getId()const;
		C3D_API uint32_t getInstanceCount( uint32_t instanceCount )const;

		PassRenderNode passNode;
		GeometryBuffers const & buffers;
		SceneNode & sceneNode;
		DataType & data;
		InstanceType & instance;
		ashes::DescriptorSet * texDescriptorSet{};
		GpuBufferOffsetT< BillboardUboConfiguration > billboardUbo;
	};
}

#endif
