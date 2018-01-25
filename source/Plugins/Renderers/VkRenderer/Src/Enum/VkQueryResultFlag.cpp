#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkQueryResultFlags convert( renderer::QueryResultFlags const & flags )
	{
		VkQueryResultFlags result{ 0 };

		if ( checkFlag( flags, renderer::QueryResultFlag::e64 ) )
		{
			result |= VK_QUERY_RESULT_64_BIT;
		}

		if ( checkFlag( flags, renderer::QueryResultFlag::eWait ) )
		{
			result |= VK_QUERY_RESULT_WAIT_BIT;
		}

		if ( checkFlag( flags, renderer::QueryResultFlag::eWithAvailability ) )
		{
			result |= VK_QUERY_RESULT_WITH_AVAILABILITY_BIT;
		}

		if ( checkFlag( flags, renderer::QueryResultFlag::ePartial ) )
		{
			result |= VK_QUERY_RESULT_PARTIAL_BIT;
		}

		return result;
	}
}
