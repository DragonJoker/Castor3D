#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Texture/GlTexture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlCubeTextureFaceAttachment::GlCubeTextureFaceAttachment( OpenGl & p_gl
		, TextureLayoutSPtr p_texture
		, CubeMapFace p_face
		, uint32_t p_mipLevel )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glFace{ p_gl.Get( p_face ) }
		, m_mipLevel{ p_mipLevel }
	{
	}

	GlCubeTextureFaceAttachment::~GlCubeTextureFaceAttachment()
	{
	}

	void GlCubeTextureFaceAttachment::DoDownload( Castor::Position const & p_offset
		, Castor::PxBufferBase & p_buffer )const
	{
		GetOpenGl().ReadBuffer( GlBufferBinding( int( GlBufferBinding::eColor0 ) + GetAttachmentIndex() ) );
		auto format = GetOpenGl().Get( p_buffer.format() );
		GetOpenGl().ReadPixels( p_offset
			, p_buffer.dimensions()
			, format.Format
			, format.Type
			, p_buffer.ptr() );
	}

	void GlCubeTextureFaceAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto texture = std::static_pointer_cast< GlTexture >( GetTexture() );

		switch ( texture->GetType() )
		{
		case TextureType::eCube:
			GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, texture->GetGlName(), m_mipLevel );
			break;

		case TextureType::eCubeArray:
			REQUIRE( m_mipLevel == 0u );
			GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, texture->GetGlName(), 0, GetLayer() * 6 + ( uint32_t( m_glFace ) - uint32_t( GlTexDim::ePositiveX ) ) );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment type" ) );
			break;
		}

		if ( m_glStatus == GlFramebufferStatus::eIncompleteMissingAttachment )
		{
			m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );

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

	void GlCubeTextureFaceAttachment::DoDetach()
	{
		auto pTexture = GetTexture();

		if ( m_glStatus != GlFramebufferStatus::eUnsupported )
		{
			GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = GlAttachmentPoint::eNone;
	}

	void GlCubeTextureFaceAttachment::DoClear( Colour const & p_colour )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eColour
			, GetAttachmentIndex()
			, p_colour.const_ptr() );
	}

	void GlCubeTextureFaceAttachment::DoClear( float p_depth )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepth
			, GetAttachmentIndex()
			, &p_depth );
	}

	void GlCubeTextureFaceAttachment::DoClear( int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eStencil
			, GetAttachmentIndex()
			, &p_stencil );
	}

	void GlCubeTextureFaceAttachment::DoClear( float p_depth, int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, GetAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}
