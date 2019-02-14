/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderCube_H___
#define ___C3D_RenderCube_H___
#pragma once

#include "Castor3DPrerequisites.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>
#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#include <Ashes/Descriptor/DescriptorSetPool.hpp>
#include <Ashes/Image/Sampler.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/Pipeline/Pipeline.hpp>
#include <Ashes/Pipeline/PipelineLayout.hpp>

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
		*\param[in] nearest
		*	Tells if the sampler needs a Nearest filter or not, if \p sampler is nullptr.
		*\param[in] sampler
		*	The sampler to use.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] nearest
		*	Dit si le sampler doit filtrer en Nearest ou pas, si \p sampler est nullptr.
		*\param[in] sampler
		*	Le sampler à utiliser.
		*/
		C3D_API explicit RenderCube( RenderSystem & renderSystem
			, bool nearest
			, SamplerSPtr sampler = nullptr );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipelines.
		*\param[in] size
		*	The render size.
		*\param[in] position
		*	The render position.
		*\param[in] program
		*	The shader program.
		*\param[in] view
		*	The source view.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\~french
		*\brief
		*	Crée les pipelines de rendu.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] position
		*	La position du rendu.
		*\param[in] program
		*	Le programme shader.
		*\param[in] view
		*	La vue cible.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
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
		*	Creates the rendering pipelines.
		*\param[in] size
		*	The render size.
		*\param[in] position
		*	The render position.
		*\param[in] program
		*	The shader program.
		*\param[in] view
		*	The source view.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\param[in] dsState
		*	The depth stencil state to use.
		*\~french
		*\brief
		*	Crée les pipelines de rendu.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] position
		*	La position du rendu.
		*\param[in] program
		*	Le programme shader.
		*\param[in] view
		*	La vue cible.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*\param[in] dsState
		*	L'état de profondeur et stencil à utiliser.
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
		*	Prepares the commands to render one face from the cube.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] subpassIndex
		*	The render subpass index.
		*\param[in] face
		*	The face index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin d'une face du cube.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] subpassIndex
		*	L'indice de la sous passe de rendu.
		*\param[in] face
		*	L'indice de la face.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex
			, uint32_t face );
		/**
		*\~english
		*\brief
		*	Prepares the commands to render one face from the cube, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] face
		*	The face index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin d'une face du cube, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] face
		*	L'indice de la face.
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
