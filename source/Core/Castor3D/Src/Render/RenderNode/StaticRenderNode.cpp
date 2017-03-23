#include "StaticRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	StaticRenderNode::StaticRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Submesh & p_data
		, Geometry & p_instance )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_pass )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
	{
	}
}
