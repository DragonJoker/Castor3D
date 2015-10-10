#include "GlDynamicTexture.hpp"
#include "GlRenderSystem.hpp"
#include "GlUnpackPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDynamicTexture::GlDynamicTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: DynamicTexture( p_renderSystem )
		, m_uiGlName( uint32_t( eGL_INVALID_INDEX ) )
		, m_pGlRenderSystem( &p_renderSystem )
		, m_gl( p_gl )
	{
	}

	GlDynamicTexture::~GlDynamicTexture()
	{
	}

	bool GlDynamicTexture::Create()
	{
		bool l_return = true;

		if ( m_uiGlName == eGL_INVALID_INDEX )
		{
			l_return = m_gl.GenTextures( 1, &m_uiGlName );
			glTrack( m_gl, GlDynamicTexture, this );
		}

		return l_return;
	}

	void GlDynamicTexture::Destroy()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteTextures( 1, &m_uiGlName );
			m_uiGlName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	void GlDynamicTexture::Cleanup()
	{
		DynamicTexture::Cleanup();
	}

	uint8_t * GlDynamicTexture::Lock( uint32_t p_uiMode )
	{
		return m_pPixelBuffer->ptr();
	}

	void GlDynamicTexture::Unlock( bool p_bModified )
	{
		if ( p_bModified )
		{
			OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );

			switch ( m_eGlDimension )
			{
			case eGL_TEXDIM_1D:
				m_gl.TexSubImage1D( m_eGlDimension, 0, 0, m_pPixelBuffer->width(), l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D:
				m_gl.TexSubImage2D( m_eGlDimension, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
				break;

			case eGL_TEXDIM_3D:
				m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D_ARRAY:
				m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
				break;
			}
		}
	}

	void GlDynamicTexture::GenerateMipmaps()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX && m_eGlDimension != eGL_TEXDIM_2D_MULTISAMPLE )
		{
			m_gl.GenerateMipmap( m_eGlDimension );
		}
	}

	void GlDynamicTexture::Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat )
	{
		OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( p_eFormat );

		switch ( m_eGlDimension )
		{
		case eGL_TEXDIM_1D:
			m_gl.TexImage1D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
			break;

		case eGL_TEXDIM_2D:
			m_gl.TexImage2D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
			break;

		case eGL_TEXDIM_3D:
			m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_pBuffer );
			break;
		}
	}

	bool GlDynamicTexture::DoBind( uint32_t p_index )
	{
		bool l_return = m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );

		if ( l_return )
		{
			l_return = m_gl.BindTexture( m_eGlDimension, m_uiGlName );
		}

		return l_return;
	}

	void GlDynamicTexture::DoUnbind( uint32_t p_index )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		m_gl.BindTexture( m_eGlDimension, 0 );
	}

	bool GlDynamicTexture::DoInitialise()
	{
		OpenGl::PixelFmt l_glPixelFmt;
		bool l_return;
		m_eIndex = eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + m_uiIndex );
		m_eGlDimension = m_gl.Get( m_eDimension );
		l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );
		l_return = DoBind( m_uiIndex );

		switch ( m_eGlDimension )
		{
		case eGL_TEXDIM_1D:
			l_return &= m_gl.TexImage1D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_2D:
			l_return &= m_gl.TexImage2D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_3D:
			l_return &= m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			l_return &= m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_2D_MULTISAMPLE:
			l_return &= m_gl.TexImage2DMultisample( m_eGlDimension, GetSamplesCount(), l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0 );
			break;
		}

		DoUnbind( m_uiIndex );
		return l_return;
	}
}
