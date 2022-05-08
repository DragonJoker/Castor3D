#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementCUSmartPtr( castor3d, SubmeshRenderNode )

namespace castor3d
{
	SubmeshRenderNode::SubmeshRenderNode( Pass & pass
		, Submesh & data
		, Geometry & instance
		, ModelBufferConfiguration & modelData )
		: pass{ &pass }
		, data{ data }
		, instance{ instance }
		, modelData{ modelData }
	{
	}

	uint32_t SubmeshRenderNode::getId()const
	{
		return instance.getId( *pass, data );
	}

	uint32_t SubmeshRenderNode::getInstanceCount()const
	{
		auto & instantiation = data.getInstantiation();
		return instantiation.getRefCount( pass->getOwner() );
	}

	GeometryBuffers const & SubmeshRenderNode::getGeometryBuffers( ShaderFlags const & shaderFlags
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, Material & material
		, TextureFlagsArray const & texturesMask
		, bool forceTexCoords )const
	{
		return data.getGeometryBuffers( shaderFlags
			, submeshFlags
			, &material
			, texturesMask
			, forceTexCoords );
	}
}
