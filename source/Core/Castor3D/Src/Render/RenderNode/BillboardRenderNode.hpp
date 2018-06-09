/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "ObjectRenderNode.hpp"
#include "Shader/Ubos/BillboardUbo.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render billboards.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des billboards.
	*/
	struct BillboardRenderNode
		: public BillboardListRenderNode
	{
		C3D_API BillboardRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffset< BillboardUbo::Configuration > billboardBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, BillboardBase & data );

		//!\~english	The billboard UBO.
		//!\~french		L'UBO de billboard.
		UniformBufferOffset< BillboardUbo::Configuration > billboardUbo;
	};
}

#endif
