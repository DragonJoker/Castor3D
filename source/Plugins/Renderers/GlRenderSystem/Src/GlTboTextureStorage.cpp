#include "GlTboTextureStorage.hpp"

#include "OpenGl.hpp"

#include <Logger.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTboTextureStorage::GlTboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage{ p_gl, p_renderSystem, p_type, p_image, p_cpuAccess, p_gpuAccess }
		, m_glBuffer{ p_gl, eGL_BUFFER_TARGET_TEXTURE }
	{
		bool l_return = m_glBuffer.Create();

		if ( l_return )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			m_glInternal = GetOpenGl().GetInternal( l_buffer->format() );
			l_return = m_glBuffer.Initialise( l_buffer->const_ptr(), l_buffer->size(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}
	}

	GlTboTextureStorage::~GlTboTextureStorage()
	{
		m_glBuffer.Cleanup();
		m_glBuffer.Destroy();
	}

	bool GlTboTextureStorage::Bind( uint32_t p_index )
	{
		return GetOpenGl().TexBuffer( eGL_TEXDIM_BUFFER, m_glInternal, m_glBuffer.GetGlName() );
	}

	void GlTboTextureStorage::Unbind( uint32_t p_index )
	{
		GetOpenGl().TexBuffer( eGL_TEXDIM_BUFFER, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorage::Lock( uint32_t p_lock )
	{
		m_glBuffer.Bind();
		return m_glBuffer.Lock( GetOpenGl().GetLockFlags( p_lock ) );
	}

	void GlTboTextureStorage::Unlock( bool p_modified )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}

	void GlTboTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_glBuffer.Fill( p_buffer, p_size.width() * p_size.height() * PF::GetBytesPerPixel( p_format ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	}
}
