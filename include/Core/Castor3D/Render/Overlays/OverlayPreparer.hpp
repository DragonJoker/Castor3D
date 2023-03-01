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
		: public OverlayVisitor
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
		C3D_API ~OverlayPreparer()noexcept override;

		void visit( PanelOverlay const & overlay )override;
		void visit( BorderPanelOverlay const & overlay )override;
		void visit( TextOverlay const & overlay )override;

	private:
		struct OverlayData
		{
			Overlay const * overlay{};
			ashes::DescriptorSetCRefArray descriptorSets{};
			OverlayGeometryBuffers geometryBuffers{};
			uint32_t index{};
			OverlayPipeline const * pipeline{};
		};

		template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
		void doPrepareOverlayDescriptors( RenderDevice const & device
			, OverlayT const & overlay
			, Pass const & pass
			, OverlayVertexBufferPoolT< VertexT, CountT > & vertexBuffer
			, FontTextureSPtr fontTexture
			, bool secondary );
		void doPrepareOverlayCommands( OverlayData const & overlay
			, ashes::CommandBuffer & commandBuffer );

	private:
		OverlayRenderer & m_renderer;
		RenderDevice const & m_device;
		VkRenderPass m_renderPass;
		VkFramebuffer m_framebuffer;
		std::vector< OverlayData > m_overlays;
	};
}

#include "OverlayPreparer.inl"

#endif
