#include <Buffer/StagingBuffer.hpp>
#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	template< typename T >
	UniformBuffer< T >::UniformBuffer( renderer::Device const & device
		, uint32_t count
		, renderer::MemoryPropertyFlags flags )
		: m_buffer{ renderer::makeUniformBuffer< T >( device
			, count
			, renderer::BufferTarget::eTransferDst
			, flags ) }
	{
	}

	template< typename T >
	UniformBuffer< T >::~UniformBuffer()
	{
	}

	template< typename T >
	bool UniformBuffer< T >::hasAvailable()const
	{
		return !m_available.empty();
	}

	template< typename T >
	uint32_t UniformBuffer< T >::allocate()
	{
		REQUIRE( hasAvailable() );
		uint32_t result = *m_available.begin();
		m_available.erase( m_available.begin() );
		return result;
	}

	template< typename T >
	void UniformBuffer< T >::deallocate( uint32_t offset )
	{
		m_available.insert( offset );
	}

	template< typename T >
	void UniformBuffer< T >::upload( renderer::StagingBuffer & stagingBuffer
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
	void UniformBuffer< T >::download( renderer::StagingBuffer & stagingBuffer
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
