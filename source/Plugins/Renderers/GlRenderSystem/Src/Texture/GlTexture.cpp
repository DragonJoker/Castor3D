#include "Texture/GlTexture.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Texture/GlDirectTextureStorage.hpp"
#include "Texture/GlGpuOnlyTextureStorage.hpp"
#include "Texture/GlImmutableTextureStorage.hpp"
#include "Texture/GlPboTextureStorage.hpp"
#include "Texture/GlTboTextureStorage.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ renderSystem, p_type, p_cpuAccess, p_gpuAccess }
		, m_glRenderSystem{ &renderSystem }
		, m_glDimension{ p_gl.get( p_type ) }
	{
	}

	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &renderSystem }
		, m_glDimension{ p_gl.get( p_type ) }
	{
	}

	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &renderSystem }
		, m_glDimension{ p_gl.get( p_type ) }
	{
	}

	GlTexture::~GlTexture()
	{
	}

	void GlTexture::generateMipmaps()const
	{
		if ( getGlName() != GlInvalidIndex && m_type != TextureType::eTwoDimensionsMS && m_type != TextureType::eBuffer )
		{
			getOpenGl().GenerateMipmap( m_glDimension );
		}
	}

	bool GlTexture::doInitialise()
	{
		return ObjectType::create();
	}

	void GlTexture::doCleanup()
	{
		ObjectType::destroy();
	}

	void GlTexture::doBind( uint32_t p_index )const
	{
		getOpenGl().ActiveTexture( GlTextureIndex( uint32_t( GlTextureIndex::eIndex0 ) + p_index ) );
		getOpenGl().BindTexture( m_glDimension, getGlName() );
		glTrackTexture( getGlName(), p_index );
	}

	void GlTexture::doUnbind( uint32_t p_index )const
	{
		glTrackTexture( 0u, p_index );
		getOpenGl().ActiveTexture( GlTextureIndex( uint32_t( GlTextureIndex::eIndex0 ) + p_index ) );
		getOpenGl().BindTexture( m_glDimension, 0 );
	}
}
