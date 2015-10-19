#include "GlStaticTexture.hpp"

#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlStaticTexture::GlStaticTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: StaticTexture( p_renderSystem )
		, m_texture( p_gl, p_renderSystem, true )
	{
	}

	GlStaticTexture::~GlStaticTexture()
	{
	}

	bool GlStaticTexture::Create()
	{
		return m_texture.Create();
	}

	void GlStaticTexture::Destroy()
	{
		m_texture.Destroy();
	}

	void GlStaticTexture::Cleanup()
	{
		StaticTexture::Cleanup();
		m_texture.Cleanup();
	}

	uint8_t * GlStaticTexture::Lock( uint32_t p_mode )
	{
		return m_texture.Lock( p_mode );
	}

	void GlStaticTexture::Unlock( bool p_modified )
	{
		m_texture.Unlock( p_modified );
	}

	void GlStaticTexture::GenerateMipmaps()
	{
		m_texture.GenerateMipmaps();
	}

	bool GlStaticTexture::DoBind( uint32_t p_index )
	{
		return m_texture.Bind( p_index );
	}

	void GlStaticTexture::DoUnbind( uint32_t p_index )
	{
		m_texture.Unbind( p_index );
	}

	bool GlStaticTexture::DoInitialise()
	{
		return m_texture.Initialise( m_pPixelBuffer, m_type, m_uiDepth, m_cpuAccess, m_gpuAccess );
	}
}
