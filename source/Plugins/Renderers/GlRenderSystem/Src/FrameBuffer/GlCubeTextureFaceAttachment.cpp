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

	bool GlCubeTextureFaceAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		FAILURE( "Can't call Blit for a cube face attachment" );
		return false;
	}

	bool GlCubeTextureFaceAttachment::DoAttach()
	{
		m_glAttachmentPoint = eGL_TEXTURE_ATTACHMENT( GetOpenGl().Get( GetAttachmentPoint() ) + GetAttachmentIndex() );
		auto l_texture = std::static_pointer_cast< GlTexture >( GetTexture() );
		bool l_return{ false };

		switch ( l_texture->GetType() )
		{
		case TextureType::Cube:
			GetOpenGl().FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, m_glFace, l_texture->GetGlName(), 0 );
			break;

		case TextureType::CubeArray:
			l_return = GetOpenGl().FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, l_texture->GetGlName(), 0, GetLayer() * 6 + ( m_glFace - eGL_TEXDIM_CUBE_FACE_POSX ) );
			break;
		}

		if ( l_return && m_glStatus == eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
		{
			m_glStatus = eGL_FRAMEBUFFER_STATUS( GetOpenGl().CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) );

			if ( m_glStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				m_glStatus = eGL_FRAMEBUFFER_COMPLETE;
			}
		}
		else
		{
			m_glStatus = eGL_FRAMEBUFFER_UNSUPPORTED;
		}

		return l_return;
	}

	void GlCubeTextureFaceAttachment::DoDetach()
	{
		auto l_pTexture = GetTexture();

		if ( m_glStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
		{
			GetOpenGl().FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = eGL_TEXTURE_ATTACHMENT_NONE;
	}
}
