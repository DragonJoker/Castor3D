#include "GlBackBuffers.hpp"

#include "OpenGl.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlBackBuffers::GlBackBuffers( OpenGl & p_gl, Engine & p_engine )
		: Holder( p_gl )
		, BackBuffers( p_engine )
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

	bool GlBackBuffers::Bind( eBUFFER p_buffer, eFRAMEBUFFER_TARGET p_target )
	{
		bool l_return = true;

		if ( GetOpenGl().HasFbo() )
		{
			l_return = GetOpenGl().BindFramebuffer( GetOpenGl().Get( p_target ), 0 );
		}

		if ( l_return )
		{
			if ( p_target == eFRAMEBUFFER_TARGET_DRAW )
			{
				l_return = GetOpenGl().DrawBuffer( GetOpenGl().Get( p_buffer ) );
			}
			else if ( p_target == eFRAMEBUFFER_TARGET_READ )
			{
				l_return = GetOpenGl().ReadBuffer( GetOpenGl().Get( p_buffer ) );
			}
		}

		return l_return;
	}

	void GlBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )
	{
	}

	void GlBackBuffers::SetReadBuffer( eATTACHMENT_POINT p_eAttach, uint8_t p_index )
	{
		GetOpenGl().ReadBuffer( eGL_BUFFER( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) + p_index ) );
	}

	bool GlBackBuffers::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = GetOpenGl().HasFbo();
		auto l_mode = GetOpenGl().Get( eFRAMEBUFFER_TARGET_READ );

		if ( l_return )
		{
			l_return = GetOpenGl().BindFramebuffer( l_mode, 0 );
		}

		if ( l_return )
		{
			l_return = GetOpenGl().ReadBuffer( eGL_BUFFER_BACK );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = GetOpenGl().Get( p_buffer->format() );
				l_return = GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
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
		GetOpenGl().ClearColor( GetClearColour().red().convert_to( l_r ), GetClearColour().green().convert_to( l_g ), GetClearColour().blue().convert_to( l_b ), GetClearColour().alpha().convert_to( l_a ) );
	}

	void GlBackBuffers::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGl().Clear( GetOpenGl().GetComponents( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL ) );
	}

	void GlBackBuffers::DoResize( Castor::Size const & p_size )
	{
	}

	bool GlBackBuffers::DoBlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )
	{
		return false;
	}

	bool GlBackBuffers::DoStretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, eINTERPOLATION_MODE p_interpolation )
	{
		return false;
	}
}
