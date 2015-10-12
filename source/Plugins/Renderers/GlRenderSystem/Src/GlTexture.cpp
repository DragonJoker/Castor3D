#include "GlTexture.hpp"

#include "GlPboTextureStorage.hpp"
#include "GlRenderSystem.hpp"
#include "GlTboTextureStorage.hpp"
#include "GlTextureStorage.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTexture::GlTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: m_glName( uint32_t( eGL_INVALID_INDEX ) )
		, m_gl( p_gl )
		, m_glRenderSystem( &p_renderSystem )
		, m_glDimension( eGL_TEXDIM_2D )
	{
	}

	GlTexture::~GlTexture()
	{
	}

	bool GlTexture::Create()
	{
		bool l_return = true;

		if ( m_glName == eGL_INVALID_INDEX )
		{
			l_return = m_gl.GenTextures( 1, &m_glName );
			glTrack( m_gl, GlStaticTexture, this );
		}

		return l_return;
	}

	void GlTexture::Destroy()
	{
		m_storage.reset();

		if ( m_glName != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteTextures( 1, &m_glName );
			m_glName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	bool GlTexture::Initialise( PxBufferBaseSPtr p_buffer, eTEXTURE_TYPE p_dimension, uint32_t p_layer )
	{
		m_glDimension = m_gl.Get( p_dimension );

		if ( m_glDimension == eGL_TEXDIM_BUFFER )
		{
			if ( m_gl.HasTbo() )
			{
				m_storage = std::make_unique< GlTboTextureStorage >( m_gl, *m_glRenderSystem );
			}
			else
			{
				m_glDimension = eGL_TEXDIM_1D;
				m_storage = std::make_unique< GlPboTextureStorage >( m_gl, *m_glRenderSystem );
			}
		}
		else
		{
			m_storage = std::make_unique< GlPboTextureStorage >( m_gl, *m_glRenderSystem );
		}

		bool l_return = m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + 0 ) );

		if ( l_return )
		{
			l_return = m_gl.BindTexture( m_glDimension, m_glName );
		}

		if ( l_return )
		{
			m_storage->Create( p_buffer );

			if ( l_return )
			{
				l_return = m_storage->Initialise( p_dimension, p_layer );
			}

			m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + 0 ) );
			m_gl.BindTexture( m_glDimension, 0 );
		}
		return l_return;
	}

	void GlTexture::Cleanup()
	{
		m_storage->Cleanup();
		m_storage->Destroy();
	}

	void GlTexture::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_storage->Fill( p_buffer, p_size, p_format );
	}

	bool GlTexture::Bind( uint32_t p_index )
	{
		bool l_return = m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );

		if ( l_return )
		{
			l_return = m_gl.BindTexture( m_glDimension, m_glName );
		}

		if ( l_return )
		{
			m_storage->Bind( p_index );
		}

		return l_return;
	}

	void GlTexture::Unbind( uint32_t p_index )
	{
		m_storage->Unbind( p_index );
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		m_gl.BindTexture( m_glDimension, 0 );
	}

	uint8_t * GlTexture::Lock( uint32_t p_lock )
	{
		return m_storage->Lock( p_lock );
	}

	void GlTexture::Unlock( bool p_modified )
	{
		m_storage->Unlock( p_modified );
	}

	void GlTexture::GenerateMipmaps()
	{
		if ( m_glName != eGL_INVALID_INDEX && m_glDimension != eGL_TEXDIM_2DMS && m_glDimension != eGL_TEXDIM_BUFFER )
		{
			m_gl.GenerateMipmap( m_glDimension );
		}
	}
}
