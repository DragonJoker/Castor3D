#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementCUSmartPtr( castor3d, SubmeshRenderNode )

namespace castor3d
{
	SubmeshRenderNode::SubmeshRenderNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: passNode{ std::move( passNode ) }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}

	uint32_t SubmeshRenderNode::getId()const
	{
		return instance.getId( data );
	}

	uint32_t SubmeshRenderNode::getInstanceCount( uint32_t instanceCount )const
	{
		return instanceCount;
	}
}
