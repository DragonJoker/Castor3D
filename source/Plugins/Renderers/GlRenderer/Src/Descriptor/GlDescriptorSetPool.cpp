#include "Descriptor/GlDescriptorSetPool.hpp"

#include "Descriptor/GlDescriptorSet.hpp"

namespace gl_renderer
{
	DescriptorSetPool::DescriptorSetPool( renderer::DescriptorSetLayout const & layout
		, uint32_t maxSets
		, bool automaticFree )
		: renderer::DescriptorSetPool{ layout, maxSets }
	{
	}

	renderer::DescriptorSetPtr DescriptorSetPool::createDescriptorSet( uint32_t bindingPoint )const
	{
		return std::make_unique< DescriptorSet >( *this, bindingPoint );
	}
}
