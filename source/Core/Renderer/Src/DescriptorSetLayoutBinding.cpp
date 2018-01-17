/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "DescriptorSetLayoutBinding.hpp"

namespace renderer
{
	DescriptorSetLayoutBinding::DescriptorSetLayoutBinding( uint32_t index
		, DescriptorType type
		, ShaderStageFlags flags )
		: m_index{ index }
		, m_type{ type }
		, m_flags{ flags }
	{
	}
}
