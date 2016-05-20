#include "GlTextureStorage.hpp"

#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureStorage::GlTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureStorage{ p_type, p_image, p_cpuAccess, p_gpuAccess }
		, Holder{ p_gl }
		, m_glRenderSystem{ &p_renderSystem }
	{
	}

	GlTextureStorage::~GlTextureStorage()
	{
	}

	void GlTextureStorage::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		DoFill( p_buffer, p_size, p_format );
	}
}
