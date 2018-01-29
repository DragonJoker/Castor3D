#include "MorphingRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	MorphingRenderNode::MorphingRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_passNode
		, ModelMatrixUbo & p_modelMatrixBuffer
		, ModelUbo & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, DataType & p_data
		, InstanceType & p_instance
		, AnimatedMesh & p_mesh
		, MorphingUbo & p_morphingUbo )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_passNode )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
		, m_mesh{ p_mesh }
		, m_morphingUbo{ p_morphingUbo }
	{
	}
}
