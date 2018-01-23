/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/StencilOpState.hpp"

namespace renderer
{
	StencilOpState::StencilOpState( StencilOp failOp
		, StencilOp passOp
		, StencilOp depthFailOp
		, CompareOp compareOp
		, uint32_t compareMask
		, uint32_t writeMask
		, uint32_t reference )
		: m_failOp{ failOp }
		, m_passOp{ passOp }
		, m_depthFailOp{ depthFailOp }
		, m_compareOp{ compareOp }
		, m_compareMask{ compareMask }
		, m_writeMask{ writeMask }
		, m_reference{ reference }
	{
	}
}
