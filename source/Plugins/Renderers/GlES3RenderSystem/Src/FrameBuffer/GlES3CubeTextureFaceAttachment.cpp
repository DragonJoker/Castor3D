#include "FrameBuffer/GlES3CubeTextureFaceAttachment.hpp"

#include "Common/OpenGlES3.hpp"
#include "FrameBuffer/GlES3FrameBuffer.hpp"
#include "Texture/GlES3Texture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3CubeTextureFaceAttachment::GlES3CubeTextureFaceAttachment( OpenGlES3 & p_gl, TextureLayoutSPtr p_texture, CubeMapFace p_face )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glFace{ p_gl.Get( p_face ) }
	{
	}

	GlES3CubeTextureFaceAttachment::~GlES3CubeTextureFaceAttachment()
	{
	}

	void GlES3CubeTextureFaceAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		FAILURE( "Can't call Blit for a cube face attachment" );
	}

	void GlES3CubeTextureFaceAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlES3AttachmentPoint( uint32_t( GetOpenGlES3().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto l_texture = std::static_pointer_cast< GlES3Texture >( GetTexture() );

		switch ( l_texture->GetType() )
		{
		case TextureType::eCube:
			GetOpenGlES3().FramebufferTexture2D( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, l_texture->GetGlES3Name(), 0 );
			break;

		case TextureType::eCubeArray:
			GetOpenGlES3().FramebufferTextureLayer( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, l_texture->GetGlES3Name(), 0, GetLayer() * 6 + ( uint32_t( m_glFace ) - uint32_t( GlES3TexDim::ePositiveX ) ) );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment type" ) );
			break;
		}

		if ( m_glStatus == GlES3FramebufferStatus::eIncompleteMissingAttachment )
		{
			m_glStatus = GlES3FramebufferStatus( GetOpenGlES3().CheckFramebufferStatus( GlES3FrameBufferMode::eDefault ) );

			if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
			{
				m_glStatus = GlES3FramebufferStatus::eComplete;
			}
		}
		else
		{
			m_glStatus = GlES3FramebufferStatus::eUnsupported;
		}
	}

	void GlES3CubeTextureFaceAttachment::DoDetach()
	{
		auto l_pTexture = GetTexture();

		if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
		{
			GetOpenGlES3().FramebufferTexture2D( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = GlES3AttachmentPoint::eNone;
	}
}
