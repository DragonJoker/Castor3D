#include "StaticRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	StaticRenderNode::StaticRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_passNode
		, ModelMatrixUbo & p_modelMatrixBuffer
		, ModelUbo & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Submesh & p_data
		, Geometry & p_instance )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_passNode )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
	{
	}
}
