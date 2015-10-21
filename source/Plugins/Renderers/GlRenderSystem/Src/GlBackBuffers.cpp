#include "GlBackBuffers.hpp"

#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlBackBuffers::GlBackBuffers( OpenGl & p_gl, Engine & p_engine )
		: BackBuffers( p_engine )
		, m_gl( p_gl )
	{
	}

	GlBackBuffers::~GlBackBuffers()
	{
	}

	bool GlBackBuffers::Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
	{
		return true;
	}

	void GlBackBuffers::Destroy()
	{
	}

	bool GlBackBuffers::Bind( eBUFFER p_eBuffer, eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_return = true;

		if ( m_gl.HasFbo() )
		{
			l_return = m_gl.BindFramebuffer( m_gl.Get( p_eTarget ), 0 );
		}

		if ( l_return )
		{
			if ( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
			{
				l_return = m_gl.DrawBuffer( m_gl.Get( p_eBuffer ) );
			}
			else if ( p_eTarget == eFRAMEBUFFER_TARGET_READ )
			{
				l_return = m_gl.ReadBuffer( m_gl.Get( p_eBuffer ) );
			}
		}

		return l_return;
	}

	bool GlBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )
	{
		return false;
	}

	bool GlBackBuffers::SetReadBuffer( eATTACHMENT_POINT p_eAttach, uint8_t p_index )
	{
		return m_gl.ReadBuffer( eGL_BUFFER( m_gl.Get( m_gl.Get( p_eAttach ) ) + p_index ) );
	}

	bool GlBackBuffers::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = m_gl.HasFbo();
		auto l_mode = m_gl.Get( eFRAMEBUFFER_TARGET_READ );

		if ( l_return )
		{
			l_return = m_gl.BindFramebuffer( l_mode, 0 );
		}

		if ( l_return )
		{
			l_return = m_gl.ReadBuffer( eGL_BUFFER_BACK );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = m_gl.Get( p_buffer->format() );
				l_return = m_gl.ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
			}
		}

		return l_return;
	}

	bool GlBackBuffers::DoInitialise( Size const & p_size )
	{
		return true;
	}

	void GlBackBuffers::DoCleanup()
	{
	}

	void GlBackBuffers::DoUpdateClearColour()
	{
		uint8_t l_r, l_g, l_b, l_a;
		m_gl.ClearColor( GetClearColour().red().convert_to( l_r ), GetClearColour().green().convert_to( l_g ), GetClearColour().blue().convert_to( l_b ), GetClearColour().alpha().convert_to( l_a ) );
	}

	void GlBackBuffers::DoClear( uint32_t p_uiTargets )
	{
		m_gl.Clear( m_gl.GetComponents( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL ) );
	}

	void GlBackBuffers::DoResize( Castor::Size const & p_size )
	{
	}

	bool GlBackBuffers::DoBlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolationMode )
	{
		return false;
	}
}
