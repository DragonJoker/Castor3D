/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingRenderNode_H___
#define ___C3D_MorphingRenderNode_H___

#include "ObjectRenderNode.hpp"
#include "Shader/Ubos/MorphingUbo.hpp"

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
		C3D_API MorphingRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedMesh & mesh
			, UniformBufferOffset< MorphingUbo::Configuration > morphingUbo );

		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & mesh;
		//!\~english	The morphing UBO.
		//!\~french		L'UBO de morphing.
		UniformBufferOffset< MorphingUbo::Configuration > morphingUbo;
	};
}

#endif
