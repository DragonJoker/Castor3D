/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorSet.hpp"

#include "Descriptor/DescriptorSetPool.hpp"

namespace renderer
{
	DescriptorSet::DescriptorSet( DescriptorSetPool const & pool, uint32_t bindingPoint )
		: m_bindingPoint{ bindingPoint }
		, m_pool{ pool }
	{
		m_pool.allocate( 1u );
	}

	DescriptorSet::~DescriptorSet()
	{
		m_pool.deallocate( 1u );
	}
}
