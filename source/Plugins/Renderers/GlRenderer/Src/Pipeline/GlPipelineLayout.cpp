#include "Pipeline/GlPipelineLayout.hpp"

#include "Descriptor/GlDescriptorSetLayout.hpp"
#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	PipelineLayout::PipelineLayout( renderer::Device const & device
		, renderer::DescriptorSetLayout const * layout )
		: renderer::PipelineLayout{ device, layout }
	{
	}
}
