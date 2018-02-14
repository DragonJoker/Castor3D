#include "Pipeline/GlVertexLayout.hpp"

namespace gl_renderer
{
	VertexLayout::VertexLayout( uint32_t bindingSlot
		, uint32_t stride
		, renderer::VertexInputRate inputRate )
		: renderer::VertexLayout{ bindingSlot, stride, inputRate }
	{
	}
}
