#include "FrameBuffer/GlES3FrameBuffer.hpp"

#include "Common/OpenGlES3.hpp"
#include "FrameBuffer/GlES3ColourRenderBuffer.hpp"
#include "FrameBuffer/GlES3DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/GlES3RenderBufferAttachment.hpp"
#include "FrameBuffer/GlES3TextureAttachment.hpp"
#include "FrameBuffer/GlES3CubeTextureFaceAttachment.hpp"
#include "Texture/GlES3Texture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3FrameBuffer::GlES3FrameBuffer( OpenGlES3 & p_gl, Engine & p_engine )
		: BindableType( p_gl,
						"GlES3FrameBuffer",
						std::bind( &OpenGlES3::GenFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::DeleteFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::IsFramebuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, this]( uint32_t p_glName )
						{
							return p_gl.BindFramebuffer( m_bindingMode, p_glName );
						} )
		, FrameBuffer( p_engine )
	{
	}

	GlES3FrameBuffer::~GlES3FrameBuffer()
	{
	}

	bool GlES3FrameBuffer::Create()
	{
		return BindableType::Create( false );
	}

	void GlES3FrameBuffer::Destroy()
	{
		BindableType::Destroy();
	}

	void GlES3FrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )const
	{
		bool l_return = false;

		if ( !p_attaches.empty() )
		{
			UIntArray l_arrayAttaches;
			l_arrayAttaches.reserve( p_attaches.size() );

			for ( auto l_attach : p_attaches )
			{
				AttachmentPoint l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_eAttach == AttachmentPoint::eColour )
				{
					l_arrayAttaches.push_back( uint32_t( GetOpenGlES3().Get( l_eAttach ) ) + l_attach->GetAttachmentIndex() );
				}
			}

			if ( !l_arrayAttaches.empty() )
			{
				GetOpenGlES3().DrawBuffers( int( l_arrayAttaches.size() ), &l_arrayAttaches[0] );
			}
			else
			{
				GetOpenGlES3().DrawBuffer( GlES3BufferBinding::eNone );
			}
		}
		else
		{
			GetOpenGlES3().DrawBuffer( GlES3BufferBinding::eNone );
		}
	}

	void GlES3FrameBuffer::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_eAttach;
		} );

		if ( l_it != m_attaches.end() )
		{
			GetOpenGlES3().ReadBuffer( GetOpenGlES3().Get( GetOpenGlES3().Get( p_eAttach ) ) );
		}
	}

	bool GlES3FrameBuffer::IsComplete()const
	{
		return GetOpenGlES3().CheckFramebufferStatus( GlES3FrameBufferMode::eDefault ) == GlES3FramebufferStatus::eComplete;
	}

	void GlES3FrameBuffer::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		DoBind( FrameBufferTarget::eRead );
		GetOpenGlES3().ReadBuffer( GlES3BufferBinding( uint32_t( GetOpenGlES3().Get( GetOpenGlES3().Get( p_point ) ) ) + p_index ) );
		OpenGlES3::PixelFmt l_pxFmt = GetOpenGlES3().Get( p_buffer->format() );
		GetOpenGlES3().ReadPixels( Position(), p_buffer->dimensions(), l_pxFmt.Format, l_pxFmt.Type, p_buffer->ptr() );
		DoUnbind();
	}

	ColourRenderBufferSPtr GlES3FrameBuffer::CreateColourRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlES3ColourRenderBuffer >( GetOpenGlES3(), p_format );
	}

	DepthStencilRenderBufferSPtr GlES3FrameBuffer::CreateDepthStencilRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlES3DepthStencilRenderBuffer >( GetOpenGlES3(), p_format );
	}

	RenderBufferAttachmentSPtr GlES3FrameBuffer::CreateAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< GlES3RenderBufferAttachment >( GetOpenGlES3(), p_renderBuffer );
	}

	TextureAttachmentSPtr GlES3FrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture )
	{
		return std::make_shared< GlES3TextureAttachment >( GetOpenGlES3(), p_texture );
	}

	TextureAttachmentSPtr GlES3FrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )
	{
		return std::make_shared< GlES3CubeTextureFaceAttachment >( GetOpenGlES3(), p_texture, p_face );
	}

	void GlES3FrameBuffer::DoClear( uint32_t p_uiTargets )
	{
		GetOpenGlES3().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGlES3().ClearDepth( 1.0 );
		GetOpenGlES3().Clear( GetOpenGlES3().GetComponents( p_uiTargets ) );
	}

	void GlES3FrameBuffer::DoBind( FrameBufferTarget p_target )const
	{
		m_bindingMode = GetOpenGlES3().Get( p_target );
		BindableType::Bind();
	}

	void GlES3FrameBuffer::DoUnbind()const
	{
		BindableType::Unbind();
	}

	void GlES3FrameBuffer::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
		GetOpenGlES3().BlitFramebuffer( p_rect, p_rect, GetOpenGlES3().GetComponents( p_components ), GlES3InterpolationMode::eNearest );
	}

	void GlES3FrameBuffer::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, GetOpenGlES3().GetComponents( p_components ), GetOpenGlES3().Get( p_interpolation ) );
	}
}
