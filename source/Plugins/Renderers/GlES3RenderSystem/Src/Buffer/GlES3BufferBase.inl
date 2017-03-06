#include "Common/OpenGlES3.hpp"

namespace GlES3Render
{
	template< typename T >
	GlES3BufferBase< T >::GlES3BufferBase( OpenGlES3 & p_gl, GlES3BufferTarget p_target )
		: BindableType{ p_gl,
						"GlES3BufferBase",
						std::bind( &OpenGlES3::GenBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::DeleteBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::IsBuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, p_target]( uint32_t p_glName )
						{
							return p_gl.BindBuffer( p_target, p_glName );
						} }
		, m_target{ p_target }
	{
	}

	template< typename T >
	GlES3BufferBase< T >::~GlES3BufferBase()
	{
	}

	template< typename T >
	void GlES3BufferBase< T >::Copy( GlES3BufferBase< T > const & p_src, uint32_t p_size )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		uint32_t l_size = p_size * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size );
		REQUIRE( p_src.m_allocatedSize >= l_size );
		GetOpenGlES3().BindBuffer( GlES3BufferTarget::eRead, p_src.GetGlES3Name() );
		GetOpenGlES3().BindBuffer( GlES3BufferTarget::eWrite, GetGlES3Name() );
		BindableType::GetOpenGlES3().CopyBufferSubData( GlES3BufferTarget::eRead, GlES3BufferTarget::eWrite, 0, 0, l_size );
		GetOpenGlES3().BindBuffer( GlES3BufferTarget::eWrite, 0 );
		GetOpenGlES3().BindBuffer( GlES3BufferTarget::eRead, 0 );
	}

	template< typename T >
	void GlES3BufferBase< T >::InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		REQUIRE( p_count > 0 );
		Bind();
		BindableType::GetOpenGlES3().BufferData( m_target, p_count * sizeof( T ), nullptr, BindableType::GetOpenGlES3().GetBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
		m_allocatedSize = p_count * sizeof( T );
		Unbind();
	}

	template< typename T >
	void GlES3BufferBase< T >::Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		Bind();
		auto l_provider = BindableType::GetOpenGlES3().GetProvider();

		if ( l_provider == GlES3Provider::eNvidia || l_provider == GlES3Provider::eATI )
		{
			auto l_buffer = Lock( p_offset, p_count, Castor3D::AccessType::eWrite );

			if ( l_buffer )
			{
				std::memcpy( l_buffer, p_buffer, l_size );
				Unlock();
			}
		}
		else
		{
			BindableType::GetOpenGlES3().BufferSubData( m_target, l_offset, l_size, p_buffer );
		}

		Unbind();
	}

	template< typename T >
	void GlES3BufferBase< T >::Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		Bind();
		auto l_buffer = Lock( p_offset, p_count, Castor3D::AccessType::eRead );

		if ( l_buffer )
		{
			std::memcpy( p_buffer, l_buffer, l_size );
			Unlock();
		}

		Unbind();
	}

	template< typename T >
	T * GlES3BufferBase< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		return reinterpret_cast< T * >( BindableType::GetOpenGlES3().MapBufferRange( m_target, l_offset, l_size, BindableType::GetOpenGlES3().GetBitfieldFlags( p_flags ) ) );
	}

	template< typename T >
	T * GlES3BufferBase< T >::Lock( Castor3D::AccessTypes const & p_flags )const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		return reinterpret_cast< T * >( BindableType::GetOpenGlES3().MapBufferRange( m_target, 0u, m_allocatedSize, BindableType::GetOpenGlES3().GetBitfieldFlags( p_flags ) ) );
	}

	template< typename T >
	void GlES3BufferBase< T >::Unlock()const
	{
		REQUIRE( this->GetGlES3Name() != GlES3InvalidIndex );
		BindableType::GetOpenGlES3().UnmapBuffer( m_target );
	}

	template< typename T >
	void GlES3BufferBase< T >::SetBindingPoint( uint32_t p_point )const
	{
		m_bindingPoint = p_point;
		BindableType::GetOpenGlES3().BindBufferBase( m_target, p_point, GetGlES3Name() );
	}
}
