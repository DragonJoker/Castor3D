#include "GlFrameBuffer.hpp"
#include "GlBuffer.hpp"
#include "GlColourRenderBuffer.hpp"
#include "GlDepthStencilRenderBuffer.hpp"
#include "GlRenderBufferAttachment.hpp"
#include "GlTextureAttachment.hpp"
#include "GlDynamicTexture.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameBuffer::GlFrameBuffer( OpenGl & p_gl, Engine * p_pEngine )
		:	FrameBuffer( p_pEngine )
		,	m_uiGlName( uint32_t( eGL_INVALID_INDEX ) )
		,	m_gl( p_gl )
	{
	}

	GlFrameBuffer::~GlFrameBuffer()
	{
	}

	bool GlFrameBuffer::Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
	{
		if ( m_gl.HasFbo() && m_uiGlName == eGL_INVALID_INDEX )
		{
			m_gl.GenFramebuffers( 1, &m_uiGlName );
			glTrack( m_gl, GlFrameBuffer, this );
		}

		return m_uiGlName != eGL_INVALID_INDEX;
	}

	void GlFrameBuffer::Destroy()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteFramebuffers( 1, &m_uiGlName );
			m_uiGlName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	bool GlFrameBuffer::SetDrawBuffers( uint32_t p_uiSize, eATTACHMENT_POINT const * p_eAttaches )
	{
		bool l_return = false;

		if ( m_arrayGlAttaches.size() )
		{
			l_return = SetDrawBuffers();
		}
		else if ( p_uiSize )
		{
			UIntArray l_arrayAttaches;
			l_arrayAttaches.reserve( p_uiSize );

			for ( uint32_t i = 0; i < p_uiSize; i++ )
			{
				if ( p_eAttaches[i] >= eATTACHMENT_POINT_COLOUR0 && p_eAttaches[i] < eATTACHMENT_POINT_DEPTH )
				{
					l_arrayAttaches.push_back( m_gl.Get( p_eAttaches[i] ) );
				}
			}

			l_return = m_gl.DrawBuffers( int( l_arrayAttaches.size() ), &l_arrayAttaches[0] );
		}

		return l_return;
	}

	bool GlFrameBuffer::SetDrawBuffers()
	{
		bool l_return = false;

		if ( m_arrayGlAttaches.size() )
		{
			l_return = m_gl.DrawBuffers( int( m_arrayGlAttaches.size() ), &m_arrayGlAttaches[0] );
		}

		return l_return;
	}

	bool GlFrameBuffer::SetReadBuffer( eATTACHMENT_POINT p_eAttach )
	{
		bool l_return = false;
		eGL_BUFFER l_eAttach = m_gl.Get( m_gl.Get( p_eAttach ) );
		UIntArrayIt l_it = std::find( m_arrayGlAttaches.begin(), m_arrayGlAttaches.end(), l_eAttach );

		if ( l_it != m_arrayGlAttaches.end() )
		{
			m_gl.ReadBuffer( l_eAttach );
		}

		return l_return;
	}

	bool GlFrameBuffer::IsComplete()
	{
		return eGL_FRAMEBUFFER_STATUS( m_gl.CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) ) == eGL_FRAMEBUFFER_COMPLETE;
	}

	ColourRenderBufferSPtr GlFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< GlColourRenderBuffer >( m_gl, p_ePixelFormat );
	}

	DepthStencilRenderBufferSPtr GlFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< GlDepthStencilRenderBuffer >( m_gl, p_ePixelFormat );
	}

	bool GlFrameBuffer::DoBind( eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_return = false;

		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_eGlBindingMode = m_gl.Get( p_eTarget );
			l_return = m_gl.BindFramebuffer( m_eGlBindingMode, m_uiGlName );
		}

		return l_return;
	}

	void GlFrameBuffer::DoUnbind()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_gl.BindFramebuffer( m_eGlBindingMode, 0 );
		}
	}

	void GlFrameBuffer::DoAttachFba( FrameBufferAttachmentRPtr p_pAttach )
	{
		DoGlAttach( p_pAttach->GetAttachmentPoint() );
	}

	void GlFrameBuffer::DoDetachFba( FrameBufferAttachmentRPtr p_pAttach )
	{
		DoGlDetach( p_pAttach->GetAttachmentPoint() );
	}

	bool GlFrameBuffer::DoAttach( eATTACHMENT_POINT p_eAttachment, DynamicTextureSPtr p_pTexture, eTEXTURE_TARGET p_eTarget, int p_iLayer )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			eGL_TEXTURE_ATTACHMENT l_eGlAttachmentPoint = m_gl.Get( p_eAttachment );
			GlDynamicTextureSPtr l_pTexture = std::static_pointer_cast< GlDynamicTexture >( p_pTexture );

			switch ( p_eTarget )
			{
			case eTEXTURE_TARGET_1D:

				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_1D )
				{
					l_return = m_gl.FramebufferTexture1D( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_2D:

				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_2D )
				{
					l_return = m_gl.FramebufferTexture2D( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_3D:

				if ( l_pTexture->GetDimension() == eTEXTURE_DIMENSION_3D )
				{
					l_return = m_gl.FramebufferTexture3D( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, m_gl.Get( l_pTexture->GetDimension() ), l_pTexture->GetGlName(), 0, p_iLayer );
				}
				else
				{
					l_return = m_gl.FramebufferTexture( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case eTEXTURE_TARGET_LAYER:
				l_return = m_gl.FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, l_pTexture->GetGlName(), 0, p_iLayer );
				break;
			}
		}

		if ( l_return )
		{
			DoGlAttach( p_eAttachment );
		}

		return l_return;
	}

	bool GlFrameBuffer::DoAttach( eATTACHMENT_POINT p_eAttachment, RenderBufferSPtr p_pRenderBuffer )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			eGL_RENDERBUFFER_ATTACHMENT l_eGlAttachmentPoint = m_gl.GetRboAttachment( p_eAttachment );
			uint32_t l_uiGlName;

			switch ( p_pRenderBuffer->GetComponent() )
			{
			case eBUFFER_COMPONENT_COLOUR:
				l_uiGlName = std::static_pointer_cast< GlColourRenderBuffer			>( p_pRenderBuffer )->GetGlName();
				break;

			case eBUFFER_COMPONENT_DEPTH:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer	>( p_pRenderBuffer )->GetGlName();
				break;

			case eBUFFER_COMPONENT_STENCIL:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer	>( p_pRenderBuffer )->GetGlName();
				break;

			default:
				l_uiGlName = eGL_INVALID_INDEX;
				break;
			}

			if ( l_uiGlName != eGL_INVALID_INDEX )
			{
				l_return = m_gl.FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE_DEFAULT, l_eGlAttachmentPoint, eGL_RENDERBUFFER_MODE_DEFAULT, l_uiGlName );
			}
		}

		if ( l_return )
		{
			DoGlAttach( p_eAttachment );
		}

		return l_return;
	}

	void GlFrameBuffer::DoDetachAll()
	{
		m_arrayGlAttaches.clear();
	}

	bool GlFrameBuffer::DoStretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolationMode )
	{
		return m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, m_gl.GetComponents( p_uiComponents ), m_gl.Get( p_eInterpolationMode ) );
	}

	void GlFrameBuffer::DoGlAttach( eATTACHMENT_POINT p_eAttachment )
	{
		if ( p_eAttachment != eATTACHMENT_POINT_DEPTH && p_eAttachment != eATTACHMENT_POINT_STENCIL )
		{
			DoGlDetach( p_eAttachment );
			uint32_t l_uiAttach = m_gl.Get( p_eAttachment );
			m_arrayGlAttaches.push_back( l_uiAttach );
		}
	}

	void GlFrameBuffer::DoGlDetach( eATTACHMENT_POINT p_eAttachment )
	{
		uint32_t l_uiAttach = m_gl.Get( p_eAttachment );
		UIntArrayIt l_it = std::find( m_arrayGlAttaches.begin(), m_arrayGlAttaches.end(), l_uiAttach );

		if ( l_it != m_arrayGlAttaches.end() )
		{
			m_arrayGlAttaches.erase( l_it );
		}
	}
}
