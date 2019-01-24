/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderCube_H___
#define ___C3D_RenderCube_H___
#pragma once

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Sampler.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>

#include <array>

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Class used to render a texture into a cube.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture dans un cube.
	*/
	class RenderCube
	{
	public:
		C3D_API virtual ~RenderCube();
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*/
		C3D_API explicit RenderCube( RenderSystem & renderSystem
			, bool nearest
			, SamplerSPtr sampler = nullptr );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipeline.
		*\param[in] program
		*	The shader program.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
		*\param[in] program
		*	Le programme shader.
		*/
		C3D_API void createPipelines( ashes::Extent2D const & size
			, castor::Position const & position
			, ashes::ShaderStageStateArray const & program
			, ashes::TextureView const & view
			, ashes::RenderPass const & renderPass
			, ashes::PushConstantRangeArray const & pushRanges );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipeline.
		*\param[in] program
		*	The shader program.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
		*\param[in] program
		*	Le programme shader.
		*/
		C3D_API void createPipelines( ashes::Extent2D const & size
			, castor::Position const & position
			, ashes::ShaderStageStateArray const & program
			, ashes::TextureView const & view
			, ashes::RenderPass const & renderPass
			, ashes::PushConstantRangeArray const & pushRanges
			, ashes::DepthStencilState const & dsState );
		/**
		*\~english
		*\brief
		*	Cleans up GPU objects.
		*\~french
		*\brief
		*	Nettoie les objets GPU.
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex
			, uint32_t face );
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*/
		C3D_API void registerFrame( ashes::CommandBuffer & commandBuffer
			, uint32_t face )const;
		/**
		*\~english
		*\return
		*	The command buffer to render the quad.
		*\~french
		*\return
		*	Le tampon de commandes de dessin du quad.
		*/
		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			CU_Require( m_commandBuffer );
			return *m_commandBuffer;
		}

	private:
		C3D_API virtual void doFillDescriptorLayoutBindings( ashes::DescriptorSetLayoutBindingArray & bindings );
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet
			, uint32_t face );
		C3D_API virtual void doRegisterFrame( ashes::CommandBuffer & commandBuffer
			, uint32_t face )const;

	protected:
		RenderSystem & m_renderSystem;
		SamplerSPtr m_sampler;
		ashes::PipelineLayoutPtr m_pipelineLayout;

	private:
		struct FacePipeline
		{
			ashes::PipelinePtr pipeline;
			ashes::DescriptorSetPtr descriptorSet;
		};

		ashes::CommandBufferPtr m_commandBuffer;
		ashes::UniformBufferPtr< castor::Matrix4x4f > m_matrixUbo;
		ashes::VertexBufferPtr< castor::Point4f > m_vertexBuffer;
		ashes::VertexLayoutPtr m_vertexLayout;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::array< FacePipeline, 6u > m_faces;
	};
}

#endif
