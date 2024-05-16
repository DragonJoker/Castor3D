/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayRenderer_H___
#define ___C3D_OverlayRenderer_H___

#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayVertexBufferPool.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class OverlayRenderer
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		friend class OverlayPreparer;

		struct ComputePipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::PipelinePtr pipeline{};
			ashes::DescriptorSetPoolPtr descriptorPool{};
			ashes::DescriptorSetPtr descriptorSet{};
			uint32_t count{};
		};

		struct TextComputePipelineDescriptor
		{
			TextComputePipelineDescriptor( ashes::DescriptorSetPtr descriptorSet
				, uint32_t count
				, OverlayTextBuffer const * textBuffer = {} )
				: descriptorSet{ castor::move( descriptorSet ) }
				, count{ count }
				, textBuffer{ textBuffer }
			{
			}

			ashes::DescriptorSetPtr descriptorSet;
			uint32_t count{};
			OverlayTextBuffer const * textBuffer{};
		};

		struct TextComputePipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::PipelinePtr pipeline{};
			ashes::DescriptorSetPoolPtr descriptorPool{};
			castor::UnorderedMap< FontTexture const *, TextComputePipelineDescriptor > sets;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	target			The target texture.
		 *\param[in]	hdrConfigUbo	The HDR configuration used when rendering to an HDR screen.
		 *\param[in]	timer			The timer for this pass.
		 *\param[in]	level			The command buffer level.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	target			La texture cible.
		 *\param[in]	hdrConfigUbo	La configuration HDR lors du rendu sur un écran HDR.
		 *\param[in]	timer			Le timer pour cette passe.
		 *\param[in]	level			Le niveau du command buffer.
		 */
		C3D_API OverlayRenderer( RenderDevice const & device
			, Texture const & target
			, HdrConfigUbo const & hdrConfigUbo
			, crg::FramePassTimer & timer
			, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );
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
		 *\param[in]	device		The render device.
		 *\param[in]	renderPass	The render pass.
		 *\param[in]	framebuffer	The framebuffer receiving the result.
		 *\param[in]	fence		The fence to wait for before rendering.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	device		Le périphérique de rendu.
		 *\param[in]	renderPass	La passe de rendu.
		 *\param[in]	framebuffer	Le framebuffer recevant le résultat.
		 *\param[in]	fence		La fence à attendre avant le rendu.
		 */
		C3D_API OverlayPreparer beginPrepare( RenderDevice const & device
			, VkRenderPass renderPass
			, VkFramebuffer framebuffer
			, crg::Fence & fence );
		/**
		 *\~english
		 *\brief			Uploads all GPU buffers to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les tampons GPU en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Records compute commands.
		 *\param[in]	context			The recording context.
		 *\param[in]	commandBuffer	The command buffer on which commands are recorded.
		 *\~french
		 *\brief		Enregistre les commandes de compute.
		 *\param[in]	context			Le contexte d'enregistrement.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes sont enregistrées.
		 */
		C3D_API void registerComputeCommands( crg::RecordContext & context
			, VkCommandBuffer commandBuffer )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		ashes::CommandBuffer const & getCommands()const noexcept
		{
			CU_Require( m_draw.commands.commandBuffer );
			return *m_draw.commands.commandBuffer;
		}

		castor::Size const & getSize()const noexcept
		{
			return m_size;
		}

		bool isSizeChanged()const noexcept
		{
			return m_sizeChanged;
		}
		/**@}*/

	public:
		struct FontTextureDescriptorConnection
		{
			FontTextureDescriptorConnection() = default;
			ashes::DescriptorSetPtr descriptorSet{};
			FontTexture::OnChanged::connection connection{};
		};

		using PanelVertexBufferPool = OverlayVertexBufferPoolT< OverlayCategory::Vertex, 6u >;
		using BorderPanelVertexBufferPool = OverlayVertexBufferPoolT< OverlayCategory::Vertex, 8u * 6u >;
		using TextVertexBufferPool = OverlayVertexBufferPoolT< TextOverlay::Vertex, MaxCharsPerOverlay >;

		struct OverlaysCommonData
		{
			explicit OverlaysCommonData( RenderDevice const & device
				, HdrConfigUbo const & hdrConfigUbo );

			ashes::DescriptorSetLayoutPtr baseDescriptorLayout;

			CameraUbo cameraUbo;
			castor::RawUniquePtr< PanelVertexBufferPool > panelVertexBuffer;
			castor::RawUniquePtr< BorderPanelVertexBufferPool > borderVertexBuffer;
			castor::RawUniquePtr< TextVertexBufferPool > textVertexBuffer;
		};

		struct OverlaysComputeData
		{
			ComputePipeline panelPipeline;
			ComputePipeline borderPipeline;
			TextComputePipeline textPipeline;

			OverlaysComputeData( RenderDevice const & device
				, OverlaysCommonData & commonData );

			void reset();
			void registerCommands( crg::RecordContext & context
				, VkCommandBuffer commandBuffer )const;

			TextComputePipelineDescriptor & getTextPipeline( FontTexture const & fontTexture );

		private:
			ComputePipeline doCreatePanelPipeline( RenderDevice const & device
				, PanelVertexBufferPool & vertexBuffer
				, CameraUbo const & cameraUbo )const;
			ComputePipeline doCreateBorderPipeline( RenderDevice const & device
				, BorderPanelVertexBufferPool & vertexBuffer
				, CameraUbo const & cameraUbo )const;
			TextComputePipeline doCreateTextPipeline( RenderDevice const & device )const;
			ashes::DescriptorSetPtr doGetTextDescriptorSet( FontTexture const & fontTexture );
			void doRegisterComputeBufferCommands( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, OverlayRenderer::ComputePipeline const & pipeline
				, ashes::BufferBase const & overlaysBuffer
				, ashes::BufferBase const & vertexBuffer )const;
			void doRegisterComputeBufferCommands( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, OverlayRenderer::TextComputePipeline const & pipeline
				, OverlayRenderer::TextComputePipelineDescriptor const & set )const;

		private:
			OverlaysCommonData & m_commonData;
		};

		struct OverlaysDrawData
		{
			CommandsSemaphore commands;
			castor::RawUniquePtr< crg::FramePassTimerBlock > timerBlock;
			castor::Vector< ashes::DescriptorSetPtr > retired;
			ashes::DescriptorSetLayoutPtr textDescriptorLayout;
			ashes::DescriptorSetPoolPtr textDescriptorPool;
			castor::UnorderedMap< FontTexture const *, FontTextureDescriptorConnection > textDescriptorSets;

			OverlaysDrawData( RenderDevice const & device
				, VkCommandBufferLevel level
				, OverlaysCommonData & commonData
				, bool isHdr );

			OverlayDrawNode & getPanelNode( RenderDevice const & device
				, VkRenderPass renderPass
				, Pass const & pass );
			OverlayDrawNode & getBorderNode( RenderDevice const & device
				, VkRenderPass renderPass
				, Pass const & pass );
			OverlayDrawNode & getTextNode( RenderDevice const & device
				, VkRenderPass renderPass
				, Pass const & pass
				, bool sdfFont );
			ashes::DescriptorSet const & createTextDescriptorSet( FontTexture & fontTexture );
			void beginPrepare( VkRenderPass renderPass
				, VkFramebuffer framebuffer
				, crg::Fence & fence
				, crg::FramePassTimer & timer
				, castor::Size const & size );
			void endPrepare();

		private:
			OverlayDrawPipeline & doGetPipeline( RenderDevice const & device
				, VkRenderPass renderPass
				, Pass const & pass
				, castor::UnorderedMap< size_t, OverlayDrawPipeline > & pipelines
				, bool borderOverlay
				, bool textOverlay
				, bool sdfFont );
			OverlayDrawPipeline doCreatePipeline( RenderDevice const & device
				, VkRenderPass renderPass
				, ashes::PipelineShaderStageCreateInfoArray program
				, TextureCombine const & texturesFlags
				, bool borderOverlay
				, bool textOverlay
				, bool sdfFont );
			ashes::PipelineShaderStageCreateInfoArray doCreateOverlayProgram( RenderDevice const & device
				, TextureCombine const & texturesFlags
				, bool textOverlay
				, bool sdfFont )const;

		private:
			castor::UnorderedMap< size_t, OverlayDrawNode > m_mapPanelNodes;
			castor::UnorderedMap< size_t, OverlayDrawNode > m_mapBorderNodes;
			castor::UnorderedMap< size_t, OverlayDrawNode > m_mapTextNodes;
			castor::UnorderedMap< size_t, OverlayDrawPipeline > m_panelPipelines;
			castor::UnorderedMap< size_t, OverlayDrawPipeline > m_borderPipelines;
			castor::UnorderedMap< size_t, OverlayDrawPipeline > m_textPipelines;
			OverlaysCommonData & m_commonData;
			bool m_isHdr;
		};

	private:
		ashes::CommandBuffer & doBeginPrepare( VkRenderPass renderPass
			, VkFramebuffer framebuffer
			, crg::Fence & fence );
		void doEndPrepare();
		castor::Pair< OverlayDrawNode *, OverlayPipelineData * > doGetDrawNodeData( RenderDevice const & device
			, VkRenderPass renderPass
			, Overlay const & overlay
			, Pass const & pass
			, bool secondary );

		void doResetCompute()noexcept
		{
			m_compute.reset();
		}

	private:
		Texture const & m_target;
		crg::FramePassTimer & m_timer;
		castor::Size m_size;
		castor::String m_previousCaption;
		bool m_sizeChanged{ true };
		OverlaysCommonData m_common;
		OverlaysDrawData m_draw;
		OverlaysComputeData m_compute;
	};
}

#endif
