/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorSetLayoutBinding.hpp"

namespace renderer
{
	DescriptorSetLayoutBinding::DescriptorSetLayoutBinding( uint32_t index
		, DescriptorType type
		, ShaderStageFlags flags
		, uint32_t count )
		: m_index{ index }
		, m_type{ type }
		, m_flags{ flags }
		, m_count{ count }
	{
	}
}
