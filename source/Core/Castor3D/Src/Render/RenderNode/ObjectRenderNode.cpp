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
		: pipeline{ pipeline }
		, passNode{ std::move( passNode ) }
		, modelMatrixUbo{ modelMatrixBuffer }
		, modelUbo{ modelBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
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
		: pipeline{ pipeline }
		, passNode{ std::move( passNode ) }
		, modelMatrixUbo{ modelMatrixBuffer }
		, modelUbo{ modelBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}
}
