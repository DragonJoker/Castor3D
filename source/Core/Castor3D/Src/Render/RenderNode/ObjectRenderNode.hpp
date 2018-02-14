/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectRenderNode_H___
#define ___C3D_ObjectRenderNode_H___

#include "PassRenderNode.hpp"
#include "SceneRenderNode.hpp"

#include "Buffer/UniformBufferPool.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render objects.
	\~french
	\brief		Structure d'aide utilisée pour le dessin d'objets.
	*/
	template< typename DataTypeT, typename InstanceTypeT >
	struct ObjectRenderNode
	{
		using DataType = DataTypeT;
		using InstanceType = InstanceTypeT;

		C3D_API ObjectRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, renderer::GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance );

		//!\~english	The pipeline.
		//!\~french		Le pipeline.
		RenderPipeline & m_pipeline;
		//!\~english	The pipeline.
		//!\~french		Le pipeline.
		PassRenderNode m_passNode;
		//!\~english	The model matrices UBO.
		//!\~french		L'UBO de matrices modèle.
		UniformBufferOffset < ModelMatrixUbo::Configuration > m_modelMatrixUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffset< ModelUbo::Configuration > m_modelUbo;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		renderer::GeometryBuffers const & m_buffers;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & m_sceneNode;
		//!\~english	The object's data.
		//!\~french		Les données de l'objet.
		DataType & m_data;
		//!\~english	The object instantiating the data.
		//!\~french		L'objet instanciant les données.
		InstanceType & m_instance;
	};
	using SubmeshRenderNode = ObjectRenderNode< Submesh, Geometry >;
	using BillboardListRenderNode = ObjectRenderNode< BillboardBase, BillboardBase >;
}

#endif
