#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkFrontFace convert( renderer::FrontFace const & value )
	{
		switch ( value )
		{
		case renderer::FrontFace::eCounterClockwise:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;

		case renderer::FrontFace::eClockwise:
			return VK_FRONT_FACE_CLOCKWISE;

		default:
			assert( false && "Unsupported front face mode");
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	}
}
