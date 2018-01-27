/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQuad_H___
#define ___C3D_RenderQuad_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Buffer/GeometryBuffers.hpp>
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
		C3D_API virtual ~RenderQuad() = default;
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
		C3D_API void createPipeline( castor::Size const & size
			, castor::Position const & position
			, renderer::ShaderProgram const & program
			, renderer::TextureView const & view
			, renderer::RenderPass const & renderPass
			, std::vector< renderer::DescriptorSetLayoutBinding > bindings );
		/**
		*\~english
		*\brief
		*	Records the frame in the given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer into which rendering commands are registered.
		*\~french
		*\brief
		*	Enregistre la frame dans le tampon de commandes donné.
		*\param[in] commandBuffer
		*	Le tampon de commandes dans lequel les commandes de rendu sont enregistrées.
		*/
		C3D_API void registerFrame( renderer::CommandBuffer & commandBuffer );

	private:
		virtual void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet );

	protected:
		RenderSystem & m_renderSystem;

	private:
		TexturedQuad m_vertexData;
		SamplerSPtr m_sampler;
		renderer::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		renderer::GeometryBuffersPtr m_geometryBuffers;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
	};
}

#endif
