/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/Pipeline.hpp"

namespace renderer
{
	Pipeline::Pipeline( Device const & device
		, PipelineLayout const & layout
		, GraphicsPipelineCreateInfo && createInfo )
		: m_createInfo
		{
			std::move( createInfo.stages ),
			createInfo.renderPass,
			createInfo.vertexInputState,
			createInfo.inputAssemblyState,
			createInfo.rasterisationState,
			createInfo.multisampleState,
			createInfo.colourBlendState,
			createInfo.dynamicStates,
			createInfo.depthStencilState,
			createInfo.tessellationState,
			createInfo.viewport,
			createInfo.scissor
		}
		, m_layout{ layout }
	{
	}
}
