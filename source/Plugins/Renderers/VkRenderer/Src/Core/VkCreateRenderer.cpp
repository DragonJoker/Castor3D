#include "VkCreateRenderer.hpp"
#include "Core/VkRenderer.hpp"

extern "C"
{
	VkRenderer_API renderer::Renderer * createRenderer( bool enableValidation )
	{
		return new vk_renderer::Renderer{ enableValidation };
	}
}
