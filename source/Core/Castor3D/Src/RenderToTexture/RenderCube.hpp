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
		C3D_API void createPipelines( renderer::Extent2D const & size
			, castor::Position const & position
			, renderer::ShaderStageStateArray const & program
			, renderer::TextureView const & view
			, renderer::RenderPass const & renderPass
			, renderer::PushConstantRangeCRefArray const & pushRanges );
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
		C3D_API void createPipelines( renderer::Extent2D const & size
			, castor::Position const & position
			, renderer::ShaderStageStateArray const & program
			, renderer::TextureView const & view
			, renderer::RenderPass const & renderPass
			, renderer::PushConstantRangeCRefArray const & pushRanges
			, renderer::DepthStencilState const & dsState );
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
		C3D_API void prepareFrame( renderer::RenderPass const & renderPass
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
		C3D_API void registerFrame( renderer::CommandBuffer & commandBuffer
			, uint32_t face )const;
		/**
		*\~english
		*\return
		*	The command buffer to render the quad.
		*\~french
		*\return
		*	Le tampon de commandes de dessin du quad.
		*/
		inline renderer::CommandBuffer const & getCommandBuffer()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

	private:
		C3D_API virtual void doFillDescriptorLayoutBindings( renderer::DescriptorSetLayoutBindingArray & bindings );
		C3D_API virtual void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet
			, uint32_t face );
		C3D_API virtual void doRegisterFrame( renderer::CommandBuffer & commandBuffer
			, uint32_t face )const;

	protected:
		RenderSystem & m_renderSystem;
		SamplerSPtr m_sampler;
		renderer::PipelineLayoutPtr m_pipelineLayout;

	private:
		struct FacePipeline
		{
			renderer::PipelinePtr pipeline;
			renderer::DescriptorSetPtr descriptorSet;
		};

		renderer::CommandBufferPtr m_commandBuffer;
		renderer::UniformBufferPtr< castor::Matrix4x4f > m_matrixUbo;
		renderer::VertexBufferPtr< castor::Point4f > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		std::array< FacePipeline, 6u > m_faces;
	};
}

#endif
