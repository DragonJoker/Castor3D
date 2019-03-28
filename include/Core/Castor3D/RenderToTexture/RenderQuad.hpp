/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#include <Ashes/Descriptor/DescriptorSetPool.hpp>
#include <Ashes/Pipeline/Pipeline.hpp>
#include <Ashes/Pipeline/PipelineLayout.hpp>
#include <Ashes/Pipeline/VertexLayout.hpp>
#include <Ashes/Pipeline/Viewport.hpp>

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Class used to render a texture into a quad.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture dans un quad.
	*/
	class RenderQuad
	{
	public:
		C3D_API virtual ~RenderQuad();
		C3D_API explicit RenderQuad( RenderQuad && rhs );
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] nearest
		*	Tells if the sampler needs a Nearest filter or not.
		*\param[in] invertU
		*	Tells if the U coordinate of UV must be inverted, thus mirroring vertically the resulting image.
		*\param[in] invertV
		*	Tells if the U coordinate of UV must be inverted, thus mirroring horizontally the resulting image.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] nearest
		*	Dit si le sampler doit filtrer en Nearest ou pas.
		*\param[in] invertU
		*	Dit si la coordonnée U de l'UV doit être inversée, rendant ainsi un miroir vertical de l'image.
		*\param[in] invertV
		*	Dit si la coordonnée V de l'UV doit être inversée, rendant ainsi un miroir horizontal de l'image.
		*/
		C3D_API explicit RenderQuad( RenderSystem & renderSystem
			, bool nearest
			, bool invertU = false
			, bool invertV = false );
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
		C3D_API void createPipeline( ashes::Extent2D const & size
			, castor::Position const & position
			, ashes::ShaderStageStateArray const & program
			, ashes::TextureView const & view
			, ashes::RenderPass const & renderPass
			, ashes::DescriptorSetLayoutBindingArray bindings
			, ashes::PushConstantRangeArray const & pushRanges );
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
		C3D_API void createPipeline( ashes::Extent2D const & size
			, castor::Position const & position
			, ashes::ShaderStageStateArray const & program
			, ashes::TextureView const & view
			, ashes::RenderPass const & renderPass
			, ashes::DescriptorSetLayoutBindingArray bindings
			, ashes::PushConstantRangeArray const & pushRanges
			, ashes::DepthStencilState dsState );
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

	private:
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet );
		C3D_API virtual void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const;

	protected:
		RenderSystem & m_renderSystem;
		SamplerSPtr m_sampler;
		ashes::PipelinePtr m_pipeline;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::CommandBufferPtr m_commandBuffer;

	private:
		std::array< TexturedQuad::Vertex, 4u > m_vertexData;
		ashes::VertexBufferPtr< TexturedQuad::Vertex > m_vertexBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::TextureView const * m_sourceView{ nullptr };
	};
}

#endif
