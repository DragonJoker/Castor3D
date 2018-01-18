/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "BufferMemoryBarrier.hpp"

namespace renderer
{
	template< typename T >
	Buffer< T >::Buffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
		: m_buffer{ device.createBuffer( uint32_t( count * sizeof( T ) )
			, target
			, flags ) }
	{
	}

	template< typename T >
	inline T * Buffer< T >::lock( uint32_t offset
		, uint32_t count
		, MemoryMapFlags flags )const
	{
		return reinterpret_cast< T * >( m_buffer->lock( uint32_t( offset * sizeof( T ) )
			, uint32_t( count * sizeof( T ) )
			, flags ) );
	}

	template< typename T >
	inline void Buffer< T >::unlock( uint32_t count
		, bool modified )const
	{
		return m_buffer->unlock( uint32_t( count * sizeof( T ) )
			, modified );
	}

	template< typename T >
	inline BufferMemoryBarrier Buffer< T >::makeTransferDestination()const
	{
		return m_buffer->makeTransferDestination();
	}

	template< typename T >
	inline BufferMemoryBarrier Buffer< T >::makeTransferSource()const
	{
		return m_buffer->makeTransferSource();
	}

	template< typename T >
	inline BufferMemoryBarrier Buffer< T >::makeVertexShaderInputResource()const
	{
		return m_buffer->makeVertexShaderInputResource();
	}

	template< typename T >
	inline BufferMemoryBarrier Buffer< T >::makeUniformBufferInput()const
	{
		return m_buffer->makeUniformBufferInput();
	}

	template< typename T >
	inline BufferMemoryBarrier Buffer< T >::makeMemoryTransitionBarrier( AccessFlags dstAccess )const
	{
		return m_buffer->makeMemoryTransitionBarrier();
	}

	template< typename T >
	inline uint32_t Buffer< T >::getCount()const
	{
		return uint32_t( m_buffer->getSize() / sizeof( T ) );
	}

	template< typename T >
	inline BufferBase const & Buffer< T >::getBuffer()const
	{
		return *m_buffer;
	}
}
