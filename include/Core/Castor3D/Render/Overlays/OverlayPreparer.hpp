/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayPreparer_H___
#define ___C3D_OverlayPreparer_H___

#include "Castor3D/Render/Overlays/OverlaysModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Overlay/OverlayVisitor.hpp"
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
	class OverlayPreparer
	{
	public:
		C3D_API OverlayPreparer( OverlayPreparer const & ) = delete;
		C3D_API OverlayPreparer & operator=( OverlayPreparer const & ) = delete;

		C3D_API OverlayPreparer( OverlayRenderer & renderer
			, RenderDevice const & device
			, VkRenderPass renderPass
			, VkFramebuffer framebuffer );
		C3D_API OverlayPreparer( OverlayPreparer && rhs )noexcept;
		C3D_API OverlayPreparer & operator=( OverlayPreparer && rhs )noexcept;
		C3D_API ~OverlayPreparer()noexcept;

		C3D_API void registerOverlay( Overlay const & overlay );
		C3D_API void fillDrawData();

	private:
		void doRegisterDrawCommands( OverlayDrawPipeline const & pipeline
			, ashes::DescriptorSetCRefArray const & descriptorSets
			, ashes::BufferBase const & indirectCommands
			, uint32_t drawCount
			, uint32_t & offset
			, ashes::CommandBuffer & commandBuffer );
		void doUpdateUbo( OverlayUboConfiguration & data
			, PanelOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize
			, uint32_t vertexOffset
			, OverlayTextBufferIndex const & textBuffer )const;
		void doUpdateUbo( OverlayUboConfiguration & data
			, BorderPanelOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize
			, uint32_t vertexOffset
			, OverlayTextBufferIndex const & textBuffer )const;
		void doUpdateUbo( OverlayUboConfiguration & data
			, TextOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize
			, uint32_t vertexOffset
			, OverlayTextBufferIndex const & textBuffer )const;

	private:
		OverlayRenderer & m_renderer;
		RenderDevice const & m_device;
		using OverlayDataArray = std::vector< OverlayDrawData >;
		using OverlayDatasMap = std::map< OverlayPipelineData *, OverlayDataArray >;
		std::map< uint32_t, OverlayDatasMap > m_levelsOverlays;
		VkRenderPass m_renderPass;
		VkFramebuffer m_framebuffer;
		std::vector< OverlayDrawData > m_overlays;
		std::map< size_t, uint32_t > m_descriptorsCounts{};
	};
}

#include "OverlayPreparer.inl"

#endif
