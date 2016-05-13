#include "GlCubeMapTexture.hpp"

#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlCubeMapTexture::GlCubeMapTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: CubeMapTexture( p_renderSystem, eACCESS_TYPE_READ, eACCESS_TYPE_READ )
		, m_texture( p_gl, p_renderSystem, true )
	{
	}

	GlCubeMapTexture::~GlCubeMapTexture()
	{
	}

	bool GlCubeMapTexture::Create()
	{
		return m_texture.Create();
	}

	void GlCubeMapTexture::Destroy()
	{
		m_texture.Destroy();
	}

	void GlCubeMapTexture::Cleanup()
	{
		CubeMapTexture::Cleanup();
		m_texture.Cleanup();
	}

	uint8_t * GlCubeMapTexture::Lock( uint32_t p_mode )
	{
		return m_texture.Lock( p_mode );
	}

	void GlCubeMapTexture::Unlock( bool p_modified )
	{
		m_texture.Unlock( p_modified );
	}

	void GlCubeMapTexture::GenerateMipmaps()const
	{
		m_texture.GenerateMipmaps();
	}

	bool GlCubeMapTexture::DoBind( uint32_t p_index )const
	{
		return m_texture.Bind( p_index );
	}

	void GlCubeMapTexture::DoUnbind( uint32_t p_index )const
	{
		m_texture.Unbind( p_index );
	}

	bool GlCubeMapTexture::DoInitialise()
	{
		return m_texture.Initialise( m_pixelBuffers[0], m_type, m_depth, m_cpuAccess, m_gpuAccess );
	}
}
