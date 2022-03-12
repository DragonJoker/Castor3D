#include "Castor3D/Miscellaneous/StagingData.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementCUSmartPtr( castor3d, StagingData )

namespace castor3d
{
	StagingData::StagingData( RenderDevice const & device
		, std::string const & debugName )
		: m_device{ device }
		, m_debugName{ debugName }
		, m_command{ m_device.graphicsData()->commandPool->createCommandBuffer( m_debugName
			, VK_COMMAND_BUFFER_LEVEL_PRIMARY ) }
		, m_fence{ m_device->createFence( m_debugName
			, VK_FENCE_CREATE_SIGNALED_BIT ) }
	{
	}

	void StagingData::upload( void const * data
		, VkDeviceSize size
		, VkDeviceSize offset
		, ashes::BufferBase const & buffer )
	{
		if ( !size || !data )
		{
			return;
		}

		auto mappedSize = ashes::getAlignedSize( size
			, m_device->getProperties().limits.nonCoherentAtomSize );

		if ( !m_staging || mappedSize > m_staging->getBuffer().getSize() )
		{
			m_staging = std::make_unique< ashes::StagingBuffer >( *m_device
				, m_debugName
				, 0u
				, mappedSize );
		}

		auto queueData = m_device.graphicsData();
		m_fence->reset();
		m_command->begin();
		m_staging->uploadBufferData( *m_command
			, reinterpret_cast< uint8_t const * >( data )
			, size
			, offset
			, buffer );
		m_command->end();
		queueData->queue->submit( *m_command
			, m_fence.get() );
		m_fence->wait( ashes::MaxTimeout );
	}

	void StagingData::uploadNoWait( void const * data
		, VkDeviceSize size
		, VkDeviceSize offset
		, ashes::BufferBase const & buffer )
	{
		auto mappedSize = ashes::getAlignedSize( size
			, m_device->getProperties().limits.nonCoherentAtomSize );

		if ( !m_staging || mappedSize > m_staging->getBuffer().getSize() )
		{
			m_staging = std::make_unique< ashes::StagingBuffer >( *m_device
				, m_debugName
				, 0u
				, mappedSize );
		}

		auto queueData = m_device.graphicsData();
		m_command->begin();
		m_staging->uploadBufferData( *m_command
			, reinterpret_cast< uint8_t const * >( data )
			, size
			, offset
			, buffer );
		m_command->end();
		queueData->queue->submit( *m_command );
	}
}
