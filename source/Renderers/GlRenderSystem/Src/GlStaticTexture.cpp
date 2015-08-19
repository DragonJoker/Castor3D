#include "GlStaticTexture.hpp"
#include "GlRenderSystem.hpp"
#include "GlUnpackPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlStaticTexture::GlStaticTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		:	StaticTexture( p_pRenderSystem )
		,	m_uiGlName( uint32_t( eGL_INVALID_INDEX ) )
		,	m_iCurrentPbo( 0 )
		,	m_pGlRenderSystem( p_pRenderSystem )
		,	m_pLockedIoBuffer( NULL )
		,	m_pIoBuffer( NULL )
		,	m_gl( p_gl )
	{
	}

	GlStaticTexture::~GlStaticTexture()
	{
		DoDeletePbos();
	}

	bool GlStaticTexture::Create()
	{
		bool l_bReturn = true;

		if ( m_uiGlName == eGL_INVALID_INDEX )
		{
			l_bReturn = m_gl.GenTextures( 1, &m_uiGlName );
		}

		return l_bReturn;
	}

	void GlStaticTexture::Destroy()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_gl.DeleteTextures( 1, &m_uiGlName );
			m_uiGlName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	void GlStaticTexture::Cleanup()
	{
		StaticTexture::Cleanup();
		DoCleanupPbos();
		DoDeletePbos();
	}

	uint8_t * GlStaticTexture::Lock( uint32_t p_uiMode )
	{
		uint8_t * l_pReturn = NULL;
		eGL_LOCK l_eLock = m_gl.GetLockFlags( p_uiMode );

		if ( !m_pLockedIoBuffer )
		{
			m_pLockedIoBuffer = m_pIoBuffer;

			if ( m_pLockedIoBuffer )
			{
				m_pLockedIoBuffer->Activate();
				m_pLockedIoBuffer->Fill( NULL, m_pPixelBuffer->size() );
				l_pReturn = m_pLockedIoBuffer->Lock( l_eLock );
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Can't lock image : it is already locked" ) );
		}

		return l_pReturn;
	}

	void GlStaticTexture::Unlock( bool p_bModified )
	{
		if ( m_pLockedIoBuffer )
		{
			m_pLockedIoBuffer->Unlock();

			if ( p_bModified )
			{
				OpenGl::PixelFmt l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );

				switch ( m_eGlDimension )
				{
				case eGL_TEXDIM_1D:
					m_gl.TexSubImage1D( m_eGlDimension, 0, 0, m_pPixelBuffer->width(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
					break;

				case eGL_TEXDIM_2D:
					m_gl.TexSubImage2D( m_eGlDimension, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
					break;

				case eGL_TEXDIM_3D:
					m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
					break;

				case eGL_TEXDIM_2D_ARRAY:
					m_gl.TexSubImage3D( m_eGlDimension, 0, 0, 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, l_glPixelFmt.Format, l_glPixelFmt.Type, NULL );
					break;
				}
			}

			m_pLockedIoBuffer->Deactivate();
			m_pLockedIoBuffer = NULL;
		}
		else
		{
			Logger::LogWarning( cuT( "Can't unlock image : it is not locked" ) );
		}
	}

	void GlStaticTexture::GenerateMipmaps()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			m_gl.GenerateMipmap( m_eGlDimension );
		}
	}

	bool GlStaticTexture::DoBind( uint32_t p_index )
	{
		bool l_bReturn = m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );

		if ( l_bReturn )
		{
			l_bReturn = m_gl.BindTexture( m_eGlDimension, m_uiGlName );
		}

		return l_bReturn;
	}

	void GlStaticTexture::DoUnbind( uint32_t p_index )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		m_gl.BindTexture( m_eGlDimension, 0 );
	}

	bool GlStaticTexture::DoInitialise()
	{
		OpenGl::PixelFmt l_glPixelFmt;
		bool l_bReturn;
		DoInitialisePbos();
		m_eIndex = eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + m_uiIndex );
		m_eGlDimension = m_gl.Get( m_eDimension );
		l_glPixelFmt = m_gl.Get( m_pPixelBuffer->format() );
		l_bReturn = DoBind( m_uiIndex );

		switch ( m_eGlDimension )
		{
		case eGL_TEXDIM_1D:
			l_bReturn &= m_gl.TexImage1D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_2D:
			l_bReturn &= m_gl.TexImage2D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_3D:
			l_bReturn &= m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			l_bReturn &= m_gl.TexImage3D( m_eGlDimension, 0, l_glPixelFmt.Internal, m_pPixelBuffer->width(), m_pPixelBuffer->height() / m_uiDepth, m_uiDepth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, m_pPixelBuffer->const_ptr() );
			break;
		}

		DoUnbind( m_uiIndex );
		return l_bReturn;
	}

	void GlStaticTexture::DoDeletePbos()
	{
		if ( m_pIoBuffer )
		{
			delete m_pIoBuffer;
			m_pIoBuffer = NULL;
		}

		m_pLockedIoBuffer = NULL;
	}

	void GlStaticTexture::DoCleanupPbos()
	{
		if ( m_pIoBuffer )
		{
			m_pIoBuffer->Destroy();
		}

		m_pLockedIoBuffer = NULL;
	}

	void GlStaticTexture::DoInitialisePbos()
	{
		m_pIoBuffer = new GlUnpackPixelBuffer( m_gl, m_pGlRenderSystem, m_pPixelBuffer );
		m_pIoBuffer->Initialise();
	}
}
