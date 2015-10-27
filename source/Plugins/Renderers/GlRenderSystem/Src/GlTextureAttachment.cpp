#include "GlTextureAttachment.hpp"
#include "GlFrameBuffer.hpp"
#include "GlDynamicTexture.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureAttachment::GlTextureAttachment( OpenGl & p_gl, DynamicTextureSPtr p_pTexture )
		: TextureAttachment( p_pTexture )
		, m_eGlAttachmentPoint( eGL_TEXTURE_ATTACHMENT_NONE )
		, m_eGlStatus( eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
		, m_gl( p_gl )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	bool GlTextureAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			l_return = GetFrameBuffer()->IsComplete();
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_pBuffer );

			if ( l_return )
			{
				l_return = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, std::static_pointer_cast< GlFrameBuffer >( GetFrameBuffer() )->GetGlName() );
			}

			if ( l_return )
			{
				l_return = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = m_gl.ReadBuffer( m_gl.Get( m_eGlAttachmentPoint ) );
			}

			if ( l_return )
			{
				if ( m_eGlAttachmentPoint == eGL_TEXTURE_ATTACHMENT_DEPTH )
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_DEPTH, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else if ( m_eGlAttachmentPoint == eGL_TEXTURE_ATTACHMENT_STENCIL )
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_STENCIL, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_COLOR, m_gl.Get( p_eInterpolation ) );
				}
			}

			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, 0 );
			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, 0 );
		}

		return l_return;
	}

	bool GlTextureAttachment::DoAttach( FrameBufferSPtr p_frameBuffer )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			m_eGlAttachmentPoint = eGL_TEXTURE_ATTACHMENT( m_gl.Get( GetAttachmentPoint() ) + GetAttachmentIndex() );
			GlDynamicTextureSPtr l_pTexture = std::static_pointer_cast< GlDynamicTexture >( GetTexture() );

			switch ( GetTarget() )
			{
			case eTEXTURE_TARGET_1D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_1D )
				{
					l_return = m_gl.FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_2D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_2D )
				{
					l_return = m_gl.FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_3D:

				if ( l_pTexture->GetType() == eTEXTURE_TYPE_3D )
				{
					l_return = m_gl.FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0, GetLayer() );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_LAYER:
				l_return = m_gl.FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
				break;
			}

			if ( l_return )
			{
				m_eGlStatus = eGL_FRAMEBUFFER_STATUS( m_gl.CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) );

				if ( m_eGlStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
				{
					m_eGlStatus = eGL_FRAMEBUFFER_COMPLETE;
				}
			}
			else
			{
				m_eGlStatus = eGL_FRAMEBUFFER_UNSUPPORTED;
			}
		}

		return l_return;
	}

	void GlTextureAttachment::DoDetach()
	{
		if ( m_gl.HasFbo() )
		{
			GlDynamicTextureSPtr l_pTexture = std::static_pointer_cast< GlDynamicTexture >( GetTexture() );

			if ( m_eGlStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				switch ( GetTarget() )
				{
				case eTEXTURE_TARGET_1D:
					m_gl.FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case eTEXTURE_TARGET_2D:
					m_gl.FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case eTEXTURE_TARGET_3D:
					m_gl.FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetType() ), 0, 0, GetLayer() );
					break;

				case eTEXTURE_TARGET_LAYER:
					m_gl.FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, 0, 0, GetLayer() );
					break;
				}
			}

			m_eGlAttachmentPoint = eGL_TEXTURE_ATTACHMENT_NONE;
		}
	}
}
