/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"

#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	namespace ovrlprep
	{
		static castor::Rectangle getBorderSize( Overlay const & overlay, castor::Size const & size )
		{
			castor::Rectangle result{};

			if ( overlay.getType() == OverlayType::eBorderPanel )
			{
				if ( auto borderPanel = overlay.getBorderPanelOverlay() )
				{
					result = borderPanel->getAbsoluteBorderSize( size );

					switch ( borderPanel->getBorderPosition() )
					{
					case BorderPosition::eMiddle:
						result.set( result.left() / 2
							, result.top() / 2
							, result.right() / 2
							, result.bottom() / 2 );
						break;
					case BorderPosition::eExternal:
						break;
					default:
						result = castor::Rectangle{};
						break;
					}
				}
			}

			return result;
		}

		static castor::Point2d updateUbo( OverlayUboConfiguration & data
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & renderSize )
		{
			auto size = overlay.getAbsoluteSize( renderSize );
			auto ratio = overlay.getRenderRatio( renderSize );
			data.size = castor::Point2f{ size.getWidth(), size.getHeight() };
			data.position = castor::Point2f{ overlay.getAbsolutePosition() };
			data.size = castor::Point2f{ castor::Point2d{ data.size } *ratio };
			data.uv = castor::Point4f{ overlay.getUV() };
			data.materialId = pass.getId();
			return ratio;
		}
	}

	OverlayPreparer::OverlayPreparer( OverlayRenderer & renderer
		, RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
		: m_renderer{ renderer }
		, m_device{ device }
		, m_renderPass{ renderPass }
		, m_framebuffer{ framebuffer }
	{
		m_renderer.m_computePanelPipeline.count = 0u;
		m_renderer.m_computeBorderPipeline.count = 0u;
	}

	OverlayPreparer::OverlayPreparer( OverlayPreparer && rhs )noexcept
		: m_renderer{ rhs.m_renderer }
		, m_device{ rhs.m_device }
		, m_renderPass{ rhs.m_renderPass }
		, m_framebuffer{ rhs.m_framebuffer }
	{
		rhs.m_renderPass = VkRenderPass{};
		rhs.m_framebuffer = VkFramebuffer{};
	}

	OverlayPreparer & OverlayPreparer::operator=( OverlayPreparer && rhs )noexcept
	{
		m_renderPass = rhs.m_renderPass;

		rhs.m_renderPass = VkRenderPass{};
		rhs.m_framebuffer = VkFramebuffer{};

		return *this;
	}

	OverlayPreparer::~OverlayPreparer()noexcept
	{
		if ( m_renderPass )
		{
			m_renderer.doBeginPrepare( m_renderPass, m_framebuffer );
			ashes::CommandBuffer & commandBuffer = *m_renderer.m_commands.commandBuffer;

			for ( auto overlayIt : m_overlays )
			{
				doPrepareOverlayCommands( overlayIt, commandBuffer );
			}

			m_renderer.doEndPrepare();
		}
	}

	void OverlayPreparer::visit( PanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlayDescriptors< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_panelVertexBuffer
						, nullptr
						, false );
					++m_renderer.m_computePanelPipeline.count;
				}
			}
		}
	}

	void OverlayPreparer::visit( BorderPanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlayDescriptors< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_panelVertexBuffer
						, nullptr
						, false );
					++m_renderer.m_computePanelPipeline.count;
				}
			}
		}

		if ( auto material = overlay.getBorderMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlayDescriptors< OverlayRenderer::BorderPanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_borderVertexBuffer
						, nullptr
						, true );
					++m_renderer.m_computeBorderPipeline.count;
				}
			}
		}
	}

	void OverlayPreparer::visit( TextOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlayDescriptors< OverlayRenderer::TextVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_textVertexBuffer
						, overlay.getFontTexture()
						, false );
				}
			}
		}
	}

	void OverlayPreparer::doPrepareOverlayCommands( OverlayData const & data
		, ashes::CommandBuffer & commandBuffer )
	{
		auto & overlay = *data.overlay;
		auto borderSize = ovrlprep::getBorderSize( overlay, m_renderer.m_size );
		auto borderOffset = castor::Size{ uint32_t( borderSize.left() ), uint32_t( borderSize.top() ) };
		auto borderExtent = borderOffset + castor::Size{ uint32_t( borderSize.right() ), uint32_t( borderSize.bottom() ) };
		auto position = overlay.getAbsolutePosition( m_renderer.m_size ) - borderOffset;
		position->x = std::max( 0, position->x );
		position->y = std::max( 0, position->y );
		auto size = overlay.getAbsoluteSize( m_renderer.m_size ) + borderExtent;
		size->x = std::min( std::max( 1u, size->x ), m_renderer.m_size[0] - position->x );
		size->y = std::min( std::max( 1u, size->y ), m_renderer.m_size[1] - position->y );
		commandBuffer.bindPipeline( *data.pipeline->pipeline );
		commandBuffer.setViewport( makeViewport( m_renderer.m_size ) );
		commandBuffer.setScissor( makeScissor( position, size ) );
		commandBuffer.bindDescriptorSets( data.descriptorSets, *data.pipeline->pipelineLayout );
		DrawConstants constants{ data.index };
		commandBuffer.pushConstants( *data.pipeline->pipelineLayout
			, VK_SHADER_STAGE_VERTEX_BIT
			, 0u
			, sizeof( constants )
			, &constants );
		commandBuffer.bindVertexBuffer( 0u
			, data.geometryBuffers.buffer->getBuffer().getBuffer()
			, data.geometryBuffers.offset );
		commandBuffer.draw( data.geometryBuffers.count
			, 1u
			, 0u
			, 0u );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, PanelOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize )const
	{
		ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, BorderPanelOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize )const
	{
		auto ratio = ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize );
		auto border = overlay.getAbsoluteBorderSize( renderSize );
		data.border = castor::Point4f{ border.left(), border.top(), border.right(), border.bottom() };
		data.border = castor::Point4f{ castor::Point4d{ data.border } *castor::Point4d{ ratio->x, ratio->y, ratio->x, ratio->y } };
		data.borderInnerUV = castor::Point4f{ overlay.getBorderInnerUV() };
		data.borderOuterUV = castor::Point4f{ overlay.getBorderOuterUV() };
		data.borderPosition = uint32_t( overlay.getBorderPosition() );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, TextOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize )const
	{
		ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize );
	}
}
