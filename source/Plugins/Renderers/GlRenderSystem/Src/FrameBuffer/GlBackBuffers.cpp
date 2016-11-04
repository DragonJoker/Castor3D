#include "FrameBuffer/GlBackBuffers.hpp"

#include "Common/OpenGl.hpp"

#include <Graphics/PixelBufferBase.hpp>

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

	bool GlBackBuffers::Create()
	{
		return true;
	}

	void GlBackBuffers::Destroy()
	{
	}

	bool GlBackBuffers::Bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
		bool l_return = true;

		if ( GetOpenGl().HasFbo() )
		{
			l_return = GetOpenGl().BindFramebuffer( GetOpenGl().Get( p_target ), 0 );
		}

		if ( l_return )
		{
			if ( p_target == FrameBufferTarget::eDraw )
			{
				l_return = GetOpenGl().DrawBuffer( GetOpenGl().Get( p_buffer ) );
			}
			else if ( p_target == FrameBufferTarget::eRead )
			{
				l_return = GetOpenGl().ReadBuffer( GetOpenGl().Get( p_buffer ) );
			}
		}

		return l_return;
	}

	void GlBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void GlBackBuffers::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		GetOpenGl().ReadBuffer( GlBufferBinding( uint32_t( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) ) + p_index ) );
	}

	bool GlBackBuffers::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		bool l_return = GetOpenGl().HasFbo();
		auto l_mode = GetOpenGl().Get( FrameBufferTarget::eRead );

		if ( l_return )
		{
			l_return = GetOpenGl().BindFramebuffer( l_mode, 0 );
		}

		if ( l_return )
		{
			l_return = GetOpenGl().ReadBuffer( GlBufferBinding::eBack );

			if ( l_return )
			{
				OpenGl::PixelFmt l_pxFmt = GetOpenGl().Get( p_buffer->format() );
				l_return = GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
			}
		}

		return l_return;
	}

	void GlBackBuffers::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGl().ClearDepth( 1.0 );
		GetOpenGl().Clear( GetOpenGl().GetComponents( uint32_t( BufferComponent::eColour ) | uint32_t( BufferComponent::eDepth ) | uint32_t( BufferComponent::eStencil ) ) );
	}

	bool GlBackBuffers::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )const
	{
		return false;
	}

	bool GlBackBuffers::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )const
	{
		return false;
	}
}
