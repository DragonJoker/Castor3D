/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectRenderNode_H___
#define ___C3D_ObjectRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNode.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
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
			, UniformBufferOffsetT< ModelMatrixUboConfiguration > modelMatrixBuffer
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
			, UniformBufferOffsetT< TexturesUboConfiguration > texturesBuffer
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
		UniformBufferOffsetT< ModelMatrixUboConfiguration > modelMatrixUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< PickingUboConfiguration > pickingUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< TexturesUboConfiguration > texturesUbo;
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
		ashes::DescriptorSetPtr uboDescriptorSet;
		//!\~english	The textures descriptor set for the object.
		//!\~french		L'ensemble de descripteurs de textures pour l'objet.
		ashes::DescriptorSetPtr texDescriptorSet;
	};
}

#endif
