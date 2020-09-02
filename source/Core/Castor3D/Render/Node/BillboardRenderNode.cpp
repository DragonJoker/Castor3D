#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffsetT< BillboardUbo::Configuration > billboardBuffer
		, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data )
		: BillboardListRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, pickingBuffer
			, texturesBuffer
			, buffers
			, sceneNode
			, data
			, data }
		, billboardUbo{ billboardBuffer }
	{
	}
}
