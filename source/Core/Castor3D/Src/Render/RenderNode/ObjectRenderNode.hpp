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
#include "Shader/Ubos/PickingUbo.hpp"

#include <Descriptor/DescriptorSet.hpp>

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

		C3D_API ObjectRenderNode( ObjectRenderNode const & ) = delete;
		C3D_API ObjectRenderNode( ObjectRenderNode && ) = default;
		C3D_API ObjectRenderNode & operator=( ObjectRenderNode const & ) = delete;
		C3D_API ObjectRenderNode & operator=( ObjectRenderNode && ) = default;

		C3D_API ObjectRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance );

		//!\~english	The pipeline.
		//!\~french		Le pipeline.
		RenderPipeline & pipeline;
		//!\~english	The pipeline.
		//!\~french		Le pipeline.
		PassRenderNode passNode;
		//!\~english	The model matrices UBO.
		//!\~french		L'UBO de matrices modèle.
		UniformBufferOffset < ModelMatrixUbo::Configuration > modelMatrixUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffset< ModelUbo::Configuration > modelUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffset< PickingUbo::Configuration > pickingUbo;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de la géométrie.
		GeometryBuffers const & buffers;
		//!\~english	The parent scene node.
		//!\~french		Le scene node parent.
		SceneNode & sceneNode;
		//!\~english	The object's data.
		//!\~french		Les données de l'objet.
		DataType & data;
		//!\~english	The object instantiating the data.
		//!\~french		L'objet instanciant les données.
		InstanceType & instance;
		//!\~english	The UBO descriptor set for the object.
		//!\~french		L'ensemble de descripteurs d'UBO pour l'objet.
		renderer::DescriptorSetPtr uboDescriptorSet;
		//!\~english	The textures descriptor set for the object.
		//!\~french		L'ensemble de descripteurs de textures pour l'objet.
		renderer::DescriptorSetPtr texDescriptorSet;
	};
	using SubmeshRenderNode = ObjectRenderNode< Submesh, Geometry >;
	using BillboardListRenderNode = ObjectRenderNode< BillboardBase, BillboardBase >;
}

#endif
