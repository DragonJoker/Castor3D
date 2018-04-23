#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	template< typename NodeType >
	inline void doUpdateNodeModelMatrix( NodeType & node )
	{
		node.modelMatrixUbo.getData().model = convert( node.sceneNode.getDerivedTransformationMatrix() );
	}

	template< typename NodeType >
	inline void doUpdateNode( NodeType & node )
	{
		node.modelUbo.getData().shadowReceiver = node.instance.isShadowReceiver();
		node.modelUbo.getData().materialIndex = node.passNode.pass.getId() - 1u;
		doUpdateNodeModelMatrix( node );
	}
}
