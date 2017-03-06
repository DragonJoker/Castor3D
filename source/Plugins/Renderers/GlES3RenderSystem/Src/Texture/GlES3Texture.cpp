#include "Texture/GlES3Texture.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Texture/GlES3DirectTextureStorage.hpp"
#include "Texture/GlES3GpuOnlyTextureStorage.hpp"
#include "Texture/GlES3ImmutableTextureStorage.hpp"
#include "Texture/GlES3PboTextureStorage.hpp"
#include "Texture/GlES3TboTextureStorage.hpp"
#include "Texture/GlES3TextureStorage.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3Texture::GlES3Texture(
		OpenGlES3 & p_gl,
		GlES3RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess )
		: ObjectType{ p_gl,
					  "GlES3Texture",
					  std::bind( &OpenGlES3::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlES3Texture::GlES3Texture(
		OpenGlES3 & p_gl,
		GlES3RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: ObjectType{ p_gl,
					  "GlES3Texture",
					  std::bind( &OpenGlES3::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlES3Texture::GlES3Texture(
		OpenGlES3 & p_gl,
		GlES3RenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: ObjectType{ p_gl,
					  "GlES3Texture",
					  std::bind( &OpenGlES3::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlES3Texture::~GlES3Texture()
	{
	}

	void GlES3Texture::GenerateMipmaps()const
	{
		if ( GetGlES3Name() != GlES3InvalidIndex && m_type != TextureType::eTwoDimensionsMS && m_type != TextureType::eBuffer )
		{
			GetOpenGlES3().GenerateMipmap( m_glDimension );
		}
	}

	bool GlES3Texture::DoInitialise()
	{
		return ObjectType::Create();
	}

	void GlES3Texture::DoCleanup()
	{
		ObjectType::Destroy();
	}

	void GlES3Texture::DoBind( uint32_t p_index )const
	{
		GetOpenGlES3().ActiveTexture( GlES3TextureIndex( uint32_t( GlES3TextureIndex::eIndex0 ) + p_index ) );
		GetOpenGlES3().BindTexture( m_glDimension, GetGlES3Name() );
		glTrackTexture( GetGlES3Name(), p_index );
	}

	void GlES3Texture::DoUnbind( uint32_t p_index )const
	{
		glTrackTexture( 0u, p_index );
		GetOpenGlES3().ActiveTexture( GlES3TextureIndex( uint32_t( GlES3TextureIndex::eIndex0 ) + p_index ) );
		GetOpenGlES3().BindTexture( m_glDimension, 0 );
	}
}
