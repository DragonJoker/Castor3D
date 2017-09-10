#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Texture/GlTexture.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlCubeTextureFaceAttachment::GlCubeTextureFaceAttachment( OpenGl & p_gl
		, TextureLayoutSPtr p_texture
		, CubeMapFace p_face
		, uint32_t p_mipLevel )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glFace{ p_gl.get( p_face ) }
		, m_mipLevel{ p_mipLevel }
		, m_face{ p_face }
	{
	}

	GlCubeTextureFaceAttachment::~GlCubeTextureFaceAttachment()
	{
	}

	void GlCubeTextureFaceAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
		getOpenGl().ReadBuffer( GlBufferBinding( int( GlBufferBinding::eColor0 ) + getAttachmentIndex() ) );
		auto format = getOpenGl().get( p_buffer.format() );
		getOpenGl().ReadPixels( p_offset
			, p_buffer.dimensions()
			, format.Format
			, format.Type
			, p_buffer.ptr() );
	}

	void GlCubeTextureFaceAttachment::doAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( getOpenGl().get( getAttachmentPoint() ) ) + getAttachmentIndex() );
		auto texture = std::static_pointer_cast< GlTexture >( getTexture() );

		switch ( texture->getType() )
		{
		case TextureType::eCube:
			getOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, texture->getGlName(), m_mipLevel );
			break;

		case TextureType::eCubeArray:
			REQUIRE( m_mipLevel == 0u );
			getOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, texture->getGlName(), 0, getLayer() * 6 + ( uint32_t( m_glFace ) - uint32_t( GlTexDim::ePositiveX ) ) );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment type" ) );
			break;
		}

		if ( m_glStatus == GlFramebufferStatus::eIncompleteMissingAttachment )
		{
			m_glStatus = GlFramebufferStatus( getOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );
			REQUIRE( m_glStatus == GlFramebufferStatus::eComplete );

			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				m_glStatus = GlFramebufferStatus::eComplete;
			}
		}
		else
		{
			m_glStatus = GlFramebufferStatus::eUnsupported;
		}
	}

	void GlCubeTextureFaceAttachment::doDetach()
	{
		auto pTexture = getTexture();

		if ( m_glStatus != GlFramebufferStatus::eUnsupported )
		{
			getOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = GlAttachmentPoint::eNone;
	}

	void GlCubeTextureFaceAttachment::doClear( Colour const & p_colour )const
	{
		getOpenGl().ClearBuffer( GlComponent::eColour
			, getAttachmentIndex()
			, p_colour.constPtr() );
	}

	void GlCubeTextureFaceAttachment::doClear( float p_depth )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepth
			, getAttachmentIndex()
			, &p_depth );
	}

	void GlCubeTextureFaceAttachment::doClear( int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eStencil
			, getAttachmentIndex()
			, &p_stencil );
	}

	void GlCubeTextureFaceAttachment::doClear( float p_depth, int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, getAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}
