#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Scene/BillboardList.hpp"

CU_ImplementCUSmartPtr( castor3d, BillboardRenderNode )

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

	SubmeshFlags BillboardRenderNode::getSubmeshFlags()const
	{
		return data.getSubmeshFlags();
	}

	MorphFlags BillboardRenderNode::getMorphFlags()const
	{
		return MorphFlags{};
	}

	ProgramFlags BillboardRenderNode::getProgramFlags()const
	{
		return data.getProgramFlags();
	}

	GeometryBuffers const & BillboardRenderNode::getGeometryBuffers( ShaderFlags const & shaderFlags
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, TextureFlagsArray const & texturesMask )const
	{
		return data.getGeometryBuffers();
	}
}
