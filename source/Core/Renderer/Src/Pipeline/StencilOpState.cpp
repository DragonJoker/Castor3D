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
		, m_hash
		{ uint16_t(
			( uint16_t( m_failOp ) << 12 )
			| ( uint16_t( m_passOp ) << 8 )
			| ( uint16_t( m_depthFailOp ) << 4 )
			| ( uint16_t( m_compareOp ) << 0 )
		) }
	{
	}
}
