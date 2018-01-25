/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineMultisampleStateCreateInfo convert( renderer::MultisampleState const & state )
	{
		if ( state.getSampleMask() )
		{
			return VkPipelineMultisampleStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				nullptr,
				convert( state.getFlags() ),
				convert( state.getRasterisationSamples() ),
				state.isSampleShadingEnabled(),
				state.getMinSampleShading(),
				&state.getSampleMask(),
				state.isAlphaToCoverageEnabled(),
				state.isAlphaToOneEnabled()
			};
		}

		return VkPipelineMultisampleStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			convert( state.getFlags() ),
			convert( state.getRasterisationSamples() ),
			state.isSampleShadingEnabled(),
			state.getMinSampleShading(),
			nullptr,
			state.isAlphaToCoverageEnabled(),
			state.isAlphaToOneEnabled()
		};
	}
}
