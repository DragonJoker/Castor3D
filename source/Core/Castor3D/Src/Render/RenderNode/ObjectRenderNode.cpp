#include "ObjectRenderNode.hpp"

#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	template<>
	ObjectRenderNode< Submesh, Geometry >::ObjectRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: m_pipeline{ pipeline }
		, m_passNode{ std::move( passNode ) }
		, m_modelMatrixUbo{ modelMatrixBuffer }
		, m_modelUbo{ modelBuffer }
		, m_buffers{ buffers }
		, m_sceneNode{ sceneNode }
		, m_data{ data }
		, m_instance{ instance }
	{
	}

	template<>
	ObjectRenderNode< BillboardBase, BillboardBase >::ObjectRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data
		, BillboardBase & instance )
		: m_pipeline{ pipeline }
		, m_passNode{ std::move( passNode ) }
		, m_modelMatrixUbo{ modelMatrixBuffer }
		, m_modelUbo{ modelBuffer }
		, m_buffers{ buffers }
		, m_sceneNode{ sceneNode }
		, m_data{ data }
		, m_instance{ instance }
	{
	}
}
