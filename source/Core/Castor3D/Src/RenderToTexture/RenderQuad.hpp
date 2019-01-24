/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
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
			, bool invertU = false
			, bool invertV = false );
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
		*\param[in] program
		*	The shader program.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
		*\param[in] program
		*	Le programme shader.
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
		*	Prpares the commands to render the quad.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex );
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
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
