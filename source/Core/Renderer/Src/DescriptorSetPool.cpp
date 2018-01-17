/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "DescriptorSetPool.hpp"

#include "DescriptorSet.hpp"
#include "DescriptorSetLayout.hpp"

namespace renderer
{
	DescriptorSetPool::DescriptorSetPool( DescriptorSetLayout const & layout, uint32_t maxSets )
		: m_layout{ layout }
		, m_maxSets{ maxSets }
	{
	}

	void DescriptorSetPool::allocate( uint32_t count )const
	{
		assert( m_allocated + count <= m_maxSets );
		m_allocated = std::min( m_allocated + count, m_maxSets );
	}
}
