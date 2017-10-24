/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingRenderNode_H___
#define ___C3D_MorphingRenderNode_H___

#include "ObjectRenderNode.hpp"

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
	struct MorphingRenderNode
		: public SubmeshRenderNode
	{
		C3D_API MorphingRenderNode( RenderPipeline & p_pipeline
			, PassRenderNode && p_passNode
			, ModelMatrixUbo & p_modelMatrixBuffer
			, ModelUbo & p_modelBuffer
			, GeometryBuffers & p_buffers
			, SceneNode & p_sceneNode
			, DataType & p_data
			, InstanceType & p_instance
			, AnimatedMesh & p_mesh
			, MorphingUbo & p_morphingUbo );

		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & m_mesh;
		//!\~english	The morphing UBO.
		//!\~french		L'UBO de morphing.
		MorphingUbo & m_morphingUbo;
	};
}

#endif
