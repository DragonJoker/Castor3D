#include "VkQueryPool.hpp"

#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	QueryPool::QueryPool( Device const & device
		, renderer::QueryType type
		, uint32_t count
		, renderer::QueryPipelineStatisticFlags pipelineStatistics )
		: renderer::QueryPool{ device, type, count, pipelineStatistics }
		, m_device{ device }
	{
		VkQueryPoolCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
			nullptr,
			0u,
			convert( type ),
			count,
			convert( pipelineStatistics )
		};
		DEBUG_DUMP( createInfo );
		vk::CreateQueryPool( device
			, &createInfo
			, nullptr
			, &m_pool );
	}

	QueryPool::~QueryPool()
	{
		vk::DestroyQueryPool( m_device
			, m_pool
			, nullptr );
	}

	void QueryPool::getResults( uint32_t firstQuery
		, uint32_t queryCount
		, uint32_t stride
		, renderer::QueryResultFlags flags
		, renderer::UInt32Array & data )const
	{
		vk::GetQueryPoolResults( m_device
			, m_pool
			, firstQuery
			, queryCount
			, data.size()
			, data.data()
			, stride
			, convert( flags ) );
	}

	void QueryPool::getResults( uint32_t firstQuery
		, uint32_t queryCount
		, uint32_t stride
		, renderer::QueryResultFlags flags
		, renderer::UInt64Array & data )const
	{
		vk::GetQueryPoolResults( m_device
			, m_pool
			, firstQuery
			, queryCount
			, data.size()
			, data.data()
			, stride
			, convert( flags ) | VK_QUERY_RESULT_64_BIT );
	}
}
