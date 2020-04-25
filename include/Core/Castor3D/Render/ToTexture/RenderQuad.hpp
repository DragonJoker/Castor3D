/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "RenderToTextureModule.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>

namespace castor3d
{
	class RenderQuad
	{
	public:
		/**
		*\~english
		*\brief
		*	Tells how the texture coordinates from the vertex buffer are built.
		*\~french
		*\brief
		*	Définit comment sont construites les coordonnées de texture du vertex buffer.
		*/
		struct TexcoordConfig
		{
			/*
			*\~english
			*\brief
			*	Tells if the U coordinate of UV must be inverted, thus mirroring vertically the resulting image.
			*\~french
			*	Dit si la coordonnée U de l'UV doit être inversée, rendant ainsi un miroir vertical de l'image.
			*/
			bool invertU{ false };
			/*
			*\~english
			*\brief
			*	Tells if the U coordinate of UV must be inverted, thus mirroring horizontally the resulting image.
			*\~french
			*	Dit si la coordonnée V de l'UV doit être inversée, rendant ainsi un miroir horizontal de l'image.
			*/
			bool invertV{ false };
		};

	private:
		C3D_API RenderQuad( RenderSystem & renderSystem
			, VkFilter samplerFilter
			, TexcoordConfig const * config );

	public:
		C3D_API virtual ~RenderQuad();
		C3D_API explicit RenderQuad( RenderQuad && rhs )noexcept;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] nearest
			*	Tells if the sampler needs a Nearest filter or not.
		*\param[in] config
		*	The texture coordinates configuration.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] nearest
			*	Dit si le sampler doit filtrer en Nearest ou pas.
		*\param[in] config
		*	La configuration des coordonnées de texture.
		*/
		C3D_API RenderQuad( RenderSystem & renderSystem
			, VkFilter samplerFilter
			, TexcoordConfig const & config );
		/**
		*\~english
		*\brief
		*	Constructor.
		*\remarks
		*	Doesn't use the texture coordinates.
		*\~french
		*\brief
		*	Constructeur.
		*\remarks
		*	N'utilise pas les coordonnées de texture.
		*/
		C3D_API RenderQuad( RenderSystem & renderSystem
			, VkFilter samplerFilter );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipeline.
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
		*\param[in] bindings
		*	The already existing bindings.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
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
		*\param[in] bindings
		*	Les attaches déjà existantes.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*/
		C3D_API void createPipeline( VkExtent2D const & size
			, castor::Position const & position
			, ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::ImageView const & view
			, ashes::RenderPass const & renderPass
			, ashes::VkDescriptorSetLayoutBindingArray bindings
			, ashes::VkPushConstantRangeArray const & pushRanges );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipeline.
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
		*\param[in] bindings
		*	The already existing bindings.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\param[in] dsState
		*	The depth stencil state to use.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
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
		*\param[in] bindings
		*	Les attaches déjà existantes.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*\param[in] dsState
		*	L'état de profondeur et stencil à utiliser.
		*/
		C3D_API void createPipeline( VkExtent2D const & size
			, castor::Position const & position
			, ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::ImageView const & view
			, ashes::RenderPass const & renderPass
			, ashes::VkDescriptorSetLayoutBindingArray bindings
			, ashes::VkPushConstantRangeArray const & pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo dsState );
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
		*	Prepares the commands to render the quad.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] subpassIndex
		*	The render subpass index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] subpassIndex
		*	L'indice de la sous passe de rendu.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex );
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*/
		C3D_API void registerFrame( ashes::CommandBuffer & commandBuffer )const;
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

		inline RenderSystem * getRenderSystem()const
		{
			return &m_renderSystem;
		}

	private:
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API virtual void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const;

	protected:
		RenderSystem & m_renderSystem;
		SamplerSPtr m_sampler;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::CommandBufferPtr m_commandBuffer;

	private:
		std::array< TexturedQuad::Vertex, 4u > m_vertexData;
		ashes::VertexBufferPtr< TexturedQuad::Vertex > m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::ImageView const * m_sourceView{ nullptr };
		bool m_useTexCoords;
	};
}

#endif
