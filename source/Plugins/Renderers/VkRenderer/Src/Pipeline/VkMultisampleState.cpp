/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineMultisampleStateCreateInfo convert( renderer::MultisampleState const & state )
	{
		if ( state.sampleMask )
		{
			return VkPipelineMultisampleStateCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				nullptr,
				convert( state.flags ),
				convert( state.rasterisationSamples ),
				state.sampleShadingEnable,
				state.minSampleShading,
				&state.sampleMask,
				state.alphaToCoverageEnable,
				state.alphaToOneEnable
			};
		}

		return VkPipelineMultisampleStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			convert( state.flags ),
			convert( state.rasterisationSamples ),
			state.sampleShadingEnable,
			state.minSampleShading,
			nullptr,
			state.alphaToCoverageEnable,
			state.alphaToOneEnable
		};
	}
}
