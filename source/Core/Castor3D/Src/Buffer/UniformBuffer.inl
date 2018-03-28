#include "Render/RenderSystem.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Core/Device.hpp>

#include <algorithm>

using namespace castor;

namespace castor3d
{
	template< typename T >
	inline UniformBuffer< T >::UniformBuffer( RenderSystem const & renderSystem
		, uint32_t count
		, renderer::MemoryPropertyFlags flags )
		: m_renderSystem{ renderSystem }
		, m_flags{ flags }
		, m_count{ count }
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			m_available.insert( i );
		}
	}

	template< typename T >
	inline UniformBuffer< T >::~UniformBuffer()
	{
	}

	template< typename T >
	inline void UniformBuffer< T >::initialise()
	{
		REQUIRE( m_renderSystem.hasCurrentDevice() );
		m_buffer = renderer::makeUniformBuffer< T >( *m_renderSystem.getCurrentDevice()
			, m_count
			, renderer::BufferTarget::eTransferDst
			, m_flags );
	}

	template< typename T >
	inline void UniformBuffer< T >::cleanup()
	{
		REQUIRE( m_renderSystem.hasCurrentDevice() );
		m_buffer.reset();
	}

	template< typename T >
	inline bool UniformBuffer< T >::hasAvailable()const
	{
		return !m_available.empty();
	}

	template< typename T >
	inline uint32_t UniformBuffer< T >::allocate()
	{
		REQUIRE( hasAvailable() );
		uint32_t result = *m_available.begin();
		m_available.erase( m_available.begin() );
		return result;
	}

	template< typename T >
	inline void UniformBuffer< T >::deallocate( uint32_t offset )
	{
		m_available.insert( offset );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( renderer::StagingBuffer & stagingBuffer
		, renderer::CommandBuffer const & commandBuffer
		, uint32_t offset
		, uint32_t count
		, T const * buffer
		, renderer::PipelineStageFlags flags )const
	{
		stagingBuffer.uploadUniformData( commandBuffer
			, buffer
			, count
			, offset
			, getBuffer()
			, flags );
	}

	template< typename T >
	inline void UniformBuffer< T >::download( renderer::StagingBuffer & stagingBuffer
		, renderer::CommandBuffer const & commandBuffer
		, uint32_t offset
		, uint32_t count
		, T * buffer
		, renderer::PipelineStageFlags flags )const
	{
		stagingBuffer.downloadUniformData( commandBuffer
			, buffer
			, count
			, offset
			, getBuffer()
			, flags );
	}
}
