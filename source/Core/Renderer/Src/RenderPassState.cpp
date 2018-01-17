/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPassState.hpp"

namespace renderer
{
	RenderPassState::RenderPassState( PipelineStageFlags pipelineStage
		, AccessFlags access
		, ImageLayoutArray const & imageLayouts )
		: m_pipelineStage{ pipelineStage }
		, m_access{ access }
		, m_imageLayouts{ imageLayouts }
	{
	}
}
