#include "Descriptor/GlDescriptorSetLayout.hpp"

#include "Descriptor/GlDescriptorSetPool.hpp"

namespace gl_renderer
{
	DescriptorSetLayout::DescriptorSetLayout( renderer::Device const & device
		, renderer::DescriptorSetLayoutBindingArray && bindings )
		: renderer::DescriptorSetLayout{ device, std::move( bindings ) }
	{
	}

	renderer::DescriptorSetPoolPtr DescriptorSetLayout::createPool( uint32_t maxSets
		, bool automaticFree )const
	{
		return std::make_unique< DescriptorSetPool >( *this, maxSets, automaticFree );
	}
}
