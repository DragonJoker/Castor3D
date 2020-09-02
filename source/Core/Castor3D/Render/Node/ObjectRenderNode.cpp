#include "Castor3D/Render/Node/ObjectRenderNode.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Program.hpp"

namespace castor3d
{
	template<>
	ObjectRenderNode< Submesh, Geometry >::ObjectRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: pipeline{ pipeline }
		, passNode{ std::move( passNode ) }
		, modelMatrixUbo{ modelMatrixBuffer }
		, modelUbo{ modelBuffer }
		, pickingUbo{ pickingBuffer }
		, texturesUbo{ texturesBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}

	template<>
	ObjectRenderNode< BillboardBase, BillboardBase >::ObjectRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data
		, BillboardBase & instance )
		: pipeline{ pipeline }
		, passNode{ std::move( passNode ) }
		, modelMatrixUbo{ modelMatrixBuffer }
		, modelUbo{ modelBuffer }
		, pickingUbo{ pickingBuffer }
		, texturesUbo{ texturesBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}
}
