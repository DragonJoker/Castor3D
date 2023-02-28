/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"

#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	OverlayPreparer::OverlayPreparer( OverlayRenderer & renderer
		, RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
		: m_renderer{ renderer }
		, m_device{ device }
		, m_renderPass{ renderPass }
	{
		m_renderer.doBeginPrepare( m_renderPass, framebuffer );
	}

	OverlayPreparer::OverlayPreparer( OverlayPreparer && rhs )noexcept
		: m_renderer{ rhs.m_renderer }
		, m_device{ rhs.m_device }
		, m_renderPass{ rhs.m_renderPass }
	{
		rhs.m_renderPass = VkRenderPass{};
	}

	OverlayPreparer & OverlayPreparer::operator=( OverlayPreparer && rhs )noexcept
	{
		m_renderPass = rhs.m_renderPass;

		rhs.m_renderPass = VkRenderPass{};

		return *this;
	}

	OverlayPreparer::~OverlayPreparer()noexcept
	{
		if ( m_renderPass )
		{
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
					doPrepareOverlay< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_panelVertexBuffer
						, nullptr
						, false );
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
					doPrepareOverlay< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_panelVertexBuffer
						, nullptr
						, false );
				}
			}
		}

		if ( auto material = overlay.getBorderMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					doPrepareOverlay< OverlayRenderer::BorderPanelVertexBufferPool::Quad >( m_device
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
					doPrepareOverlay< OverlayRenderer::TextVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, *m_renderer.m_textVertexBuffer
						, overlay.getFontTexture()
						, false );
				}
			}
		}
	}
}
