/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderSubpassState.hpp"

namespace renderer
{
	RenderSubpassState::RenderSubpassState( PipelineStageFlags pipelineStage
		, AccessFlags access )
		: m_pipelineStage{ pipelineStage }
		, m_access{ access }
	{
	}
}
