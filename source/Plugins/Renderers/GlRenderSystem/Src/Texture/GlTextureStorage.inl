#include "Common/OpenGl.hpp"

#include <Texture/TextureImage.hpp>

namespace GlRender
{
	template< typename Traits >
	GlTextureStorage< Traits >::GlTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, Castor3D::TextureStorageType p_type, Castor3D::TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: Castor3D::TextureStorage{ p_type, p_image, p_cpuAccess, p_gpuAccess }
		, Holder{ p_gl }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glType{ p_gl.Get( p_type ) }
		, m_impl{ *this }
	{
	}

	template< typename Traits >
	GlTextureStorage< Traits >::~GlTextureStorage()
	{
	}

	template< typename Traits >
	bool GlTextureStorage< Traits >::Bind( uint32_t p_index )const
	{
		return m_impl.Bind( *this, p_index );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Unbind( uint32_t p_index )const
	{
		m_impl.Unbind( *this, p_index );
	}

	template< typename Traits >
	uint8_t * GlTextureStorage< Traits >::Lock( uint32_t p_lock )
	{
		return m_impl.Lock( *this, p_lock );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Unlock( bool p_modified )
	{
		m_impl.Unlock( *this, p_modified );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		m_impl.Fill( *this, p_buffer, p_size, p_format );
	}
}
