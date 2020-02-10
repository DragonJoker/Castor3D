#include "Castor3D/Render/RenderNode/ObjectRenderNode.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	template<>
	ObjectRenderNode< Submesh, Geometry >::ObjectRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffset< TexturesUbo::Configuration > texturesBuffer
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
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffset< TexturesUbo::Configuration > texturesBuffer
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
