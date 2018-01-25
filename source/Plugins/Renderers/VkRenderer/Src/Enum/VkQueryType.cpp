#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkQueryType convert( renderer::QueryType const & value )
	{
		switch ( value )
		{
		case renderer::QueryType::eOcclusion:
			return VK_QUERY_TYPE_OCCLUSION;

		case renderer::QueryType::eTimestamp:
			return VK_QUERY_TYPE_TIMESTAMP;

		case renderer::QueryType::ePipelineStatistics:
			return VK_QUERY_TYPE_PIPELINE_STATISTICS;

		default:
			assert( false && "Unsupported query type" );
			return VK_QUERY_TYPE_TIMESTAMP;
		}
	}
}
