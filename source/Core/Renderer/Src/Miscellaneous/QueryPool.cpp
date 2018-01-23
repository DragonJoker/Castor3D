#include "QueryPool.hpp"

namespace renderer
{
	QueryPool::QueryPool( Device const & device
		, QueryType type
		, uint32_t count
		, QueryPipelineStatisticFlags pipelineStatistics )
		: m_device{ device }
		, m_type{ type }
		, m_count{ count }
		, m_pipelineStatistics{ pipelineStatistics }
	{
	}
}
