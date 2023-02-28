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
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/FramePassTimer.hpp>

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
			C3D_API Preparer( Preparer const & ) = delete;
			C3D_API Preparer & operator=( Preparer const & ) = delete;
			C3D_API explicit Preparer( OverlayRenderer & renderer
				, RenderDevice const & device
				, VkRenderPass renderPass
				, VkFramebuffer framebuffer );
			C3D_API Preparer( Preparer && rhs );
			C3D_API Preparer & operator=( Preparer && rhs );
			C3D_API ~Preparer()override;

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

		friend class Preparer;

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
		C3D_API Preparer beginPrepare( RenderDevice const & device
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

	private:
		struct Configuration
		{
			castor::Point4f positionRatio;
			castor::Point4i renderSizeIndex;
		};

		struct Pipeline
		{
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

		struct FontTextureDescriptorConnection
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
				, RenderDevice const & device
				, CameraUbo const & cameraUbo
				, ashes::DescriptorSetLayout const & descriptorLayout
				, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
				, ashes::PipelineVertexInputStateCreateInfo const & texDecl
				, uint32_t count );
			VertexBufferIndexT< VertexT, CountT > allocate( OverlayRenderNode & node );
			void deallocate( VertexBufferIndexT< VertexT, CountT > const & index );
			void upload( ashes::CommandBuffer const & cb );

			Engine & engine;
			RenderDevice const & device;
			ashes::BufferPtr< Configuration > overlaysData;
			castor::ArrayView< Configuration > overlaysBuffer;
			ashes::PipelineVertexInputStateCreateInfo const & noTexDeclaration;
			ashes::PipelineVertexInputStateCreateInfo const & texDeclaration;
			VertexBufferPoolUPtr buffer;
			std::vector< ObjectBufferOffset > allocated;
			ashes::DescriptorSetPoolPtr descriptorPool;
			ashes::DescriptorSetPtr descriptorSet;

		private:
			ashes::DescriptorSetPtr doCreateDescriptorSet( CameraUbo const & cameraUbo
				, ashes::DescriptorSetLayout const & descriptorLayout
				, std::string const & debugName )const;
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
			OverlayGeometryBuffers geometryBuffers{};
		};

		using PanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 6u >;
		using PanelVertexBufferIndex = PanelVertexBufferPool::MyBufferIndex;
		using BorderPanelVertexBufferPool = VertexBufferPoolT< OverlayCategory::Vertex, 8u * 6u >;
		using BorderPanelVertexBufferIndex = BorderPanelVertexBufferPool::MyBufferIndex;
		using TextVertexBufferPool = VertexBufferPoolT< TextOverlay::Vertex, MaxOverlayCharsPerBuffer >;
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
		Pipeline doCreatePipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, ashes::PipelineShaderStageCreateInfoArray program
			, TextureCombine const & texturesFlags
			, bool text );
		Pipeline & doGetPipeline( RenderDevice const & device
			, VkRenderPass renderPass
			, Pass const & pass
			, std::map< uint32_t, Pipeline > & pipelines
			, bool text );
		ashes::PipelineShaderStageCreateInfoArray doCreateOverlayProgram( RenderDevice const & device
			, TextureCombine const & texturesFlags
			, bool text );
		ashes::DescriptorSet const & doCreateTextDescriptorSet( FontTexture & fontTexture );

	private:
		Texture const & m_target;
		crg::FramePassTimer & m_timer;
		std::unique_ptr< crg::FramePassTimerBlock > m_timerBlock;
		CommandsSemaphore m_commands;
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
		CameraUbo m_cameraUbo;
		std::vector< ashes::DescriptorSetPtr > m_retired;
		ashes::DescriptorSetLayoutPtr m_baseDescriptorLayout;
		ashes::DescriptorSetLayoutPtr m_textDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_textDescriptorPool;
		std::map< FontTexture const *, FontTextureDescriptorConnection > m_textDescriptorSets;
	};
}

#endif
