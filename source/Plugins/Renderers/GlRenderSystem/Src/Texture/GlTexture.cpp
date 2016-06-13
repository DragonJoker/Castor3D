#include "Texture/GlTexture.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Texture/GlDirectTextureStorage.hpp"
#include "Texture/GlPboTextureStorage.hpp"
#include "Texture/GlTboTextureStorage.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTexture::GlTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess }
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

	void GlTexture::GenerateMipmaps()const
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_type != TextureType::TwoDimensionsMS && m_type != TextureType::Buffer )
		{
			GetOpenGl().GenerateMipmap( m_glDimension );
		}
	}

	bool GlTexture::DoBind( uint32_t p_index )const
	{
		bool l_return = GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );

		if ( l_return )
		{
			l_return = GetOpenGl().BindTexture( m_glDimension, GetGlName() );
		}

		return l_return;
	}

	void GlTexture::DoUnbind( uint32_t p_index )const
	{
		GetOpenGl().ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + p_index ) );
		GetOpenGl().BindTexture( m_glDimension, 0 );
	}
}
