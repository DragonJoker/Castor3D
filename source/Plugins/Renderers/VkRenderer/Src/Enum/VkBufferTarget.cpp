#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBufferUsageFlags convert( renderer::BufferTargets const & targets )
	{
		VkBufferUsageFlags result{ 0 };

		if ( checkFlag( targets, renderer::BufferTarget::eIndexBuffer ) )
		{
			result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eDispatchIndirectBuffer )
			|| checkFlag( targets, renderer::BufferTarget::eDrawIndirectBuffer ) )
		{
			result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eStorageBuffer ) )
		{
			result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eStorageTexelBuffer ) )
		{
			result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eTransferDst )
			|| checkFlag( targets, renderer::BufferTarget::eUnpackBuffer ) )
		{
			result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eTransferSrc )
			|| checkFlag( targets, renderer::BufferTarget::ePackBuffer ) )
		{
			result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eUniformBuffer ) )
		{
			result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eUniformTexelBuffer ) )
		{
			result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		}

		if ( checkFlag( targets, renderer::BufferTarget::eVertexBuffer ) )
		{
			result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}

		return result;
	}
}
