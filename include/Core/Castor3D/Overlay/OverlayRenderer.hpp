/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayRenderer_H___
#define ___C3D_OverlayRenderer_H___

#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Limits.hpp"
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
			C3D_API explicit Preparer( OverlayRenderer & renderer
				, RenderDevice const & device
				, VkRenderPass renderPass );

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
			VkRenderPass m_renderPass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device	The GPU device.
		 *\param[in]	target	The target texture.
		 *\param[in]	level	The command buffer level.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	target	La texture cible.
		 *\param[in]	level	Le niveau du command buffer.
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
		 *\param[in]	renderPass	The render pass.
		 *\param[in]	framebuffer	The framebuffer receiving the result.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	renderPass	La passe de rendu.
		 *\param[in]	framebuffer	Le framebuffer recevant le résultat.
		 */
		C3D_API void beginPrepare( VkRenderPass renderPass
			, VkFramebuffer framebuffer );
		/**
		 *\~english
		 *\brief		Ends the overlays preparation.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
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
		 *\param[in]	timer	The render pass timer.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Termine la préparation des incrustations.
		 *\param[in]	timer	Le timer de la passe de rendu.
		 *\param[in]	queue	La queue recevant les commandes d'initialisation.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 *\return		Les sémaphores signalés par ce dessin.
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

		Preparer getPreparer( RenderDevice const & device
			, VkRenderPass renderPass )
		{
			return Preparer{ *this, device, renderPass };
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

		struct OverlayDescriptorConnection
		{
			ashes::DescriptorSetPtr descriptorSet;
			FontTexture::OnChanged::connection connection{};
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
				, UniformBufferPool & uboPool
				, RenderDevice const & device
				, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
				, ashes::PipelineVertexInputStateCreateInfo const & texDecl
				, uint32_t count );
			VertexBufferIndexT< VertexT, CountT > allocate( OverlayRenderNode & node );
			void deallocate( VertexBufferIndexT< VertexT, CountT > const & index );
			void upload( ashes::CommandBuffer const & cb );

			Engine & engine;
			RenderDevice const & device;
			UniformBufferPool & uboPool;
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
			std::map< void *, OverlayDescriptorConnection > descriptorSets{};
		};

		using PanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 6u >;
		using PanelVertexBufferIndex = PanelVertexBufferPool::MyBufferIndex;
		using BorderPanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 8u * 6u >;
		using BorderPanelVertexBufferIndex = BorderPanelVertexBufferPool::MyBufferIndex;
		using TextVertexBufferPool = VertexBufferPoolT< TextOverlay::Vertex, MaxOverlayCharsPerBuffer >;
		using TextVertexBufferIndex = TextVertexBufferPool::MyBufferIndex;

	private:
		OverlayRenderNode & doGetPanelNode( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass );
		OverlayRenderNode & doGetTextNode( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		Pipeline doCreatePipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, ashes::PipelineShaderStageCreateInfoArray program
			, FilteredTextureFlags const & texturesFlags
			, bool text );
		Pipeline & doGetPipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, std::map< uint32_t, Pipeline > & pipelines
			, bool text );
		ashes::PipelineShaderStageCreateInfoArray doCreateOverlayProgram( RenderDevice const & device
			, FilteredTextureFlags const & textures
			, bool text );
		ashes::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, Pass const & pass
			, UniformBufferOffsetT< Configuration > const & overlayUbo
			, uint32_t index
			, bool update = true );
		ashes::DescriptorSetPtr doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
			, Pass const & pass
			, UniformBufferOffsetT< Configuration > const & overlayUbo
			, uint32_t index
			, TextureLayout const & texture
			, Sampler const & sampler );

	private:
		UniformBufferPool & m_uboPool;
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
		std::map< uint32_t, Pipeline > m_panelPipelines;
		std::map< uint32_t, Pipeline > m_textPipelines;
		castor::String m_previousCaption;
		bool m_sizeChanged{ true };
		MatrixUbo m_matrixUbo;
		std::vector< ashes::DescriptorSetPtr > m_retired;
	};
}

#endif
