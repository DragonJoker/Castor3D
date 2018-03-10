#include "Descriptor/GlDescriptorSetLayout.hpp"

namespace gl_renderer
{
	DescriptorSetLayout::DescriptorSetLayout( renderer::Device const & device
		, renderer::DescriptorSetLayoutBindingArray && bindings )
		: renderer::DescriptorSetLayout{ device, std::move( bindings ) }
	{
	}
}
