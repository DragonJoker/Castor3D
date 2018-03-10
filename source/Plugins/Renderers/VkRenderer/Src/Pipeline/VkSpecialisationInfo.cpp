/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

#include "VkSpecialisationMapEntry.hpp"

namespace vk_renderer
{
	VkSpecializationInfo convert( renderer::SpecialisationInfoBase const & state
		, std::vector< VkSpecializationMapEntry > const & mapEntries )
	{
		return VkSpecializationInfo
		{
			uint32_t( mapEntries.size() ),
			mapEntries.data(),
			uint32_t( state.getSize() ),
			state.getData()
		};
	}
}
