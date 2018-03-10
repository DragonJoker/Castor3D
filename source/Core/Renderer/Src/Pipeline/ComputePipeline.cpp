/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/ComputePipeline.hpp"

namespace renderer
{
	ComputePipeline::ComputePipeline( Device const & device
		, PipelineLayout const & layout
		, ComputePipelineCreateInfo && createInfo )
		: m_createInfo
		{
			std::move( createInfo.stage )
		}
		, m_layout{ layout }
	{
	}
}
