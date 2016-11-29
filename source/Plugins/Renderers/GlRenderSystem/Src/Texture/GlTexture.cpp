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

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & p_renderSystem,
		TextureType p_type,
		FlagCombination< AccessType > const & p_cpuAccess,
		FlagCombination< AccessType > const & p_gpuAccess )
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

	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & p_renderSystem,
		TextureType p_type,
		FlagCombination< AccessType > const & p_cpuAccess,
		FlagCombination< AccessType > const & p_gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlTexture::GlTexture(
		OpenGl & p_gl,
		GlRenderSystem & p_renderSystem,
		TextureType p_type,
		FlagCombination< AccessType > const & p_cpuAccess,
		FlagCombination< AccessType > const & p_gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: ObjectType{ p_gl,
					  "GlTexture",
					  std::bind( &OpenGl::GenTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteTextures, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsTexture, std::ref( p_gl ), std::placeholders::_1 )
					}
		, TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glDimension{ p_gl.Get( p_type ) }
	{
	}

	GlTexture::~GlTexture()
	{
	}

	void GlTexture::GenerateMipmaps()const
	{
		if ( GetGlName() != GlInvalidIndex && m_type != TextureType::eTwoDimensionsMS && m_type != TextureType::eBuffer )
		{
			GetOpenGl().GenerateMipmap( m_glDimension );
		}
	}

	bool GlTexture::DoBind( uint32_t p_index )const
	{
		bool l_return = GetOpenGl().ActiveTexture( GlTextureIndex( uint32_t( GlTextureIndex::eIndex0 ) + p_index ) );

		if ( l_return )
		{
			l_return = GetOpenGl().BindTexture( m_glDimension, GetGlName() );
		}

		return l_return;
	}

	bool GlTexture::DoInitialise()
	{
		return ObjectType::Create();
	}

	void GlTexture::DoCleanup()
	{
		ObjectType::Destroy();
	}

	void GlTexture::DoUnbind( uint32_t p_index )const
	{
		GetOpenGl().ActiveTexture( GlTextureIndex( uint32_t( GlTextureIndex::eIndex0 ) + p_index ) );
		GetOpenGl().BindTexture( m_glDimension, 0 );
	}
}
