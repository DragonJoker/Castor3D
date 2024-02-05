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
			, VkFramebuffer framebuffer
			, crg::Fence & fence );
		C3D_API OverlayPreparer( OverlayPreparer && rhs )noexcept;
		C3D_API OverlayPreparer & operator=( OverlayPreparer && rhs )noexcept;
		C3D_API ~OverlayPreparer()noexcept;

		C3D_API OverlaysCounts registerOverlay( Overlay const & overlay );
		C3D_API void fillDrawData()noexcept;

		void setDrawCounts( uint32_t & value )noexcept
		{
			m_drawCounts = &value;
		}

	private:
		void doRegisterDrawCommands( OverlayDrawPipeline const & pipeline
			, ashes::DescriptorSetCRefArray const & descriptorSets
			, ashes::BufferBase const & indirectCommands
			, uint32_t drawCount
			, uint32_t & offset
			, ashes::CommandBuffer const & commandBuffer )noexcept;
		void doRegisterDrawCommands( OverlayDrawPipeline const & pipeline
			, VkDrawIndirectCommand const & command
			, uint32_t drawId
			, uint32_t & offset
			, ashes::CommandBuffer const & commandBuffer )noexcept;
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
		crg::Fence & m_fence;
		using OverlayDataArray = castor::Vector< OverlayDrawData >;
		using OverlayDatasMap = castor::Map< OverlayPipelineData *, OverlayDataArray >;
		castor::Map< uint32_t, OverlayDatasMap > m_levelsOverlays;
		VkRenderPass m_renderPass;
		VkFramebuffer m_framebuffer;
		castor::Vector< OverlayDrawData > m_overlays;
		castor::Map< size_t, uint32_t > m_descriptorsCounts{};
		uint32_t * m_drawCounts{};
	};
}

#include "OverlayPreparer.inl"

#endif
