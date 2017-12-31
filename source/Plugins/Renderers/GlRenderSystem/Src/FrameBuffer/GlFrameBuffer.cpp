#include "FrameBuffer/GlFrameBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlColourRenderBuffer.hpp"
#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"
#include "FrameBuffer/GlRenderBufferAttachment.hpp"
#include "FrameBuffer/GlTextureAttachment.hpp"
#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"
#include "Texture/GlTexture.hpp"

using namespace castor3d;
using namespace castor;

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

	bool GlFrameBuffer::initialise()
	{
		return BindableType::create( false );
	}

	void GlFrameBuffer::cleanup()
	{
		BindableType::destroy();
	}

	void GlFrameBuffer::setDrawBuffers( AttachArray const & p_attaches )const
	{
		if ( !p_attaches.empty() )
		{
			UIntArray arrayAttaches;
			arrayAttaches.reserve( p_attaches.size() );

			for ( auto attach : p_attaches )
			{
				AttachmentPoint eAttach = attach->getAttachmentPoint();

				if ( eAttach == AttachmentPoint::eColour )
				{
					arrayAttaches.push_back( uint32_t( getOpenGl().get( eAttach ) ) + attach->getAttachmentIndex() );
				}
			}

			if ( !arrayAttaches.empty() )
			{
				getOpenGl().DrawBuffers( int( arrayAttaches.size() ), &arrayAttaches[0] );
			}
			else
			{
				getOpenGl().DrawBuffer( GlBufferBinding::eNone );
			}
		}
		else
		{
			getOpenGl().DrawBuffer( GlBufferBinding::eNone );
		}
	}

	void GlFrameBuffer::setReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
		auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->getAttachmentPoint() == p_eAttach;
		} );

		if ( it != m_attaches.end() )
		{
			getOpenGl().ReadBuffer( getOpenGl().get( getOpenGl().get( p_eAttach ) ) );
		}
	}

	bool GlFrameBuffer::isComplete()const
	{
		return getOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) == GlFramebufferStatus::eComplete;
	}

	void GlFrameBuffer::downloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )const
	{
		doBind( FrameBufferTarget::eRead );
		getOpenGl().ReadBuffer( GlBufferBinding( uint32_t( getOpenGl().get( getOpenGl().get( p_point ) ) ) + p_index ) );
		OpenGl::PixelFmt pxFmt = getOpenGl().get( p_buffer->format() );
		getOpenGl().ReadPixels( Position(), p_buffer->dimensions(), pxFmt.Format, pxFmt.Type, p_buffer->ptr() );
		doUnbind();
	}

	ColourRenderBufferSPtr GlFrameBuffer::createColourRenderBuffer( PixelFormat p_format )const
	{
		return std::make_shared< GlColourRenderBuffer >( getOpenGl(), p_format );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::createDepthStencilRenderBuffer( PixelFormat p_format )const
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( getOpenGl(), p_format );
	}

	RenderBufferAttachmentSPtr GlFrameBuffer::createAttachment( RenderBufferSPtr p_renderBuffer )const
	{
		return std::make_shared< GlRenderBufferAttachment >( getOpenGl(), p_renderBuffer );
	}

	TextureAttachmentSPtr GlFrameBuffer::createAttachment( TextureLayoutSPtr p_texture )const
	{
		return std::make_shared< GlTextureAttachment >( getOpenGl(), p_texture, 0u );
	}

	TextureAttachmentSPtr GlFrameBuffer::createAttachment( TextureLayoutSPtr texture
		, uint32_t mipLevel )const
	{
		return std::make_shared< GlTextureAttachment >( getOpenGl(), texture, mipLevel );
	}

	TextureAttachmentSPtr GlFrameBuffer::createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )const
	{
		return std::make_shared< GlCubeTextureFaceAttachment >( getOpenGl(), p_texture, p_face );
	}

	TextureAttachmentSPtr GlFrameBuffer::createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel )const
	{
		return std::make_shared< GlCubeTextureFaceAttachment >( getOpenGl(), p_texture, p_face, p_mipLevel );
	}

	void GlFrameBuffer::doBind( FrameBufferTarget p_target )const
	{
		m_bindingMode = getOpenGl().get( p_target );

		if ( isSRGB() )
		{
			getOpenGl().Enable( GlTweak::eFramebufferSRGB );
		}

		BindableType::bind();
	}

	void GlFrameBuffer::doUnbind()const
	{
		BindableType::unbind();

		if ( isSRGB() )
		{
			getOpenGl().Disable( GlTweak::eFramebufferSRGB );
		}
	}

	void GlFrameBuffer::doBlitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
		getOpenGl().BlitFramebuffer( p_rect, p_rect, getOpenGl().getComponents( p_components ), GlInterpolationMode::eNearest );
	}

	void GlFrameBuffer::doStretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		getOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, getOpenGl().getComponents( p_components ), getOpenGl().get( p_interpolation ) );
	}

	void GlFrameBuffer::doClear( BufferComponents p_targets )const
	{
		getOpenGl().ClearColor( m_redClear, m_greenClear, m_blueClear, m_alphaClear );
		getOpenGl().ClearDepth( 1.0 );

		if ( checkFlag( p_targets, BufferComponent::eDepth ) )
		{
			getOpenGl().DepthMask( true );
		}

		getOpenGl().Clear( getOpenGl().getComponents( p_targets ) );
	}
}
