#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data )
		: BillboardListRenderNode{ std::move( passNode )
			, modelBuffer
			, modelInstancesBuffer
			, buffers
			, sceneNode
			, data
			, data }
		, billboardUbo{ billboardBuffer }
	{
	}
}
