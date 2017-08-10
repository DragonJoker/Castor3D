#include "Common/OpenGl.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlRender
{
	template< typename Traits >
	GlTextureStorage< Traits >::GlTextureStorage( OpenGl & p_gl
												  , GlRenderSystem & renderSystem
												  , castor3d::TextureStorageType p_type
												  , castor3d::TextureLayout & p_layout
												  , castor3d::AccessTypes const & p_cpuAccess
												  , castor3d::AccessTypes const & p_gpuAccess )
		: castor3d::TextureStorage{ p_type, p_layout, p_cpuAccess, p_gpuAccess }
		, Holder{ p_gl }
		, m_glRenderSystem{ &renderSystem }
		, m_glType{ p_gl.get( p_type ) }
		, m_impl{ *this }
	{
		for ( auto & image : p_layout )
		{
			m_impl.fill( *this, *image );
		}
	}

	template< typename Traits >
	GlTextureStorage< Traits >::~GlTextureStorage()
	{
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::bind( uint32_t p_index )const
	{
		m_impl.bind( *this, p_index );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::unbind( uint32_t p_index )const
	{
		m_impl.unbind( *this, p_index );
	}

	template< typename Traits >
	uint8_t * GlTextureStorage< Traits >::lock( castor3d::AccessTypes const & p_lock )
	{
		return m_impl.lock( *this, p_lock, 0u );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::unlock( bool p_modified )
	{
		m_impl.unlock( *this, p_modified, 0u );
	}

	template< typename Traits >
	uint8_t * GlTextureStorage< Traits >::lock( castor3d::AccessTypes const & p_lock, uint32_t p_index )
	{
		return m_impl.lock( *this, p_lock, p_index );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::unlock( bool p_modified, uint32_t p_index )
	{
		m_impl.unlock( *this, p_modified, p_index );
	}
}
