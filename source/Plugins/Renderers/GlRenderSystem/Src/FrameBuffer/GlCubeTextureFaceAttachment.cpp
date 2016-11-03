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
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto l_texture = std::static_pointer_cast< GlTexture >( GetTexture() );
		bool l_return{ false };

		switch ( l_texture->GetType() )
		{
		case TextureType::Cube:
			l_return = GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::Default, m_glAttachmentPoint, m_glFace, l_texture->GetGlName(), 0 );
			break;

		case TextureType::CubeArray:
			l_return = GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::Default, m_glAttachmentPoint, l_texture->GetGlName(), 0, GetLayer() * 6 + ( uint32_t( m_glFace ) - uint32_t( GlTexDim::PositiveX ) ) );
			break;
		}

		if ( l_return && m_glStatus == GlFramebufferStatus::IncompleteMissingAttachment )
		{
			m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::Default ) );

			if ( m_glStatus != GlFramebufferStatus::Unsupported )
			{
				m_glStatus = GlFramebufferStatus::Complete;
			}
		}
		else
		{
			m_glStatus = GlFramebufferStatus::Unsupported;
		}

		return l_return;
	}

	void GlCubeTextureFaceAttachment::DoDetach()
	{
		auto l_pTexture = GetTexture();

		if ( m_glStatus != GlFramebufferStatus::Unsupported )
		{
			GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::Default, m_glAttachmentPoint, m_glFace, 0, 0 );
		}

		m_glAttachmentPoint = GlAttachmentPoint::None;
	}
}
