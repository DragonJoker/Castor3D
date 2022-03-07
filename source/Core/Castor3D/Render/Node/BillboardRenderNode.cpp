#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Scene/BillboardList.hpp"

CU_ImplementCUSmartPtr( castor3d, BillboardRenderNode )

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( Pass & pass
		, BillboardBase & data )
		: pass{ pass }
		, data{ data }
		, instance{ data }
	{
	}

	uint32_t BillboardRenderNode::getId()const
	{
		return instance.getId( pass );
	}

	uint32_t BillboardRenderNode::getInstanceCount()const
	{
		return data.getCount();
	}

	GeometryBuffers const & BillboardRenderNode::getGeometryBuffers( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, Material & material
		, TextureFlagsArray const & texturesMask
		, bool forceTexCoords )const
	{
		return data.getGeometryBuffers();
	}
}
