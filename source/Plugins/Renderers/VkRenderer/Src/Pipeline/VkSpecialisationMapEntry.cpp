/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSpecializationMapEntry convert( renderer::SpecialisationMapEntry const & state )
	{
		return VkSpecializationMapEntry
		{
			state.constantID,
			state.offset,
			renderer::getSize( state.format ),
		};
	}
}
