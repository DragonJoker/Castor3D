#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkFormat convert( renderer::AttributeFormat format )
	{
		switch ( format )
		{
		case renderer::AttributeFormat::eFloat:
			return VK_FORMAT_R32_SFLOAT;

		case renderer::AttributeFormat::eVec2f:
			return VK_FORMAT_R32G32_SFLOAT;

		case renderer::AttributeFormat::eVec3f:
			return VK_FORMAT_R32G32B32_SFLOAT;

		case renderer::AttributeFormat::eVec4f:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

		case renderer::AttributeFormat::eMat4f:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

		case renderer::AttributeFormat::eInt:
			return VK_FORMAT_R32_SINT;

		case renderer::AttributeFormat::eVec2i:
			return VK_FORMAT_R32G32_SINT;

		case renderer::AttributeFormat::eVec3i:
			return VK_FORMAT_R32G32B32_SINT;

		case renderer::AttributeFormat::eVec4i:
			return VK_FORMAT_R32G32B32A32_SINT;

		case renderer::AttributeFormat::eColour:
			return VK_FORMAT_R8G8B8A8_UNORM;

		default:
			assert( false && "Unsupported attribute format." );
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	}
}
