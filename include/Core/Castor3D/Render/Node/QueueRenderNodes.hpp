/*
See LICENSE file in root folder
*/
#ifndef ___C3D_QueueRenderNodes_H___
#define ___C3D_QueueRenderNodes_H___

#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
		//!\~english	The submesh render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de submesh, triés par programme shader.
		SubmeshRenderNodePtrByPipelineMap submeshNodes;
		//!\~english	The submesh render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de submesh, triés par programme shader.
		SubmeshRenderNodesPtrByPipelineMap instancedSubmeshNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardRenderNodePtrByPipelineMap billboardNodes;

		C3D_API explicit QueueRenderNodes( RenderQueue const & queue );

		C3D_API void sort( ShadowMapLightTypeArray & shadowMaps );
		C3D_API void prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors );

		C3D_API SubmeshRenderNode & createNode( Pass & pass
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( Pass & pass
			, BillboardBase & instance );
	};
}

#endif
