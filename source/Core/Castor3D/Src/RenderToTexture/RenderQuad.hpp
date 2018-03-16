/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>

namespace castor3d
{
	/**
	*\~english
	*\brief
	*	Class used to render colour textures.
	*\~french
	*\brief
	*	Classe utilisée pour rendre les textures couleur.
	*/
	class RenderQuad
	{
	public:
		C3D_API virtual ~RenderQuad();
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] invertU
		*	Tells if the U coordinate of UV must be inverted, thus mirroring the reulting image.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] invertU
		*	Dit si la coordonnée U de l'UV doit être inversée, rendant ainsi un mirroir de l'image.
		*/
		C3D_API explicit RenderQuad( RenderSystem & renderSystem
			, bool nearest
			, bool invertU = false );
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
		C3D_API void createPipeline( renderer::Extent2D const & size
			, castor::Position const & position
			, renderer::ShaderStageStateArray const & program
			, renderer::TextureView const & view
			, renderer::RenderPass const & renderPass
			, renderer::DescriptorSetLayoutBindingArray bindings
			, renderer::PushConstantRangeCRefArray const & pushRanges );
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad.
		*/
		C3D_API void prepareFrame();
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*/
		C3D_API void registerFrame( renderer::CommandBuffer & commandBuffer )const;
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
		C3D_API virtual void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet );
		C3D_API virtual void doRegisterFrame( renderer::CommandBuffer & commandBuffer )const;

	protected:
		RenderSystem & m_renderSystem;
		SamplerSPtr m_sampler;
		renderer::PipelinePtr m_pipeline;
		renderer::PipelineLayoutPtr m_pipelineLayout;

	private:
		TexturedQuad m_vertexData;
		renderer::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		renderer::DescriptorSetLayoutPtr m_descriptorSetLayout;
		renderer::DescriptorSetPoolPtr m_descriptorSetPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
