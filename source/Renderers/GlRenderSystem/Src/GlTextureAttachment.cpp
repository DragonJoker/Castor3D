#include "GlTextureAttachment.hpp"
#include "GlFrameBuffer.hpp"
#include "GlDynamicTexture.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureAttachment::GlTextureAttachment( OpenGl & p_gl, DynamicTextureSPtr p_pTexture )
		:	TextureAttachment( p_pTexture )
		,	m_eGlAttachmentPoint( eGL_TEXTURE_ATTACHMENT_NONE )
		,	m_eGlStatus( eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
		,	m_gl( p_gl )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	bool GlTextureAttachment::DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
	{
		bool l_bReturn = false;

		if ( m_gl.HasFbo() )
		{
			FrameBufferSPtr l_pFrameBuffer = GetFrameBuffer();
			DynamicTextureSPtr l_pTexture = GetTexture();
			l_bReturn = l_pFrameBuffer != nullptr && l_pTexture != nullptr;

			if ( l_bReturn && l_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ ) )
			{
				m_gl.ReadBuffer( m_gl.Get( m_eGlAttachmentPoint ) );
				OpenGl::PixelFmt l_pxFmt = m_gl.Get( p_pBuffer->format() );
				l_bReturn = m_gl.ReadPixels( Position(), l_pTexture->GetDimensions(), l_pxFmt.Format, l_pxFmt.Type, p_pBuffer->ptr() );
				l_pFrameBuffer->Unbind();
			}
		}

		return l_bReturn;
	}

	bool GlTextureAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation )
	{
		bool l_bReturn = false;

		if ( m_gl.HasFbo() )
		{
			l_bReturn = m_eGlStatus == eGL_FRAMEBUFFER_COMPLETE;
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_pBuffer );

			if ( l_bReturn )
			{
				l_bReturn = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, m_pGlFrameBuffer.lock()->GetGlName() );
			}

			if ( l_bReturn )
			{
				l_bReturn = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, l_pBuffer->GetGlName() );
			}

			if ( l_bReturn )
			{
				l_bReturn = m_gl.ReadBuffer( m_gl.Get( m_eGlAttachmentPoint ) );
			}

			if ( l_bReturn )
			{
				if ( m_eGlAttachmentPoint == eGL_TEXTURE_ATTACHMENT_DEPTH )
				{
					l_bReturn = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_DEPTH, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else if ( m_eGlAttachmentPoint == eGL_TEXTURE_ATTACHMENT_STENCIL )
				{
					l_bReturn = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_STENCIL, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else
				{
					l_bReturn = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_BUFFER_BIT_COLOR, m_gl.Get( p_eInterpolation ) );
				}
			}

			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, 0 );
			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, 0 );
		}

		return l_bReturn;
	}

	bool GlTextureAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		bool l_bReturn = false;

		if ( m_gl.HasFbo() )
		{
			m_eGlAttachmentPoint = m_gl.Get( p_eAttachment );
			GlDynamicTextureSPtr l_pTexture = std::static_pointer_cast< GlDynamicTexture >( GetTexture() );
			m_pGlFrameBuffer = std::static_pointer_cast< GlFrameBuffer >( p_pFrameBuffer );

			switch ( GetAttachedTarget() )
			{
			case eTEXTURE_TARGET_1D:
				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_1D )
				{
					l_bReturn = m_gl.FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_bReturn = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_2D:
				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_2D )
				{
					l_bReturn = m_gl.FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_bReturn = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_3D:
				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_3D )
				{
					l_bReturn = m_gl.FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0, GetAttachedLayer() );
				}
				else
				{
					l_bReturn = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_LAYER:
				l_bReturn = m_gl.FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, l_pTexture->GetGlName(), 0, GetAttachedLayer() );
				break;
			}

			if ( l_bReturn )
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

		return l_bReturn;
	}

	void GlTextureAttachment::DoDetach()
	{
		if ( m_gl.HasFbo() )
		{
			GlDynamicTextureSPtr l_pTexture = std::static_pointer_cast< GlDynamicTexture >( GetTexture() );

			if ( m_eGlStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				switch ( GetAttachedTarget() )
				{
				case eTEXTURE_TARGET_1D:
					m_gl.FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), 0, 0 );
					break;

				case eTEXTURE_TARGET_2D:
					m_gl.FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), 0, 0 );
					break;

				case eTEXTURE_TARGET_3D:
					m_gl.FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), 0, 0, GetAttachedLayer() );
					break;

				case eTEXTURE_TARGET_LAYER:
					m_gl.FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, 0, 0, GetAttachedLayer() );
					break;
				}
			}

			m_eGlAttachmentPoint = eGL_TEXTURE_ATTACHMENT_NONE;
		}
	}
}
