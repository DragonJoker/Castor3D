#include "GlDynamicTexture.hpp"

#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDynamicTexture::GlDynamicTexture( OpenGl & p_gl, eTEXTURE_TYPE p_type, GlRenderSystem & p_renderSystem );
		: DynamicTexture{ p_type, p_renderSystem }
		, m_texture{ p_gl, p_renderSystem, false }
	{
	}

	GlDynamicTexture::~GlDynamicTexture()
	{
	}

	bool GlDynamicTexture::Create()
	{
		return m_texture.Create();
	}

	void GlDynamicTexture::Destroy()
	{
		m_texture.Destroy();
	}

	void GlDynamicTexture::Cleanup()
	{
		DynamicTexture::Cleanup();
		m_texture.Cleanup();
	}

	uint8_t * GlDynamicTexture::Lock( uint32_t p_mode )
	{
		return m_texture.Lock( p_mode );
	}

	void GlDynamicTexture::Unlock( bool p_modified )
	{
		m_texture.Unlock( p_modified );
	}

	void GlDynamicTexture::GenerateMipmaps()const
	{
		m_texture.GenerateMipmaps();
	}

	void GlDynamicTexture::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_texture.Fill( p_buffer, p_size, p_format );
	}

	bool GlDynamicTexture::DoBind( uint32_t p_index )const
	{
		return m_texture.Bind( p_index );
	}

	void GlDynamicTexture::DoUnbind( uint32_t p_index )const
	{
		m_texture.Unbind( p_index );
	}

	bool GlDynamicTexture::DoInitialise()
	{
		return m_texture.Initialise( m_pixelBuffer, m_type, m_depth, m_cpuAccess, m_gpuAccess );
	}
}
