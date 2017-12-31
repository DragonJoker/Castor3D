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
	void GlBufferBase< T >::copy( GlBufferBase< T > const & p_src, uint32_t p_size )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		uint32_t size = p_size * sizeof( T );
		REQUIRE( size > 0 );
		REQUIRE( m_allocatedSize >= size );
		REQUIRE( p_src.m_allocatedSize >= size );
		getOpenGl().BindBuffer( GlBufferTarget::eRead, p_src.getGlName() );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, getGlName() );
		BindableType::getOpenGl().CopyBufferSubData( GlBufferTarget::eRead, GlBufferTarget::eWrite, 0, 0, size );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
		getOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
	}

	template< typename T >
	void GlBufferBase< T >::initialiseStorage( uint32_t p_count, castor3d::BufferAccessType p_type, castor3d::BufferAccessNature p_nature )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( p_count > 0 );
		bind();
		BindableType::getOpenGl().BufferData( m_target, p_count * sizeof( T ), nullptr, BindableType::getOpenGl().getBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
		m_allocatedSize = p_count * sizeof( T );
		unbind();
	}

	template< typename T >
	void GlBufferBase< T >::upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		uint32_t size = p_count * sizeof( T );
		uint32_t offset = p_offset * sizeof( T );
		REQUIRE( size > 0 );
		REQUIRE( m_allocatedSize >= size + offset );
		bind();
		auto provider = BindableType::getOpenGl().getProvider();

		if ( provider == GlProvider::eNvidia || provider == GlProvider::eATI )
		{
			auto buffer = lock( p_offset, p_count, castor3d::AccessType::eWrite );

			if ( buffer )
			{
				std::memcpy( buffer, p_buffer, size );
				unlock();
			}
		}
		else
		{
			BindableType::getOpenGl().BufferSubData( m_target, offset, size, p_buffer );
		}

		unbind();
	}

	template< typename T >
	void GlBufferBase< T >::download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		uint32_t size = p_count * sizeof( T );
		uint32_t offset = p_offset * sizeof( T );
		REQUIRE( size > 0 );
		REQUIRE( m_allocatedSize >= size + offset );
		bind();
		auto buffer = lock( p_offset, p_count, castor3d::AccessType::eRead );

		if ( buffer )
		{
			std::memcpy( p_buffer, buffer, size );
			unlock();
		}

		unbind();
	}

	template< typename T >
	T * GlBufferBase< T >::lock( uint32_t p_offset, uint32_t p_count, castor3d::AccessTypes const & p_flags )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		uint32_t size = p_count * sizeof( T );
		uint32_t offset = p_offset * sizeof( T );
		REQUIRE( size > 0 );
		REQUIRE( m_allocatedSize >= size + offset );
		return reinterpret_cast< T * >( BindableType::getOpenGl().MapBufferRange( m_target, offset, size, BindableType::getOpenGl().getBitfieldFlags( p_flags ) ) );
	}

	template< typename T >
	T * GlBufferBase< T >::lock( GlAccessType p_access )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		return reinterpret_cast< T * >( BindableType::getOpenGl().MapBuffer( m_target, p_access ) );
	}

	template< typename T >
	void GlBufferBase< T >::unlock()const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		BindableType::getOpenGl().UnmapBuffer( m_target );
	}

	template< typename T >
	void GlBufferBase< T >::setBindingPoint( uint32_t p_point )const
	{
		m_bindingPoint = p_point;
		BindableType::getOpenGl().BindBufferBase( m_target, p_point, getGlName() );
	}
}
