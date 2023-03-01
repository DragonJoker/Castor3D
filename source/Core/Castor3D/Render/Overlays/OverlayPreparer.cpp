/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"

#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	namespace ovrlprep
	{
		castor::Rectangle getBorderSize( Overlay const & overlay, castor::Size const & size )
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
		size->x = std::max( 1u, size->x );
		size->y = std::max( 1u, size->y );
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
}
