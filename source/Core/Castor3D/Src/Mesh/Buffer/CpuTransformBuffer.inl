#include "Engine.hpp"

#include "Mesh/Buffer/GpuTransformBuffer.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	template< typename T >
	inline CpuTransformBuffer< T >::CpuTransformBuffer( Engine & p_engine, TransformBufferDeclaration const & p_declaration )
		: Castor::OwnedBy< Engine >( p_engine )
		, m_gpuBuffer()
		, m_data()
		, m_savedSize( 0 )
		, m_bufferDeclaration( p_declaration )
	{
	}

	template< typename T >
	inline CpuTransformBuffer< T >::~CpuTransformBuffer()
	{
	}

	template< typename T >
	inline bool CpuTransformBuffer< T >::Create( ShaderProgram & p_program )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetEngine()->GetRenderSystem()->CreateTransformBuffer( p_program, m_declaration );
		}

		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			l_return = m_gpuBuffer->Create();
		}

		return l_return;
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Destroy()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Destroy();
			m_gpuBuffer.reset();
		}
	}

	template< typename T >
	inline bool CpuTransformBuffer< T >::Initialise()
	{
		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			l_return = m_gpuBuffer->Initialise();
		}

		return l_return;
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Cleanup()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Cleanup();
		}
	}

	template< typename T >
	inline T * CpuTransformBuffer< T >::Lock( uint32_t p_offset, uint32_t p_count, AccessType p_flags )
	{
		T * l_return = nullptr;

		if ( m_gpuBuffer )
		{
			l_return = reinterpret_cast< uint8_t const * >( m_gpuBuffer->Lock( p_offset * sizeof( T ), p_count * sizeof( T ), p_flags ) );
		}

		return l_return;
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Unlock()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Unlock();
		}
	}

	template< typename T >
	inline bool CpuTransformBuffer< T >::Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		bool l_return = false;

		if ( m_gpuBuffer )
		{
			l_return = m_gpuBuffer->Fill( reinterpret_cast< uint8_t const * >( p_buffer ), p_size * sizeof( T ), p_type, p_nature );
		}

		return l_return;
	}

	template< typename T >
	inline bool CpuTransformBuffer< T >::Bind()
	{
		bool l_return = false;

		if ( m_gpuBuffer )
		{
			l_return = m_gpuBuffer->Bind();
		}

		return l_return;
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Unbind()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Unbind();
		}
	}

	template< typename T >
	inline T const & CpuTransformBuffer< T >:: operator[]( uint32_t p_index )const
	{
		REQUIRE( p_index < m_data.size() );
		return m_data[p_index];
	}

	template< typename T >
	inline T & CpuTransformBuffer< T >::operator[]( uint32_t p_index )
	{
		REQUIRE( p_index < m_data.size() );
		return m_data[p_index];
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::AddElement( T const & p_value )
	{
		m_data.push_back( p_value );
	}

	template< typename T >
	inline uint32_t CpuTransformBuffer< T >::GetSize()const
	{
		uint32_t l_uiReturn = m_savedSize;

		if ( m_data.size() )
		{
			l_uiReturn = uint32_t( m_data.size() );
		}

		return l_uiReturn;
	}

	template< typename T >
	inline uint32_t CpuTransformBuffer< T >::GetCapacity()const
	{
		// Safe cast since I limit a buffer size to uint32_t
		return uint32_t( m_data.capacity() );
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Resize( uint32_t p_uiNewSize )
	{
		m_data.resize( p_uiNewSize, T{} );
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Grow( uint32_t p_uiIncrement )
	{
		if ( p_uiIncrement + GetCapacity() < p_uiIncrement )
		{
			throw std::range_error( "Can't reserve more space for this buffer" );
		}
		else
		{
			m_data.reserve( m_data.capacity() + p_uiIncrement );
		}
	}

	template< typename T >
	inline void CpuTransformBuffer< T >::Clear()
	{
		m_savedSize = uint32_t( m_data.size() );
		m_data.clear();
	}

	template< typename T >
	inline GpuTransformBuffer const & CpuTransformBuffer< T >::GetGpuBuffer()const
	{
		REQUIRE( m_gpuBuffer );
		return *m_gpuBuffer;
	}
	template< typename T >
	inline T const * CpuTransformBuffer< T >::data()const
	{
		return ( m_data.size() ? &m_data[0] : nullptr );
	}

	template< typename T >
	inline T * CpuTransformBuffer< T >::data()
	{
		return ( m_data.size() ? &m_data[0] : nullptr );
	}

	template< typename T >
	inline TransformBufferDeclaration const & CpuTransformBuffer< T >::GetDeclaration()const
	{
		return m_bufferDeclaration;
	}
}
