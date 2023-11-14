#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Scene/BillboardList.hpp"

CU_ImplementSmartPtr( castor3d, BillboardRenderNode )

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( Pass & pass
		, BillboardBase & data
		, ModelBufferConfiguration & modelData
		, BillboardUboConfiguration & billboardData )
		: pass{ &pass }
		, data{ data }
		, instance{ data }
		, modelData{ modelData }
		, billboardData{ billboardData }
	{
	}

	uint32_t BillboardRenderNode::getId()const
	{
		return instance.getId( *pass );
	}

	uint32_t BillboardRenderNode::getInstanceCount()const
	{
		return data.getCount();
	}

	ObjectBufferOffset const & BillboardRenderNode::getSourceBufferOffsets()const
	{
		return data.getBufferOffsets();
	}

	ObjectBufferOffset const & BillboardRenderNode::getFinalBufferOffsets()const
	{
		return data.getBufferOffsets();
	}

	SubmeshComponentCombineID BillboardRenderNode::getComponentCombineID()const
	{
		return data.getComponentCombineID();
	}

	MorphFlags BillboardRenderNode::getMorphFlags()const
	{
		return MorphFlags{};
	}

	ProgramFlags BillboardRenderNode::getProgramFlags()const
	{
		return data.getProgramFlags();
	}

	VkPrimitiveTopology BillboardRenderNode::getPrimitiveTopology()const
	{
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	}

	GeometryBuffers const & BillboardRenderNode::getGeometryBuffers( PipelineFlags const & flags )const
	{
		return data.getGeometryBuffers();
	}

	GpuBufferOffsetT< castor::Point4f > BillboardRenderNode::getMorphTargets()const
	{
		return {};
	}

	SceneNode & BillboardRenderNode::getSceneNode()const
	{
		return *instance.getNode();
	}
}
