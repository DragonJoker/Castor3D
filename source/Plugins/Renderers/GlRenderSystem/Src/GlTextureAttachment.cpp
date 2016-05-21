#include "GlTextureAttachment.hpp"

#include "GlFrameBuffer.hpp"
#include "GlTexture.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureAttachment::GlTextureAttachment( OpenGl & p_gl, TextureLayoutSPtr p_texture )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glAttachmentPoint( eGL_TEXTURE_ATTACHMENT_NONE )
		, m_glStatus( eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	bool GlTextureAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_interpolation )
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			l_return = GetFrameBuffer()->IsComplete();
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_buffer );

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, std::static_pointer_cast< GlFrameBuffer >( GetFrameBuffer() )->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().ReadBuffer( eGL_BUFFER( GetOpenGl().Get( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() ) );
			}

			if ( l_return )
			{
				if ( m_glAttachmentPoint == eGL_TEXTURE_ATTACHMENT_DEPTH )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_DEPTH, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else if ( m_glAttachmentPoint == eGL_TEXTURE_ATTACHMENT_STENCIL )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_STENCIL, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_COLOR, GetOpenGl().Get( p_interpolation ) );
				}
			}

			GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, 0 );
			GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, 0 );
		}

		return l_return;
	}

	bool GlTextureAttachment::DoAttach( FrameBufferSPtr p_frameBuffer )
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			m_glAttachmentPoint = eGL_TEXTURE_ATTACHMENT( GetOpenGl().Get( GetAttachmentPoint() ) + GetAttachmentIndex() );
			auto l_pTexture = std::static_pointer_cast< GlTexture >( GetTexture() );

			switch ( GetTarget() )
			{
			case eTEXTURE_TYPE_1D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_1D )
				{
					l_return = GetOpenGl().FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TYPE_2D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_2D )
				{
					l_return = GetOpenGl().FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TYPE_3D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_3D )
				{
					l_return = GetOpenGl().FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0, GetLayer() );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TYPE_2DARRAY:
				l_return = GetOpenGl().FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
				break;
			}

			if ( l_return )
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
		}

		return l_return;
	}

	void GlTextureAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			auto l_pTexture = GetTexture();

			if ( m_glStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				switch ( GetTarget() )
				{
				case eTEXTURE_TYPE_1D:
					GetOpenGl().FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case eTEXTURE_TYPE_2D:
					GetOpenGl().FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case eTEXTURE_TYPE_3D:
					GetOpenGl().FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0, GetLayer() );
					break;

				case eTEXTURE_TYPE_2DARRAY:
					GetOpenGl().FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, 0, 0, GetLayer() );
					break;
				}
			}

			m_glAttachmentPoint = eGL_TEXTURE_ATTACHMENT_NONE;
		}
	}
}
