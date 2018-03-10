/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorPool.hpp"

namespace renderer
{
	DescriptorPool::DescriptorPool( DescriptorPoolCreateFlags flags )
		: m_automaticFree{ !checkFlag( flags, DescriptorPoolCreateFlag::eFreeDescriptorSet ) }
	{
	}
}
