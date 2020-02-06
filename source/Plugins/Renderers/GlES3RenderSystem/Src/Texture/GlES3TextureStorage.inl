#include "Common/OpenGlES3.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlES3Render
{
	template< typename Traits >
	GlES3TextureStorage< Traits >::GlES3TextureStorage( OpenGlES3 & p_gl
												  , GlES3RenderSystem & p_renderSystem
												  , Castor3D::TextureStorageType p_type
												  , Castor3D::TextureLayout & p_layout
												  , Castor3D::AccessTypes const & p_cpuAccess
												  , Castor3D::AccessTypes const & p_gpuAccess )
		: Castor3D::TextureStorage{ p_type, p_layout, p_cpuAccess, p_gpuAccess }
		, Holder{ p_gl }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glType{ p_gl.Get( p_type ) }
		, m_impl{ *this }
	{
		for ( auto & l_image : p_layout )
		{
			m_impl.Fill( *this, *l_image );
		}
	}

	template< typename Traits >
	GlES3TextureStorage< Traits >::~GlES3TextureStorage()
	{
	}

	template< typename Traits >
	void GlES3TextureStorage< Traits >::Bind( uint32_t p_index )const
	{
		m_impl.Bind( *this, p_index );
	}

	template< typename Traits >
	void GlES3TextureStorage< Traits >::Unbind( uint32_t p_index )const
	{
		m_impl.Unbind( *this, p_index );
	}

	template< typename Traits >
	uint8_t * GlES3TextureStorage< Traits >::Lock( Castor3D::AccessTypes const & p_lock )
	{
		return m_impl.Lock( *this, p_lock, 0u );
	}

	template< typename Traits >
	void GlES3TextureStorage< Traits >::Unlock( bool p_modified )
	{
		m_impl.Unlock( *this, p_modified, 0u );
	}

	template< typename Traits >
	uint8_t * GlES3TextureStorage< Traits >::Lock( Castor3D::AccessTypes const & p_lock, uint32_t p_index )
	{
		return m_impl.Lock( *this, p_lock, p_index );
	}

	template< typename Traits >
	void GlES3TextureStorage< Traits >::Unlock( bool p_modified, uint32_t p_index )
	{
		m_impl.Unlock( *this, p_modified, p_index );
	}
}
