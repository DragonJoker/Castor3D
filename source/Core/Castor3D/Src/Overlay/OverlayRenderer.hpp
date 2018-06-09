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
#include <Miscellaneous/Hash.hpp>

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
	private:
		class Preparer
			: public OverlayVisitor
		{
		public:
			explicit Preparer( OverlayRenderer & renderer );

			void visit( PanelOverlay const & overlay )override;
			void visit( BorderPanelOverlay const & overlay )override;
			void visit( TextOverlay const & overlay )override;

		private:
			template< typename QuadT, typename OverlayT, typename BufferIndexT, typename BufferPoolT, typename VertexT >
			void doPrepareOverlay( OverlayT const & overlay
				, Pass const & pass
				, std::map< size_t, BufferIndexT > & overlays
				, std::vector< BufferPoolT > & vertexBuffers
				, std::vector < VertexT > const & vertices
				, FontTextureSPtr fontTexture );

		private:
			OverlayRenderer & m_renderer;
		};

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
		 *\brief		Begins the overlays preparation.
		 *\param[in]	viewport	The render window viewport.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	viewport	Le viewport de la fenêtre de rendu.
		 */
		C3D_API void beginPrepare( Viewport const & viewport
			, RenderPassTimer const & timer
			, renderer::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 */
		C3D_API void endPrepare( RenderPassTimer const & timer );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 */
		C3D_API void render( RenderPassTimer & timer );
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

		castor::Size const & getSize()const
		{
			return m_size;
		}

		bool isSizeChanged()const
		{
			return m_sizeChanged;
		}

		Preparer getPreparer()
		{
			return Preparer{ *this };
		}

		renderer::Semaphore const & getSemaphore()
		{
			return *m_finished;
		}
		/**@}*/

	private:
		struct Configuration
		{
			castor::Point2f position;
			castor::Point2i renderSize;
			castor::Point2f renderRatio;
			int32_t materialIndex;
		};

		struct Pipeline
		{
			renderer::DescriptorSetLayoutPtr descriptorLayout;
			renderer::DescriptorSetPoolPtr descriptorPool;
			renderer::PipelineLayoutPtr pipelineLayout;
			renderer::PipelinePtr pipeline;
		};

		struct OverlayRenderNode
		{
			Pipeline & pipeline;
			Pass const & pass;
		};

		struct OverlayGeometryBuffers
		{
			GeometryBuffers noTexture;
			GeometryBuffers textured;
		};

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferIndex;

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferPool
		{
			using MyBufferIndex = VertexBufferIndex< VertexT, CountT >;
			using Quad = std::array< VertexT, CountT >;

			VertexBufferPool( renderer::Device const & device
				, renderer::VertexLayout const & declaration
				, uint32_t count );
			VertexBufferIndex< VertexT, CountT > allocate( OverlayRenderNode & node );
			void deallocate( VertexBufferIndex< VertexT, CountT > const & index );
			void upload();

			uint32_t const maxCount;
			std::vector< Quad > data;
			renderer::VertexLayout const & declaration;
			renderer::VertexBufferPtr< Quad > buffer;
			std::set< uint32_t > free;
			renderer::UniformBufferPtr< Configuration > ubo;
		};

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferIndex
		{
			operator bool()const
			{
				return index != ~( 0u );
			}

			VertexBufferPool< VertexT, CountT > & pool;
			OverlayRenderNode & node;
			uint32_t index;
			OverlayGeometryBuffers geometryBuffers;
			renderer::DescriptorSetPtr descriptorSet;
			FontTexture::OnChanged::connection connection;
		};

		using PanelVertexBufferPool = VertexBufferPool< OverlayCategory::Vertex, 6u >;
		using PanelVertexBufferIndex = PanelVertexBufferPool::MyBufferIndex;
		using BorderPanelVertexBufferPool = VertexBufferPool< OverlayCategory::Vertex, 8u * 6u >;
		using BorderPanelVertexBufferIndex = BorderPanelVertexBufferPool::MyBufferIndex;
		using TextVertexBufferPool = VertexBufferPool< TextOverlay::Vertex, 600u >;
		using TextVertexBufferIndex = TextVertexBufferPool::MyBufferIndex;

		OverlayRenderNode & doGetPanelNode( Pass const & pass );
		OverlayRenderNode & doGetTextNode( Pass const & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		Pipeline & doGetPipeline( TextureChannels textureFlags
			, std::map< uint32_t, Pipeline > & pipelines );
		renderer::ShaderStageStateArray doCreateOverlayProgram( TextureChannels const & textureFlags );
		renderer::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureChannels textureFlags
			, Pass const & pass
			, renderer::UniformBuffer< Configuration > const & ubo
			, uint32_t index
			, bool update = true );
		renderer::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureChannels textureFlags
			, Pass const & pass
			, renderer::UniformBuffer< Configuration > const & ubo
			, uint32_t index
			, TextureLayout const & texture
			, Sampler const & sampler );
		void doCreateRenderPass();

		template< typename VertexBufferIndexT, typename VertexBufferPoolT >
		VertexBufferIndexT & doGetVertexBuffer( std::vector< std::unique_ptr< VertexBufferPoolT > > & pools
			, std::map< size_t, VertexBufferIndexT > & overlays
			, Overlay const & overlay
			, Pass const & pass
			, OverlayRenderNode & node
			, renderer::Device const & device
			, renderer::VertexLayout const & layout
			, uint32_t maxCount )
		{
			auto hash = std::hash< Overlay const * >{}( &overlay );
			hash = castor::hashCombine( hash, pass );
			auto it = overlays.find( hash );

			if ( it == overlays.end() )
			{
				for ( auto & pool : pools )
				{
					if ( it == overlays.end() )
					{
						auto result = pool->allocate( node );

						if ( bool( result ) )
						{
							it = overlays.emplace( hash, std::move( result ) ).first;
						}
					}
				}

				if ( it == overlays.end() )
				{
					pools.emplace_back( std::make_unique< VertexBufferPoolT >( device, layout, maxCount ) );
					auto result = pools.back()->allocate( node );
					it = overlays.emplace( hash, std::move( result ) ).first;
				}
			}

			return it->second;
		}

	private:
		renderer::TextureView const & m_target;
		renderer::CommandBufferPtr m_commandBuffer;
		std::vector< std::unique_ptr< PanelVertexBufferPool > > m_panelVertexBuffers;
		std::vector< std::unique_ptr< BorderPanelVertexBufferPool > > m_borderVertexBuffers;
		std::vector< std::unique_ptr< TextVertexBufferPool > > m_textVertexBuffers;
		std::map< size_t, PanelVertexBufferIndex > m_panelOverlays;
		std::map< size_t, BorderPanelVertexBufferIndex > m_borderPanelOverlays;
		std::map< size_t, TextVertexBufferIndex > m_textOverlays;
		renderer::VertexLayoutPtr m_declaration;
		renderer::VertexLayoutPtr m_textDeclaration;
		castor::Size m_size;
		std::map< Pass const *, OverlayRenderNode > m_mapPanelNodes;
		std::map< Pass const *, OverlayRenderNode > m_mapTextNodes;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		std::map< uint32_t, Pipeline > m_panelPipelines;
		std::map< uint32_t, Pipeline > m_textPipelines;
		int m_previousBorderZIndex{ 0 };
		int m_previousPanelZIndex{ 0 };
		int m_previousTextZIndex{ 0 };
		castor::String m_previousCaption;
		bool m_sizeChanged{ true };
		MatrixUbo m_matrixUbo;
		renderer::Semaphore const * m_toWait{ nullptr };
		renderer::SemaphorePtr m_finished;
		renderer::FencePtr m_fence;
	};
}

#endif
