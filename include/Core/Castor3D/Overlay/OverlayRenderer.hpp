/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayRenderer_H___
#define ___C3D_OverlayRenderer_H___

#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class OverlayRenderer
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		template< typename VertexT, uint32_t CountT >
		struct VertexBufferIndexT;

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferPoolT;
		template< typename VertexT, uint32_t CountT >
		using VertexBufferPoolPtrT = std::unique_ptr< VertexBufferPoolT< VertexT, CountT > >;

		class Preparer
			: public OverlayVisitor
		{
		public:
			explicit Preparer( OverlayRenderer & renderer
				, RenderDevice const & device );

			void visit( PanelOverlay const & overlay )override;
			void visit( BorderPanelOverlay const & overlay )override;
			void visit( TextOverlay const & overlay )override;

		private:
			template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
			void doPrepareOverlay( RenderDevice const & device
				, OverlayT const & overlay
				, Pass const & pass
				, std::map< size_t, VertexBufferIndexT< VertexT, CountT > > & overlays
				, std::vector< VertexBufferPoolPtrT< VertexT, CountT > > & vertexBuffers
				, std::vector < VertexT > const & vertices
				, FontTextureSPtr fontTexture );

		private:
			OverlayRenderer & m_renderer;
			RenderDevice const & m_device;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	uboPools	The UBO pools.
		 *\param[in]	target		The target texture view.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	uboPools	Les pools d'UBO.
		 *\param[in]	target		La vue de texture cible.
		 */
		C3D_API OverlayRenderer( RenderDevice const & device
			, Texture const & target
			, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~OverlayRenderer();
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Begins the overlays preparation.
		 *\param[in]	timer	The render pass timer.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API void beginPrepare( FramePassTimer const & timer );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\param[in]	timer	The render pass timer.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 */
		C3D_API void endPrepare( FramePassTimer const & timer );
		/**
		 *\~english
		 *\brief		Begins the overlays preparation.
		 *\param[in]	timer	The render pass timer.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API void beginPrepare( VkRenderPass renderPass
			, VkFramebuffer framebuffer );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\param[in]	timer	The render pass timer.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 */
		C3D_API void endPrepare();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\param[in]	device	The GPU device.
		 *\param[in]	timer	The render pass timer.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 */
		C3D_API crg::SemaphoreWaitArray render( FramePassTimer & timer
			, ashes::Queue const & queue
			, crg::SemaphoreWaitArray const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		ashes::CommandBuffer const & getCommands()const
		{
			CU_Require( m_commands.commandBuffer );
			return *m_commands.commandBuffer;
		}

		castor::Size const & getSize()const
		{
			return m_size;
		}

		bool isSizeChanged()const
		{
			return m_sizeChanged;
		}

		Preparer getPreparer( RenderDevice const & device )
		{
			return Preparer{ *this, device };
		}

		ashes::Semaphore const & getSemaphore()
		{
			return *m_commands.semaphore;
		}
		/**@}*/

	private:
		struct Configuration
		{
			castor::Point4f positionRatio;
			castor::Point4i renderSizeIndex;
		};

		struct Pipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout;
			ashes::DescriptorSetPoolPtr descriptorPool;
			ashes::PipelineLayoutPtr pipelineLayout;
			ashes::GraphicsPipelinePtr pipeline;
		};

	public:
		struct OverlayRenderNode
		{
			Pipeline & pipeline;
			Pass const & pass;
		};

	public:
		struct OverlayGeometryBuffers
		{
			GeometryBuffers noTexture;
			GeometryBuffers textured;
		};

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferIndexT;

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferPoolT
		{
			using MyBufferIndex = VertexBufferIndexT< VertexT, CountT >;
			using Quad = std::array< VertexT, CountT >;

			VertexBufferPoolT( Engine & engine
				, std::string const & debugName
				, UniformBufferPools & uboPools
				, RenderDevice const & device
				, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
				, ashes::PipelineVertexInputStateCreateInfo const & texDecl
				, uint32_t count );
			VertexBufferIndexT< VertexT, CountT > allocate( OverlayRenderNode & node );
			void deallocate( VertexBufferIndexT< VertexT, CountT > const & index );
			void upload( ashes::CommandBuffer const & cb );

			Engine & engine;
			RenderDevice const & device;
			UniformBufferPools & uboPools;
			ashes::PipelineVertexInputStateCreateInfo const & noTexDeclaration;
			ashes::PipelineVertexInputStateCreateInfo const & texDeclaration;
			VertexBufferPoolUPtr buffer;
			std::vector< ObjectBufferOffset > allocated;
		};

		template< typename VertexT, uint32_t CountT >
		struct VertexBufferIndexT
		{
			operator bool()const
			{
				return index != InvalidIndex;
			}

			VertexBufferPoolT< VertexT, CountT > & pool;
			OverlayRenderNode & node;
			uint32_t index;
			UniformBufferOffsetT< Configuration > overlayUbo{};
			OverlayGeometryBuffers geometryBuffers{};
			ashes::DescriptorSetPtr descriptorSet{};
			FontTexture::OnChanged::connection connection{};
		};

		using PanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 6u >;
		using PanelVertexBufferIndex = PanelVertexBufferPool::MyBufferIndex;
		using BorderPanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 8u * 6u >;
		using BorderPanelVertexBufferIndex = BorderPanelVertexBufferPool::MyBufferIndex;
		using TextVertexBufferPool = VertexBufferPoolT< TextOverlay::Vertex, 600u >;
		using TextVertexBufferIndex = TextVertexBufferPool::MyBufferIndex;

	private:
		OverlayRenderNode & doGetPanelNode( RenderDevice const & device
			, Pass const & pass );
		OverlayRenderNode & doGetTextNode( RenderDevice const & device
			, Pass const & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		Pipeline doCreatePipeline( RenderDevice const & device
			, Pass const & pass
			, ashes::PipelineShaderStageCreateInfoArray program
			, FilteredTextureFlags const & texturesFlags
			, bool text );
		Pipeline & doGetPipeline( RenderDevice const & device
			, Pass const & pass
			, std::map< uint32_t, Pipeline > & pipelines
			, bool text );
		ashes::PipelineShaderStageCreateInfoArray doCreateOverlayProgram( RenderDevice const & device
			, FilteredTextureFlags const & textures
			, bool text );
		ashes::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureFlags textures
			, Pass const & pass
			, UniformBufferOffsetT< Configuration > const & overlayUbo
			, uint32_t index
			, bool update = true );
		ashes::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, TextureFlags textures
			, Pass const & pass
			, UniformBufferOffsetT< Configuration > const & overlayUbo
			, uint32_t index
			, TextureLayout const & texture
			, Sampler const & sampler );
		void doCreateRenderPass( RenderDevice const & device );

	private:
		UniformBufferPools & m_uboPools;
		Texture const & m_target;
		CommandsSemaphore m_commands;
		ashes::FencePtr m_fence;
		std::vector< std::unique_ptr< PanelVertexBufferPool > > m_panelVertexBuffers;
		std::vector< std::unique_ptr< BorderPanelVertexBufferPool > > m_borderVertexBuffers;
		std::vector< std::unique_ptr< TextVertexBufferPool > > m_textVertexBuffers;
		std::map< size_t, PanelVertexBufferIndex > m_panelOverlays;
		std::map< size_t, BorderPanelVertexBufferIndex > m_borderPanelOverlays;
		std::map< size_t, TextVertexBufferIndex > m_textOverlays;
		ashes::PipelineVertexInputStateCreateInfo m_noTexDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_texDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_noTexTextDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_texTextDeclaration;
		castor::Size m_size;
		std::map< Pass const *, OverlayRenderNode > m_mapPanelNodes;
		std::map< Pass const *, OverlayRenderNode > m_mapTextNodes;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		std::map< uint32_t, Pipeline > m_panelPipelines;
		std::map< uint32_t, Pipeline > m_textPipelines;
		castor::String m_previousCaption;
		bool m_sizeChanged{ true };
		MatrixUbo m_matrixUbo;
	};
}

#endif
