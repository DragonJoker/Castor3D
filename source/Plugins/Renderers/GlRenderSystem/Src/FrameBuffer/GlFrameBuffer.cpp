#include "FrameBuffer/GlFrameBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlColourRenderBuffer.hpp"
#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"
#include "FrameBuffer/GlRenderBufferAttachment.hpp"
#include "FrameBuffer/GlTextureAttachment.hpp"
#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"
#include "Texture/GlTexture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameBuffer::GlFrameBuffer( OpenGl & p_gl, Engine & engine )
		: BindableType( p_gl,
						"GlFrameBuffer",
						std::bind( &OpenGl::GenFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteFramebuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsFramebuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, this]( uint32_t p_glName )
						{
							return p_gl.BindFramebuffer( m_bindingMode, p_glName );
						} )
		, FrameBuffer( engine )
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

	void GlFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )const
	{
		bool result = false;

		if ( !p_attaches.empty() )
		{
			UIntArray arrayAttaches;
			arrayAttaches.reserve( p_attaches.size() );

			for ( auto attach : p_attaches )
			{
				AttachmentPoint eAttach = attach->GetAttachmentPoint();

				if ( eAttach == AttachmentPoint::eColour )
				{
					arrayAttaches.push_back( uint32_t( GetOpenGl().Get( eAttach ) ) + attach->GetAttachmentIndex() );
				}
			}

			if ( !arrayAttaches.empty() )
			{
				GetOpenGl().DrawBuffers( int( arrayAttaches.size() ), &arrayAttaches[0] );
			}
			else
			{
				GetOpenGl().DrawBuffer( GlBufferBinding::eNone );
			}
		}
		else
		{
			GetOpenGl().DrawBuffer( GlBufferBinding::eNone );
		}
	}

	void GlFrameBuffer::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_eAttach;
		} );

		if ( it != m_attaches.end() )
		{
			GetOpenGl().ReadBuffer( GetOpenGl().Get( GetOpenGl().Get( p_eAttach ) ) );
		}
	}

	bool GlFrameBuffer::IsComplete()const
	{
		return GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) == GlFramebufferStatus::eComplete;
	}

	void GlFrameBuffer::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		DoBind( FrameBufferTarget::eRead );
		GetOpenGl().ReadBuffer( GlBufferBinding( uint32_t( GetOpenGl().Get( GetOpenGl().Get( p_point ) ) ) + p_index ) );
		OpenGl::PixelFmt pxFmt = GetOpenGl().Get( p_buffer->format() );
		GetOpenGl().ReadPixels( Position(), p_buffer->dimensions(), pxFmt.Format, pxFmt.Type, p_buffer->ptr() );
		DoUnbind();
	}

	ColourRenderBufferSPtr GlFrameBuffer::CreateColourRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlColourRenderBuffer >( GetOpenGl(), p_format );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::CreateDepthStencilRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( GetOpenGl(), p_format );
	}

	RenderBufferAttachmentSPtr GlFrameBuffer::CreateAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< GlRenderBufferAttachment >( GetOpenGl(), p_renderBuffer );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture )
	{
		return std::make_shared< GlTextureAttachment >( GetOpenGl(), p_texture );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )
	{
		return std::make_shared< GlCubeTextureFaceAttachment >( GetOpenGl(), p_texture, p_face );
	}

	TextureAttachmentSPtr GlFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel )
	{
		return std::make_shared< GlCubeTextureFaceAttachment >( GetOpenGl(), p_texture, p_face, p_mipLevel );
	}

	void GlFrameBuffer::DoBind( FrameBufferTarget p_target )const
	{
		m_bindingMode = GetOpenGl().Get( p_target );
		BindableType::Bind();
	}

	void GlFrameBuffer::DoUnbind()const
	{
		BindableType::Unbind();
	}

	void GlFrameBuffer::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
		GetOpenGl().BlitFramebuffer( p_rect, p_rect, GetOpenGl().GetComponents( p_components ), GlInterpolationMode::eNearest );
	}

	void GlFrameBuffer::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GetOpenGl().GetComponents( p_components ), GetOpenGl().Get( p_interpolation ) );
	}

	void GlFrameBuffer::DoClear( BufferComponents p_targets )
	{
		GetOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		GetOpenGl().ClearDepth( 1.0 );

		if ( CheckFlag( p_targets, BufferComponent::eDepth ) )
		{
			GetOpenGl().DepthMask( true );
		}

		GetOpenGl().Clear( GetOpenGl().GetComponents( p_targets ) );
	}
}
