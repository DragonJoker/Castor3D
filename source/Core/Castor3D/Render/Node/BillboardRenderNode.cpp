#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Scene/BillboardList.hpp"

CU_ImplementCUSmartPtr( castor3d, BillboardRenderNode )

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data )
		: passNode{ std::move( passNode ) }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ data }
	{
	}

	uint32_t BillboardRenderNode::getId()const
	{
		return instance.getId();
	}

	uint32_t BillboardRenderNode::getInstanceMult( uint32_t instanceMult )const
	{
		return data.getCount();
	}
}
