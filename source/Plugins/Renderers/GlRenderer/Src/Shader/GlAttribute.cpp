#include "Shader/GlAttribute.hpp"

#include "Pipeline/GlVertexLayout.hpp"

namespace gl_renderer
{
	AttributeBase::AttributeBase( renderer::VertexLayout & layout
		, renderer::AttributeFormat format
		, uint32_t location
		, uint32_t offset )
		: renderer::AttributeBase{ layout, format, location, offset }
	{
	}
}
