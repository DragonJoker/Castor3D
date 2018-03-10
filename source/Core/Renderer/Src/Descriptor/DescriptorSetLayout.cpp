/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorSetLayout.hpp"
#include "Descriptor/DescriptorSetPool.hpp"

#include <algorithm>

namespace renderer
{
	DescriptorSetLayout::DescriptorSetLayout( Device const & device
		, DescriptorSetLayoutBindingArray && bindings )
		: m_device{ device }
		, m_bindings{ std::move( bindings ) }
	{
	}

	DescriptorSetLayoutBinding const & DescriptorSetLayout::getBinding( uint32_t bindingPoint
		, uint32_t index )const
	{
		auto it = std::find_if( m_bindings.begin()
			, m_bindings.end()
			, [bindingPoint, index]( DescriptorSetLayoutBinding const & lookup )
		{
			return lookup.getBindingPoint() == bindingPoint
				&& index < lookup.getDescriptorsCount();
		} );

		if ( it == m_bindings.end() )
		{
			throw std::range_error{ "Descriptor set layout binding was not found." };
		}

		return *it;
	}

	renderer::DescriptorSetPoolPtr DescriptorSetLayout::createPool( uint32_t maxSets
		, bool automaticFree )const
	{
		return std::make_unique< DescriptorSetPool >( m_device
			, *this
			, maxSets
			, automaticFree );
	}
}
