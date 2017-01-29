#include "FrameBuffer/GlES3BackBuffers.hpp"

#include "Common/OpenGlES3.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3BackBuffers::GlES3BackBuffers( OpenGlES3 & p_gl, Engine & p_engine )
		: Holder( p_gl )
		, BackBuffers( p_engine )
	{
	}

	GlES3BackBuffers::~GlES3BackBuffers()
	{
	}

	bool GlES3BackBuffers::Create()
	{
		return true;
	}

	void GlES3BackBuffers::Destroy()
	{
	}

	void GlES3BackBuffers::Bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
		REQUIRE( GetOpenGlES3().HasFbo() );
		GetOpenGlES3().BindFramebuffer( GetOpenGlES3().Get( p_target ), 0 );

		if ( p_target == FrameBufferTarget::eDraw )
		{
			GetOpenGlES3().DrawBuffer( GetOpenGlES3().Get( p_buffer ) );
		}
		else if ( p_target == FrameBufferTarget::eRead )
		{
			GetOpenGlES3().ReadBuffer( GetOpenGlES3().Get( p_buffer ) );
		}
	}

	void GlES3BackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void GlES3BackBuffers::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		GetOpenGlES3().ReadBuffer( GlES3BufferBinding( uint32_t( GetOpenGlES3().Get( GetOpenGlES3().Get( p_eAttach ) ) ) + p_index ) );
	}

	void GlES3BackBuffers::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		REQUIRE( GetOpenGlES3().HasFbo() );
		auto l_mode = GetOpenGlES3().Get( FrameBufferTarget::eRead );
		GetOpenGlES3().BindFramebuffer( l_mode, 0 );
		GetOpenGlES3().ReadBuffer( GlES3BufferBinding::eBack );
		OpenGlES3::PixelFmt l_pxFmt = GetOpenGlES3().Get( p_buffer->format() );
		GetOpenGlES3().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
	}

	void GlES3BackBuffers::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGlES3().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGlES3().ClearDepth( 1.0 );
		GetOpenGlES3().Clear( GetOpenGlES3().GetComponents( uint32_t( BufferComponent::eColour ) | uint32_t( BufferComponent::eDepth ) | uint32_t( BufferComponent::eStencil ) ) );
	}

	void GlES3BackBuffers::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void GlES3BackBuffers::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}
