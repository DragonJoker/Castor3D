#include "FrameBuffer/GlBackBuffers.hpp"

#include "Common/OpenGl.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlBackBuffers::GlBackBuffers( OpenGl & p_gl, Engine & engine )
		: Holder( p_gl )
		, BackBuffers( engine )
	{
	}

	GlBackBuffers::~GlBackBuffers()
	{
	}

	bool GlBackBuffers::initialise()
	{
		return true;
	}

	void GlBackBuffers::cleanup()
	{
	}

	void GlBackBuffers::bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
		REQUIRE( getOpenGl().hasFbo() );
		getOpenGl().BindFramebuffer( getOpenGl().get( p_target ), 0 );

		if ( p_target == FrameBufferTarget::eDraw )
		{
			getOpenGl().DrawBuffer( getOpenGl().get( p_buffer ) );
		}
		else if ( p_target == FrameBufferTarget::eRead )
		{
			getOpenGl().ReadBuffer( getOpenGl().get( p_buffer ) );
		}
	}

	void GlBackBuffers::setDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void GlBackBuffers::setReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		getOpenGl().ReadBuffer( GlBufferBinding( uint32_t( getOpenGl().get( getOpenGl().get( p_eAttach ) ) ) + p_index ) );
	}

	void GlBackBuffers::downloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )const
	{
		REQUIRE( getOpenGl().hasFbo() );
		auto mode = getOpenGl().get( FrameBufferTarget::eRead );
		getOpenGl().BindFramebuffer( mode, 0 );
		getOpenGl().ReadBuffer( GlBufferBinding::eBack );
		OpenGl::PixelFmt pxFmt = getOpenGl().get( p_buffer->format() );
		getOpenGl().ReadPixels( Position(), p_buffer->dimensions(), pxFmt.Format, pxFmt.Type, p_buffer->ptr() );
	}

	void GlBackBuffers::doClear( BufferComponents p_targets )const
	{
		getOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		getOpenGl().ClearDepth( 1.0 );
		getOpenGl().Clear( getOpenGl().getComponents( p_targets ) );
	}

	void GlBackBuffers::doBlitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void GlBackBuffers::doStretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}
