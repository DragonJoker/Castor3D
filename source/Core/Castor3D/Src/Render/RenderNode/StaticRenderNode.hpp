/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StaticRenderNode_H___
#define ___C3D_StaticRenderNode_H___

#include "ObjectRenderNode.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render static submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages non animés.
	*/
	struct StaticRenderNode
		: public SubmeshRenderNode
	{
		C3D_API StaticRenderNode( RenderPipeline & p_pipeline
			, PassRenderNode && p_passNode
			, ModelMatrixUbo & p_modelMatrixBuffer
			, ModelUbo & p_modelBuffer
			, renderer::GeometryBuffers & p_buffers
			, SceneNode & p_sceneNode
			, Submesh & p_data
			, Geometry & p_instance );
	};
}

#endif
