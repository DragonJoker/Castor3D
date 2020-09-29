/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "RenderToTextureModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>

#include <type_traits>

namespace castor3d
{
	struct RenderQuadConfig
	{
		/**
		*\~english
		*\brief
		*	Tells how the texture coordinates from the vertex buffer are built.
		*\~french
		*\brief
		*	Définit comment sont construites les coordonnées de texture du vertex buffer.
		*/
		struct Texcoord
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
		ashes::Optional< VkImageSubresourceRange > range;
		ashes::Optional< Texcoord > texcoordConfig;
		ashes::Optional< BlendMode > blendMode;
		ashes::Optional< uint32_t > descriptorSetsCount;
		ashes::Optional< bool > arraySource;
	};

	class RenderQuad
		: public castor::Named
	{
		template< typename ConfigT, typename BuilderT >
		friend class RenderQuadBuilderT;

	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] samplerFilter
		*	The sampler filter for the source texture.
		*\param[in] config
		*	The texture coordinates configuration.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] samplerFilter
		*	Le filtre d'échantillonnage pour la texture source.
		*\param[in] config
		*	La configuration des coordonnées de texture.
		*/
		C3D_API RenderQuad( RenderSystem & renderSystem
			, RenderDevice const & device
			, castor::String const & name
			, VkFilter samplerFilter
			, RenderQuadConfig config );

	public:
		C3D_API virtual ~RenderQuad();
		C3D_API explicit RenderQuad( RenderQuad && rhs )noexcept;
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
			, ashes::ImageView const & inputView
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
			, ashes::ImageView const & inputView
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
		*\param[in] descriptorSetIndex
		*	The render descriptor set index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] subpassIndex
		*	L'indice de la sous passe de rendu.
		*\param[in] descriptorSetIndex
		*	L'indice du descriptor set.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex
			, uint32_t descriptorSetIndex );
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
		inline void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex )
		{
			prepareFrame( renderPass, subpassIndex, 0u );
		}
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] descriptorSetIndex
		*	The render descriptor set index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] descriptorSetIndex
		*	L'indice du descriptor set.
		*/
		C3D_API void registerFrame( ashes::CommandBuffer & commandBuffer
			, uint32_t descriptorSetIndex )const;
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] descriptorSetIndex
		*	The render descriptor set index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] descriptorSetIndex
		*	L'indice du descriptor set.
		*/
		inline void registerFrame( ashes::CommandBuffer & commandBuffer )const
		{
			registerFrame( commandBuffer, 0u );
		}
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API uint32_t getDescriptorSetIndex( uint32_t descriptorBaseIndex
			, uint32_t level )const;

		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			CU_Require( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline RenderSystem * getRenderSystem()const
		{
			return &m_renderSystem;
		}

		inline RenderDevice const & getDevice()const
		{
			return m_device;
		}
		/**@}*/

	private:
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet
			, uint32_t descriptorSetIndex );
		inline void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet
			, uint32_t descriptorBaseIndex
			, uint32_t level )
		{
			doFillDescriptorSet( descriptorSetLayout
				, descriptorSet
				, getDescriptorSetIndex( descriptorBaseIndex, level ) );
		}

		C3D_API virtual void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const;

	protected:
		RenderSystem & m_renderSystem;
		RenderDevice const & m_device;
		SamplerSPtr m_sampler;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::CommandBufferPtr m_commandBuffer;

	private:
		std::array< TexturedQuad::Vertex, 4u > m_vertexData;
		ashes::VertexBufferPtr< TexturedQuad::Vertex > m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		std::vector< ashes::DescriptorSetPtr > m_descriptorSets;
		uint32_t m_descriptorSetCount{ 1u };
		ashes::ImageView const * m_sourceView{ nullptr };
		ashes::ImageViewArray m_sourceMipViews;
		bool m_useTexCoords;
		bool m_arraySource;
		BlendMode m_blendMode;
	};

	template< typename ConfigT, typename BuilderT >
	class RenderQuadBuilderT
	{
		static_assert( std::is_same_v< ConfigT, RenderQuadConfig >
			|| std::is_base_of_v< RenderQuadConfig, ConfigT >
			, "RenderQuadBuilderT::ConfigT must derive from castor3d::RenderQuadConfig" );

	public:
		inline RenderQuadBuilderT()
		{
		}
		/**
		*\~english
		*\param[in] config
		*	The texture coordinates configuration.
		*\~french
		*\param[in] config
		*	La configuration des coordonnées de texture.
		*/
		inline BuilderT & texcoordConfig( RenderQuadConfig::Texcoord const & config )
		{
			m_config.texcoordConfig = config;
			return static_cast< BuilderT & >( *this );
		}
		/**
		*\~english
		*\param[in] range
		*	Contains mip levels for the sampler.
		*\~french
		*\brief
		*\param[in] range
		*	Contient les mip levels, pour l'échantillonneur.
		*/
		inline BuilderT & range( VkImageSubresourceRange const & range )
		{
			m_config.range = range;
			return static_cast< BuilderT & >( *this );
		}
		/**
		*\~english
		*\param[in] range
		*	Contains mip levels for the sampler.
		*\~french
		*\brief
		*\param[in] range
		*	Contient les mip levels, pour l'échantillonneur.
		*/
		inline BuilderT & arraySource( bool arraySource = true )
		{
			m_config.arraySource = arraySource;
			return static_cast< BuilderT & >( *this );
		}
		/**
		*\~english
		*\param[in] blend
		*	Contains blend to destination status.
		*\~french
		*\brief
		*\param[in] blend
		*	Contient le statut de mélange à la destination.
		*/
		inline BuilderT & blendMode( BlendMode blendMode )
		{
			m_config.blendMode = blendMode;
			return static_cast< BuilderT & >( *this );
		}
		/**
		*\~english
		*\param[in] count
		*	Contains the number of descriptor sets to reserve.
		*\~french
		*\brief
		*\param[in] blend
		*	Contient le nombre de descriptor sets à réserver.
		*/
		inline BuilderT & descriptorSetsCount( uint32_t descriptorSetsCount )
		{
			m_config.descriptorSetsCount = descriptorSetsCount;
			return static_cast< BuilderT & >( *this );
		}
		/**
		*\~english
		*\brief
		*	Creates the RenderQuad.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] samplerFilter
		*	The sampler filter for the source texture.
		*\~french
		*\brief
		*	Crée le RenderQuad.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] samplerFilter
		*	Le filtre d'échantillonnage pour la texture source.
		*/
		inline RenderQuadUPtr build( RenderSystem & renderSystem
			, RenderDevice const & device
			, castor::String const & name
			, VkFilter samplerFilter )
		{
			return std::unique_ptr< RenderQuad >( new RenderQuad{ renderSystem
				, device
				, name
				, samplerFilter
				, m_config } );
		}

	protected:
		ConfigT m_config;
	};

	class RenderQuadBuilder
		: public RenderQuadBuilderT< RenderQuadConfig, RenderQuadBuilder >
	{
	};
}

#endif
