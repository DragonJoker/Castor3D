/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectRenderNode_H___
#define ___C3D_ObjectRenderNode_H___

#include "RenderNodeModule.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNode.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

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
			, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
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
		UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< ModelUbo::Configuration > modelUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< PickingUbo::Configuration > pickingUbo;
		//!\~english	The model UBO.
		//!\~french		L'UBO de modèle.
		UniformBufferOffsetT< TexturesUbo::Configuration > texturesUbo;
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
