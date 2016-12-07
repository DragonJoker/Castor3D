#include "Common/OpenGl.hpp"

#include <Texture/TextureLayout.hpp>

namespace GlRender
{
	template< typename Traits >
	GlTextureStorage< Traits >::GlTextureStorage( OpenGl & p_gl
												  , GlRenderSystem & p_renderSystem
												  , Castor3D::TextureStorageType p_type
												  , Castor3D::TextureLayout & p_layout
												  , Castor::FlagCombination< Castor3D::AccessType > const & p_cpuAccess
												  , Castor::FlagCombination< Castor3D::AccessType > const & p_gpuAccess )
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
	GlTextureStorage< Traits >::~GlTextureStorage()
	{
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Bind( uint32_t p_index )const
	{
		m_impl.Bind( *this, p_index );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Unbind( uint32_t p_index )const
	{
		m_impl.Unbind( *this, p_index );
	}

	template< typename Traits >
	uint8_t * GlTextureStorage< Traits >::Lock( Castor::FlagCombination< Castor3D::AccessType > const & p_lock )
	{
		return m_impl.Lock( *this, p_lock, 0u );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Unlock( bool p_modified )
	{
		m_impl.Unlock( *this, p_modified, 0u );
	}

	template< typename Traits >
	uint8_t * GlTextureStorage< Traits >::Lock( Castor::FlagCombination< Castor3D::AccessType > const & p_lock, uint32_t p_index )
	{
		return m_impl.Lock( *this, p_lock, p_index );
	}

	template< typename Traits >
	void GlTextureStorage< Traits >::Unlock( bool p_modified, uint32_t p_index )
	{
		m_impl.Unlock( *this, p_modified, p_index );
	}
}
