#include "GlTexture.hpp"

#include "GlDirectTextureStorage.hpp"
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
	GlTexture::GlTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem, bool p_static )
		: Object( p_gl,
				  "GlTexture",
				  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
				  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
				  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
				  )
		, m_glRenderSystem( &p_renderSystem )
		, m_glDimension( eGL_TEXDIM_2D )
		, m_static( p_static )
	{
	}

	GlTexture::~GlTexture()
	{
	}

	bool GlTexture::Initialise( PxBufferBaseSPtr p_buffer, eTEXTURE_TYPE p_dimension, uint32_t p_layer, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		REQUIRE( !m_storage );
		m_glDimension = GetOpenGl().Get( p_dimension );

		if ( m_glDimension == eGL_TEXDIM_BUFFER )
		{
			if ( GetOpenGl().HasTbo() )
			{
				m_storage = std::make_unique< GlTboTextureStorage >( GetOpenGl(), *m_glRenderSystem );
			}
			else
			{
				m_glDimension = eGL_TEXDIM_1D;
			}
		}

		if ( !m_storage )
		{
			if ( m_static )
			{
				m_storage = std::make_unique< GlDirectTextureStorage >( GetOpenGl(), *m_glRenderSystem );
			}
			else
			{
				m_storage = std::make_unique< GlPboTextureStorage >( GetOpenGl(), *m_glRenderSystem );
			}
		}

		bool l_return = GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + 0 ) );

		if ( l_return )
		{
			l_return = GetOpenGl().BindTexture( m_glDimension, GetGlName() );
		}

		if ( l_return )
		{
			m_storage->Create( p_buffer );

			if ( l_return )
			{
				l_return = m_storage->Initialise( p_dimension, p_layer );
			}

			GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + 0 ) );
			GetOpenGl().BindTexture( m_glDimension, 0 );
		}

		return l_return;
	}

	void GlTexture::Cleanup()
	{
		if ( m_storage )
		{
			m_storage->Cleanup();
			m_storage->Destroy();
			m_storage.reset();
		}
	}

	void GlTexture::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		REQUIRE( m_storage );
		m_storage->Fill( p_buffer, p_size, p_format );
	}

	bool GlTexture::Bind( uint32_t p_index )
	{
		REQUIRE( m_storage );
		bool l_return = GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );

		if ( l_return )
		{
			l_return = GetOpenGl().BindTexture( m_glDimension, GetGlName() );
		}

		if ( l_return )
		{
			m_storage->Bind( p_index );
		}

		return l_return;
	}

	void GlTexture::Unbind( uint32_t p_index )
	{
		REQUIRE( m_storage );
		m_storage->Unbind( p_index );
		GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		GetOpenGl().BindTexture( m_glDimension, 0 );
	}

	uint8_t * GlTexture::Lock( uint32_t p_lock )
	{
		REQUIRE( m_storage );
		return m_storage->Lock( p_lock );
	}

	void GlTexture::Unlock( bool p_modified )
	{
		REQUIRE( m_storage );
		m_storage->Unlock( p_modified );
	}

	void GlTexture::GenerateMipmaps()
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_glDimension != eGL_TEXDIM_2DMS && m_glDimension != eGL_TEXDIM_BUFFER )
		{
			GetOpenGl().GenerateMipmap( m_glDimension );
		}
	}
}
