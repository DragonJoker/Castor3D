#include "FrameBuffer/GlCubeTextureFaceAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Texture/GlTexture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlCubeTextureFaceAttachment::GlCubeTextureFaceAttachment( OpenGl & p_gl, TextureLayoutSPtr p_texture, CubeMapFace p_face )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glFace{ p_gl.Get( p_face ) }
	{
	}

	GlCubeTextureFaceAttachment::~GlCubeTextureFaceAttachment()
	{
	}

	void GlCubeTextureFaceAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto l_texture = std::static_pointer_cast< GlTexture >( GetTexture() );

		switch ( l_texture->GetType() )
		{
		case TextureType::eCube:
			GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, l_texture->GetGlName(), 0 );
			break;

		case TextureType::eCubeArray:
			GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_texture->GetGlName(), 0, GetLayer() * 6 + ( uint32_t( m_glFace ) - uint32_t( GlTexDim::ePositiveX ) ) );
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
		auto l_pTexture = GetTexture();

		if ( m_glStatus != GlFramebufferStatus::eUnsupported )
		{
			GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = GlAttachmentPoint::eNone;
	}
}
