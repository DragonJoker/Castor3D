/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningRenderNode_H___
#define ___C3D_SkinningRenderNode_H___

#include "ObjectRenderNode.hpp"
#include "Shader/Ubos/SkinningUbo.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/06/2016
	\~english
	\brief		Helper structure used to render animated submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages animés.
	*/
	struct SkinningRenderNode
		: public SubmeshRenderNode
	{
		C3D_API SkinningRenderNode( RenderPipeline & p_pipeline
			, PassRenderNode && p_passNode
			, ModelMatrixUbo & p_modelMatrixBuffer
			, ModelUbo & p_modelBuffer
			, GeometryBuffers & p_buffers
			, SceneNode & p_sceneNode
			, DataType & p_data
			, InstanceType & p_instance
			, AnimatedSkeleton & p_skeleton
			, SkinningUbo & p_skinningUbo );

		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton & m_skeleton;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		SkinningUbo & m_skinningUbo;
	};
}

#endif
