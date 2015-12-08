#include "GlTextureStorage.hpp"

#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureStorage::GlTextureStorage( OpenGl & p_gl, GlTexture & p_texture, GlRenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: OwnedBy< GlTexture >( p_texture )
		, Holder( p_gl )
		, m_glRenderSystem( &p_renderSystem )
		, m_depth( 0 )
		, m_cpuAccess( p_cpuAccess )
		, m_gpuAccess( p_gpuAccess )
	{
	}

	GlTextureStorage::~GlTextureStorage()
	{
	}

	bool GlTextureStorage::Create( PxBufferBaseSPtr p_buffer )
	{
		m_buffer = p_buffer;
		return DoCreate();
	}

	void GlTextureStorage::Destroy()
	{
		DoDestroy();
		m_buffer.reset();
	}

	bool GlTextureStorage::Initialise( uint32_t p_depth )
	{
		m_depth = p_depth;
		return DoInitialise();
	}

	void GlTextureStorage::Cleanup()
	{
		DoCleanup();
	}

	void GlTextureStorage::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		DoFill( p_buffer, p_size, p_format );
	}

	bool GlTextureStorage::Bind( uint32_t p_index )
	{
		return DoBind( p_index );
	}

	void GlTextureStorage::Unbind( uint32_t p_index )
	{
		DoUnbind( p_index );
	}

	uint8_t * GlTextureStorage::Lock( uint32_t p_lock )
	{
		return DoLock( p_lock );
	}

	void GlTextureStorage::Unlock( bool p_modified )
	{
		DoUnlock( p_modified );
	}
}
