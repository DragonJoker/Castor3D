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
	GlTexture::GlTexture( OpenGl & p_gl, eTEXTURE_TYPE p_type, GlRenderSystem & p_renderSystem )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlTexture::~GlTexture()
	{
	}

	bool GlTexture::Create()
	{
		return ObjectType::Create();
	}

	void GlTexture::Destroy()
	{
		ObjectType::Destroy();
	}

	void GlTexture::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		REQUIRE( m_storage );
		m_storage->Fill( p_buffer, p_size, p_format );
	}

	void GlTexture::GenerateMipmaps()const
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_glDimension != eGL_TEXDIM_2DMS && m_glDimension != eGL_TEXDIM_BUFFER )
		{
			GetOpenGl().GenerateMipmap( m_glDimension );
		}
	}

	bool GlTexture::DoInitialise()
	{
		bool l_return = GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + 0 ) );

		if ( l_return )
		{
			l_return = GetOpenGl().BindTexture( m_glDimension, GetGlName() );

			if ( l_return )
			{
				m_storage->Create( p_buffer );

				if ( l_return )
				{
					l_return = m_storage->Initialise( p_layer );
				}

				GetOpenGl().BindTexture( m_glDimension, 0 );
			}
		}

		return l_return;
	}

	void GlTexture::DoCleanup()
	{
	}

	bool GlTexture::DoBind( uint32_t p_index )const
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

	void GlTexture::DoUnbind( uint32_t p_index )const
	{
		REQUIRE( m_storage );
		m_storage->Unbind( p_index );
		GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		GetOpenGl().BindTexture( m_glDimension, 0 );
	}
}
