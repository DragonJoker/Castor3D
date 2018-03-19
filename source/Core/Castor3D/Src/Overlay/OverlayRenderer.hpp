/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OVERLAY_RENDERER_H___
#define ___C3D_OVERLAY_RENDERER_H___

#include <Pipeline/VertexLayout.hpp>
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/OverlayUbo.hpp"
#include "Castor3DPrerequisites.hpp"
#include "TextOverlay.hpp"

#include <Design/OwnedBy.hpp>

#ifdef drawText
#	undef drawText
#endif

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		The overlay renderer.
	\~french
	\brief		Le renderer d'incrustation.
	*/
	class OverlayRenderer
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	target			The target texture view.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	target			La vue de texture cible.
		 */
		C3D_API explicit OverlayRenderer( RenderSystem & renderSystem
			, renderer::TextureView const & target );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~OverlayRenderer();
		/**
		 *\~english
		 *\brief		Initialises the buffers.
		 *\~french
		 *\brief		Initialise les tampons.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Flushes the renderer.
		 *\~french
		 *\brief		Nettoie le renderer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Function to draw a PanelOverlay.
		 *\param[in]	overlay	The overlay to draw.
		 *\~french
		 *\brief		Fonction de dessin d'un PanelOverlay.
		 *\param[in]	overlay	L'incrustation à dessiner.
		 */
		C3D_API void drawPanel( PanelOverlay & overlay );
		/**
		 *\~english
		 *\brief		Function to draw a BorderPanelOverlay.
		 *\param[in]	overlay	The overlay to draw.
		 *\~french
		 *\brief		Fonction de dessin d'un BorderPanelOverlay.
		 *\param[in]	overlay	L'incrustation à dessiner.
		 */
		C3D_API void drawBorderPanel( BorderPanelOverlay & overlay );
		/**
		 *\~english
		 *\brief		Function to draw a TextOverlay.
		 *\param[in]	overlay	The overlay to draw.
		 *\~french
		 *\brief		Fonction de dessin d'un TextOverlay.
		 *\param[in]	overlay	L'incrustation à dessiner.
		 */
		C3D_API void drawText( TextOverlay & overlay );
		/**
		 *\~english
		 *\brief		Begins the overlays rendering.
		 *\param[in]	viewport	The render window viewport.
		 *\~french
		 *\brief		Commence le rendu des incrustations.
		 *\param[in]	viewport	Le viewport de la fenêtre de rendu.
		 */
		C3D_API void beginRender( Viewport const & viewport );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering.
		 *\~french
		 *\brief		Termine le rendu des incrustations.
		 */
		C3D_API void endRender();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline renderer::CommandBuffer const & getCommands()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalFinished );
			return *m_signalFinished;
		}

		castor::Size const & getSize()const
		{
			return m_size;
		}

		bool isSizeChanged()const
		{
			return m_sizeChanged;
		}
		/**@}*/

	private:
		struct Pipeline
		{
			renderer::DescriptorSetLayoutPtr descriptorLayout;
			renderer::DescriptorSetPoolPtr descriptorPool;
			renderer::PipelineLayoutPtr pipelineLayout;
			renderer::PipelinePtr pipeline;
		};

		struct OverlayRenderNode
		{
			Pipeline & m_pipeline;
			Pass & m_pass;
			OverlayUbo & m_overlayUbo;
			renderer::DescriptorSetPtr m_descriptorSet;
		};

		struct OverlayGeometryBuffers
		{
			GeometryBuffers m_noTexture;
			GeometryBuffers m_textured;
		};

		OverlayRenderNode & doGetPanelNode( Pass & pass );
		OverlayRenderNode & doGetTextNode( Pass & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		Pipeline & doGetPipeline( TextureChannels textureFlags
			, std::map< uint32_t, Pipeline > & pipelines );
		OverlayGeometryBuffers & doCreateTextGeometryBuffers();
		void doDrawItem( renderer::CommandBuffer const & commandBuffer
			, Material & material
			, renderer::VertexBufferBase const & vertexBuffer
			, OverlayGeometryBuffers const & geometryBuffers
			, uint32_t count );
		void doDrawItem( renderer::CommandBuffer const & commandBuffer
			, Pass & pass
			, renderer::VertexBufferBase const & vertexBuffer
			, uint32_t count );
		void doDrawItem( renderer::CommandBuffer const & commandBuffer
			, Pass & pass
			, renderer::VertexBufferBase const & vertexBuffer
			, TextureLayout const & texture
			, Sampler const & sampler
			, uint32_t count );
		OverlayGeometryBuffers & doFillTextPart( int32_t count
			, TextOverlay::VertexArray::const_iterator & it
			, uint32_t index );
		renderer::ShaderStageStateArray doCreateOverlayProgram( TextureChannels const & textureFlags );
		renderer::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureChannels textureFlags
			, Pass const & pass );
		renderer::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureChannels textureFlags
			, Pass const & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		void doCreateRenderPass();

	private:
		renderer::TextureView const & m_target;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_signalFinished;
		renderer::VertexBufferPtr< OverlayCategory::Vertex > m_panelVertexBuffer;
		renderer::VertexBufferPtr< OverlayCategory::Vertex > m_borderVertexBuffer;
		std::vector< renderer::VertexBufferPtr< TextOverlay::Vertex > > m_textsVertexBuffers;
		OverlayGeometryBuffers m_panelGeometryBuffers;
		OverlayGeometryBuffers m_borderGeometryBuffers;
		std::list< OverlayGeometryBuffers > m_textsGeometryBuffers;
		renderer::VertexLayoutPtr m_declaration;
		renderer::VertexLayoutPtr m_textDeclaration;
		castor::Size m_size;
		std::map< Pass *, OverlayRenderNode > m_mapPanelNodes;
		std::map< Pass *, OverlayRenderNode > m_mapTextNodes;
		renderer::RenderPassPtr m_renderPass;
		std::map< uint32_t, Pipeline > m_panelPipelines;
		std::map< uint32_t, Pipeline > m_textPipelines;
		int m_previousBorderZIndex{ 0 };
		int m_previousPanelZIndex{ 0 };
		int m_previousTextZIndex{ 0 };
		castor::String m_previousCaption;
		std::array< OverlayCategory::Vertex, 6u > m_panelVertex;
		std::array< OverlayCategory::Vertex, 8u * 6u > m_borderVertex;
		bool m_sizeChanged{ true };
		MatrixUbo m_matrixUbo;
		OverlayUbo m_overlayUbo;
	};
}

#endif
