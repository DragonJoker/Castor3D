#include "GlFrameBuffer.hpp"
#include "GlBuffer.hpp"
#include "GlColourRenderBuffer.hpp"
#include "GlDepthStencilRenderBuffer.hpp"
#include "GlRenderBufferAttachment.hpp"
#include "GlTextureAttachment.hpp"
#include "GlDynamicTexture.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameBuffer::GlFrameBuffer( OpenGl & p_gl, Engine & p_engine )
		: BindableType( p_gl,
						"GlFrameBuffer",
						std::bind( &OpenGl::GenFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsFramebuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, this]( uint32_t p_glName )
						{
							return p_gl.BindFramebuffer( m_bindingMode, p_glName );
						} )
		, FrameBuffer( p_engine )
	{
	}

	GlFrameBuffer::~GlFrameBuffer()
	{
	}

	bool GlFrameBuffer::Create()
	{
		return BindableType::Create( false );
	}

	void GlFrameBuffer::Destroy()
	{
		BindableType::Destroy();
	}

	void GlFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )
	{
		bool l_return = false;

		if ( !p_attaches.empty() )
		{
			UIntArray l_arrayAttaches;
			l_arrayAttaches.reserve( p_attaches.size() );

			for ( auto && l_attach : p_attaches )
			{
				eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_eAttach == eATTACHMENT_POINT_COLOUR )
				{
					l_arrayAttaches.push_back( GetOpenGl().Get( l_eAttach ) + l_attach->GetAttachmentIndex() );
				}
			}

			GetOpenGl().DrawBuffers( int( l_arrayAttaches.size() ), &l_arrayAttaches[0] );
		}
	}

	void GlFrameBuffer::SetReadBuffer( eATTACHMENT_POINT p_eAttach, uint8_t p_index )
	{
		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_eAttach;
		} );

		if ( l_it != m_attaches.end() )
		{
			GetOpenGl().ReadBuffer( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) );
		}
	}

	bool GlFrameBuffer::IsComplete()const
	{
		return GetOpenGl().CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) == eGL_FRAMEBUFFER_COMPLETE;
	}

	bool GlFrameBuffer::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = DoBind( eFRAMEBUFFER_TARGET_READ );

		if ( l_return )
		{
			l_return = GetOpenGl().ReadBuffer( eGL_BUFFER( GetOpenGl().Get( GetOpenGl().Get( p_point ) ) + p_index ) );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = GetOpenGl().Get( p_buffer->format() );
				l_return = GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
			}

			DoUnbind();
		}

		return l_return;
	}

	ColourRenderBufferSPtr GlFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_format )
	{
		return std::make_shared< GlColourRenderBuffer >( GetOpenGl(), p_format );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_format )
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( GetOpenGl(), p_format );
	}

	RenderBufferAttachmentSPtr GlFrameBuffer::CreateAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< GlRenderBufferAttachment >( GetOpenGl(), p_renderBuffer );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( DynamicTextureSPtr p_texture )
	{
		return std::make_shared< GlTextureAttachment >( GetOpenGl(), p_texture );
	}

	void GlFrameBuffer::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGl().ClearColor( GetClearColour().red(), GetClearColour().green(), GetClearColour().blue(), GetClearColour().alpha() );
		GetOpenGl().Clear( GetOpenGl().GetComponents( p_uiTargets ) );
	}

	bool GlFrameBuffer::DoBind( eFRAMEBUFFER_TARGET p_target )
	{
		m_bindingMode = GetOpenGl().Get( p_target );
		return BindableType::Bind();
	}

	void GlFrameBuffer::DoUnbind()
	{
		BindableType::Unbind();
	}

	bool GlFrameBuffer::DoBlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )
	{
		return GetOpenGl().BlitFramebuffer( p_rect, p_rect, GetOpenGl().GetComponents( p_components ), eGL_INTERPOLATION_MODE_NEAREST );
	}

	bool GlFrameBuffer::DoStretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, eINTERPOLATION_MODE p_interpolation )
	{
		return GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GetOpenGl().GetComponents( p_components ), GetOpenGl().Get( p_interpolation ) );
	}
}
