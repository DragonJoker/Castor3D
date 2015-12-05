#include "GlTboTextureStorage.hpp"

#include "OpenGl.hpp"

#include <Logger.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTboTextureStorage::GlTboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: GlTextureStorage( p_gl, p_renderSystem )
		, m_glBuffer( p_gl, eGL_BUFFER_TARGET_TEXTURE )
	{
	}

	GlTboTextureStorage::~GlTboTextureStorage()
	{
	}

	bool GlTboTextureStorage::DoCreate()
	{
		return m_glBuffer.Create();
	}

	void GlTboTextureStorage::DoDestroy()
	{
		m_glBuffer.Destroy();
	}

	bool GlTboTextureStorage::DoInitialise()
	{
		PxBufferBaseSPtr l_buffer = m_buffer.lock();
		m_glDimension = eGL_TEXDIM_BUFFER;
		m_glInternal = GetOpenGl().GetInternal( l_buffer->format() );
		return m_glBuffer.Initialise( l_buffer->const_ptr(), l_buffer->size(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	}

	void GlTboTextureStorage::DoCleanup()
	{
		m_glBuffer.Cleanup();
	}

	void GlTboTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_glBuffer.Fill( p_buffer, p_size.width() * p_size.height() * PF::GetBytesPerPixel( p_format ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	}

	bool GlTboTextureStorage::DoBind( uint32_t p_index )
	{
		return GetOpenGl().TexBuffer( m_glDimension, m_glInternal, m_glBuffer.GetGlName() );
	}

	void GlTboTextureStorage::DoUnbind( uint32_t p_index )
	{
		GetOpenGl().TexBuffer( m_glDimension, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorage::DoLock( uint32_t p_lock )
	{
		m_glBuffer.Bind();
		return m_glBuffer.Lock( GetOpenGl().GetLockFlags( p_lock ) );
	}

	void GlTboTextureStorage::DoUnlock( bool p_modified )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}
}
