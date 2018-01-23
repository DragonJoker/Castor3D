/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/TessellationState.hpp"

namespace renderer
{
	TessellationState::TessellationState( TessellationStateFlags flags
		, uint32_t patchControlPoints )
		: m_flags{ flags }
		, m_patchControlPoints{ patchControlPoints }
	{
	}
}
