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
	bool GlBufferBase< T >::Copy( GlBufferBase< T > const & p_src, uint32_t p_size )
	{
		bool l_return = GetOpenGl().BindBuffer( GlBufferTarget::eRead, p_src.GetGlName() );

		if ( l_return )
		{
			l_return = GetOpenGl().BindBuffer( GlBufferTarget::eWrite, GetGlName() );

			if ( l_return )
			{
				l_return = BindableType::GetOpenGl().CopyBufferSubData( GlBufferTarget::eRead, GlBufferTarget::eWrite, 0, 0, p_size * sizeof( T ) );
				GetOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
			}

			GetOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		bool l_return = Bind();

		if ( l_return )
		{
			l_return = BindableType::GetOpenGl().BufferData( m_target, p_size * sizeof( T ), p_buffer, BindableType::GetOpenGl().GetBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
			Unbind();
		}

		return l_return;
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )
	{
		T * l_return = nullptr;

		if ( this->GetGlName() != GlInvalidIndex )
		{
			l_return = reinterpret_cast< T * >( BindableType::GetOpenGl().MapBufferRange( m_target, p_offset * sizeof( T ), p_count * sizeof( T ), BindableType::GetOpenGl().GetBitfieldFlags( p_flags ) ) );
		}

		return l_return;
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( GlAccessType p_access )
	{
		T * l_return = nullptr;

		if ( this->GetGlName() != GlInvalidIndex )
		{
			l_return = reinterpret_cast< T * >( BindableType::GetOpenGl().MapBuffer( m_target, p_access ) );
		}

		return l_return;
	}

	template< typename T >
	bool GlBufferBase< T >::Unlock()
	{
		bool l_return = this->GetGlName() != GlInvalidIndex;
		l_return &= BindableType::GetOpenGl().UnmapBuffer( m_target );
		return l_return;
	}
}
