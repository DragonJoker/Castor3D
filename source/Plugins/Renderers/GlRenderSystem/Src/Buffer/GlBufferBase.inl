#include "Common/OpenGl.hpp"

namespace GlRender
{
	template< typename T >
	GlBufferBase< T >::GlBufferBase( OpenGl & p_gl, GlBufferTarget p_target )
		: BindableType{ p_gl,
						"GlBufferBase",
						std::bind( &OpenGl::GenBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsBuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl, p_target]( uint32_t p_glName )
						{
							return p_gl.BindBuffer( p_target, p_glName );
						} }
		, m_target{ p_target }
	{
	}

	template< typename T >
	GlBufferBase< T >::~GlBufferBase()
	{
	}

	template< typename T >
	bool GlBufferBase< T >::Copy( GlBufferBase< T > const & p_src, uint32_t p_size )const
	{
		uint32_t l_size = p_size * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size );
		REQUIRE( p_src.m_allocatedSize >= l_size );
		bool l_return = GetOpenGl().BindBuffer( GlBufferTarget::eRead, p_src.GetGlName() );

		if ( l_return )
		{
			l_return = GetOpenGl().BindBuffer( GlBufferTarget::eWrite, GetGlName() );

			if ( l_return )
			{
				l_return = BindableType::GetOpenGl().CopyBufferSubData( GlBufferTarget::eRead, GlBufferTarget::eWrite, 0, 0, l_size );
				GetOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
			}

			GetOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const
	{
		REQUIRE( p_count > 0 );
		bool l_return = Bind();

		if ( l_return )
		{
			l_return = BindableType::GetOpenGl().BufferData( m_target, p_count * sizeof( T ), nullptr, BindableType::GetOpenGl().GetBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
			m_allocatedSize = p_count * sizeof( T );
			Unbind();
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const
	{
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		bool l_return = Bind();

		if ( l_return )
		{
			auto l_provider = BindableType::GetOpenGl().GetProvider();

			if ( l_provider == GlProvider::eNvidia
				 || l_provider == GlProvider::eATI )
			{
				auto l_buffer = Lock( p_offset, p_count, Castor3D::AccessType::eWrite );

				if ( l_buffer )
				{
					std::memcpy( l_buffer, p_buffer, l_size );
					Unlock();
				}
				else
				{
					l_return = false;
				}
			}
			else
			{
				l_return = BindableType::GetOpenGl().BufferSubData( m_target, l_offset, l_size, p_buffer );
			}

			Unbind();
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const
	{
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		bool l_return = Bind();

		if ( l_return )
		{
			auto l_buffer = Lock( p_offset, p_count, Castor3D::AccessType::eRead );

			if ( l_buffer )
			{
				std::memcpy( p_buffer, l_buffer, l_size );
				Unlock();
			}
			else
			{
				l_return = false;
			}

			Unbind();
		}

		return l_return;
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor::FlagCombination< Castor3D::AccessType > const & p_flags )const
	{
		uint32_t l_size = p_count * sizeof( T );
		uint32_t l_offset = p_offset * sizeof( T );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= l_size + l_offset );
		T * l_return = nullptr;

		if ( this->GetGlName() != GlInvalidIndex )
		{
			l_return = reinterpret_cast< T * >( BindableType::GetOpenGl().MapBufferRange( m_target, l_offset, l_size, BindableType::GetOpenGl().GetBitfieldFlags( p_flags ) ) );
		}

		return l_return;
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( GlAccessType p_access )const
	{
		T * l_return = nullptr;

		if ( this->GetGlName() != GlInvalidIndex )
		{
			l_return = reinterpret_cast< T * >( BindableType::GetOpenGl().MapBuffer( m_target, p_access ) );
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::Unlock()const
	{
		bool l_return = this->GetGlName() != GlInvalidIndex;
		l_return &= BindableType::GetOpenGl().UnmapBuffer( m_target );
		return l_return;
	}

	template< typename T >
	void GlBufferBase< T >::SetBindingPoint( uint32_t p_point )const
	{
		m_bindingPoint = p_point;
		BindableType::GetOpenGl().BindBufferBase( m_target, p_point, GetGlName() );
	}
}
