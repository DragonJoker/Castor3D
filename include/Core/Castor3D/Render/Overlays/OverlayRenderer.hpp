/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayRenderer_H___
#define ___C3D_OverlayRenderer_H___

#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayVertexBufferIndex.hpp"
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
			, crg::FramePassTimer & timer
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
		 *\param[in]	device		The render device.
		 *\param[in]	renderPass	The render pass.
		 *\param[in]	framebuffer	The framebuffer receiving the result.
		 *\~french
		 *\brief		Commence la préparation des incrustations.
		 *\param[in]	device		Le périphérique de rendu.
		 *\param[in]	renderPass	La passe de rendu.
		 *\param[in]	framebuffer	Le framebuffer recevant le résultat.
		 */
		C3D_API OverlayPreparer beginPrepare( RenderDevice const & device
			, VkRenderPass renderPass
			, VkFramebuffer framebuffer );
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );
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
		/**@}*/

	public:
		struct FontTextureDescriptorConnection
		{
			ashes::DescriptorSetPtr descriptorSet;
			FontTexture::OnChanged::connection connection{};
		};

		struct ComputePipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout{};
			ashes::PipelineLayoutPtr pipelineLayout{};
			ashes::PipelinePtr pipeline{};
			ashes::DescriptorSetPoolPtr descriptorPool{};
			ashes::DescriptorSetPtr descriptorSet{};
			uint32_t count{};
		};

		using PanelVertexBufferPool = OverlayVertexBufferPoolT< OverlayCategory::Vertex, 6u >;
		using BorderPanelVertexBufferPool = OverlayVertexBufferPoolT< OverlayCategory::Vertex, 8u * 6u >;
		using TextVertexBufferPool = OverlayVertexBufferPoolT< TextOverlay::Vertex, MaxOverlayCharsPerBuffer >;
		using PanelVertexBufferIndex = PanelVertexBufferPool::MyBufferIndex;
		using BorderPanelVertexBufferIndex = BorderPanelVertexBufferPool::MyBufferIndex;
		using TextVertexBufferIndex = TextVertexBufferPool::MyBufferIndex;

	private:
		void doBeginPrepare( VkRenderPass renderPass
			, VkFramebuffer framebuffer );
		void doEndPrepare();
		OverlayRenderNode & doGetPanelNode( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass );
		OverlayRenderNode & doGetTextNode( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, TextureLayout const & texture
			, Sampler const & sampler );
		OverlayPipeline doCreatePipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, ashes::PipelineShaderStageCreateInfoArray program
			, TextureCombine const & texturesFlags
			, bool text );
		OverlayPipeline & doGetPipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, std::map< uint32_t, OverlayPipeline > & pipelines
			, bool text );
		ComputePipeline doCreatePanelPipeline( RenderDevice const & device );
		ashes::PipelineShaderStageCreateInfo doCreatePanelProgram( RenderDevice const & device );
		ComputePipeline doCreateBorderPipeline( RenderDevice const & device );
		ashes::PipelineShaderStageCreateInfo doCreateBorderProgram( RenderDevice const & device );
		ashes::PipelineShaderStageCreateInfoArray doCreateOverlayProgram( RenderDevice const & device
			, TextureCombine const & texturesFlags
			, bool text );
		ashes::DescriptorSet const & doCreateTextDescriptorSet( FontTexture & fontTexture );

	private:
		Texture const & m_target;
		crg::FramePassTimer & m_timer;
		std::unique_ptr< crg::FramePassTimerBlock > m_timerBlock;
		CommandsSemaphore m_commands;
		ashes::PipelineVertexInputStateCreateInfo m_noTexDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_texDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_noTexTextDeclaration;
		ashes::PipelineVertexInputStateCreateInfo m_texTextDeclaration;
		CameraUbo m_cameraUbo;
		ashes::DescriptorSetLayoutPtr m_baseDescriptorLayout;
		std::unique_ptr< PanelVertexBufferPool > m_panelVertexBuffer;
		std::unique_ptr< BorderPanelVertexBufferPool > m_borderVertexBuffer;
		std::unique_ptr< TextVertexBufferPool > m_textVertexBuffer;
		castor::Size m_size;
		std::map< Pass const *, OverlayRenderNode > m_mapPanelNodes;
		std::map< Pass const *, OverlayRenderNode > m_mapTextNodes;
		std::map< uint32_t, OverlayPipeline > m_panelPipelines;
		std::map< uint32_t, OverlayPipeline > m_textPipelines;
		castor::String m_previousCaption;
		bool m_sizeChanged{ true };
		std::vector< ashes::DescriptorSetPtr > m_retired;
		ashes::DescriptorSetLayoutPtr m_textDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_textDescriptorPool;
		std::map< FontTexture const *, FontTextureDescriptorConnection > m_textDescriptorSets;
		ComputePipeline m_computePanelPipeline;
		ComputePipeline m_computeBorderPipeline;
	};
}

#endif
