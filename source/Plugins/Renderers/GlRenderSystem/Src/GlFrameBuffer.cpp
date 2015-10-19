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
		: FrameBuffer( p_engine )
		, m_uiGlName( uint32_t( eGL_INVALID_INDEX ) )
		, m_gl( p_gl )
	{
	}

	GlFrameBuffer::~GlFrameBuffer()
	{
	}

	bool GlFrameBuffer::Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
	{
		if ( m_gl.HasFbo() && m_uiGlName == eGL_INVALID_INDEX )
		{
			m_gl.GenFramebuffers( 1, &m_uiGlName );
			glTrack( m_gl, GlFrameBuffer, this );
		}

		return m_uiGlName != eGL_INVALID_INDEX;
	}

	void GlFrameBuffer::Destroy()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteFramebuffers( 1, &m_uiGlName );
			m_uiGlName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	bool GlFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )
	{
		bool l_return = false;

		if ( !p_attaches.empty() )
		{
			UIntArray l_arrayAttaches;
			l_arrayAttaches.reserve( p_attaches.size() );

			for ( auto && l_attach : p_attaches )
			{
				eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_eAttach != eATTACHMENT_POINT_NONE && l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL )
				{
					l_arrayAttaches.push_back( m_gl.Get( l_eAttach ) );
				}
			}

			l_return = m_gl.DrawBuffers( int( l_arrayAttaches.size() ), &l_arrayAttaches[0] );
		}

		return l_return;
	}

	bool GlFrameBuffer::SetReadBuffer( eATTACHMENT_POINT p_eAttach, uint8_t p_index )
	{
		bool l_return = false;

		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_eAttach;
		} );

		if ( l_it != m_attaches.end() )
		{
			l_return = m_gl.ReadBuffer( m_gl.Get( m_gl.Get( p_eAttach ) ) );
		}

		return l_return;
	}

	bool GlFrameBuffer::IsComplete()
	{
		return eGL_FRAMEBUFFER_STATUS( m_gl.CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) ) == eGL_FRAMEBUFFER_COMPLETE;
	}

	bool GlFrameBuffer::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = m_gl.HasFbo();
		auto l_mode = m_gl.Get( eFRAMEBUFFER_TARGET_READ );

		if ( l_return )
		{
			l_return = m_gl.BindFramebuffer( l_mode, m_uiGlName );
		}

		if ( l_return )
		{
			l_return = m_gl.ReadBuffer( m_gl.Get( eGL_TEXTURE_ATTACHMENT( m_gl.Get( p_point ) + p_index ) ) );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = m_gl.Get( p_buffer->format() );
				l_return = m_gl.ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
			}

			m_gl.BindFramebuffer( l_mode, 0 );
		}

		return l_return;
	}

	ColourRenderBufferSPtr GlFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< GlColourRenderBuffer >( m_gl, p_ePixelFormat );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( m_gl, p_ePixelFormat );
	}

	bool GlFrameBuffer::DoBind( eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_return = false;

		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_eGlBindingMode = m_gl.Get( p_eTarget );
			l_return = m_gl.BindFramebuffer( m_eGlBindingMode, m_uiGlName );
		}

		return l_return;
	}

	void GlFrameBuffer::DoUnbind()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_gl.BindFramebuffer( m_eGlBindingMode, 0 );
		}
	}

	bool GlFrameBuffer::DoBlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolationMode )
	{
		return m_gl.BlitFramebuffer( p_rectDst, p_rectDst, m_gl.GetComponents( p_uiComponents ), m_gl.Get( p_eInterpolationMode ) );
	}
}
