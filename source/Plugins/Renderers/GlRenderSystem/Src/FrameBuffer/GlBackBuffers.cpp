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

	void GlBackBuffers::Bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
		REQUIRE( GetOpenGl().HasFbo() );
		GetOpenGl().BindFramebuffer( GetOpenGl().Get( p_target ), 0 );

		if ( p_target == FrameBufferTarget::eDraw )
		{
			GetOpenGl().DrawBuffer( GetOpenGl().Get( p_buffer ) );
		}
		else if ( p_target == FrameBufferTarget::eRead )
		{
			GetOpenGl().ReadBuffer( GetOpenGl().Get( p_buffer ) );
		}
	}

	void GlBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void GlBackBuffers::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		GetOpenGl().ReadBuffer( GlBufferBinding( uint32_t( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) ) + p_index ) );
	}

	void GlBackBuffers::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		REQUIRE( GetOpenGl().HasFbo() );
		auto mode = GetOpenGl().Get( FrameBufferTarget::eRead );
		GetOpenGl().BindFramebuffer( mode, 0 );
		GetOpenGl().ReadBuffer( GlBufferBinding::eBack );
		OpenGl::PixelFmt pxFmt = GetOpenGl().Get( p_buffer->format() );
		GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), pxFmt.Format, pxFmt.Type, p_buffer->ptr() );
	}

	void GlBackBuffers::DoClear( BufferComponents p_targets )
	{
		GetOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGl().ClearDepth( 1.0 );
		GetOpenGl().Clear( GetOpenGl().GetComponents( p_targets ) );
	}

	void GlBackBuffers::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void GlBackBuffers::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}
