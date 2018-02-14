/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/DepthStencilState.hpp"

namespace renderer
{
	DepthStencilState::DepthStencilState( DepthStencilStateFlags flags
		, bool depthTestEnable
		, bool depthWriteEnable
		, CompareOp depthCompareOp
		, bool depthBoundsTestEnable
		, bool stencilTestEnable
		, StencilOpState const & front
		, StencilOpState const & back
		, float minDepthBounds
		, float maxDepthBounds )
		: m_flags{ flags }
		, m_depthTestEnable{ depthTestEnable }
		, m_depthWriteEnable{ depthWriteEnable }
		, m_depthCompareOp{ depthCompareOp }
		, m_depthBoundsTestEnable{ depthBoundsTestEnable }
		, m_stencilTestEnable{ stencilTestEnable }
		, m_front{ front }
		, m_back{ back }
		, m_minDepthBounds{ minDepthBounds }
		, m_maxDepthBounds{ maxDepthBounds }
		, m_hash
		{ uint8_t(
			( uint8_t( m_depthTestEnable ) << 7 )
			| ( uint8_t( m_depthWriteEnable ) << 6 )
			| ( uint8_t( m_depthCompareOp ) << 2 )
			| ( uint8_t( m_depthBoundsTestEnable ) << 1 )
			| ( uint8_t( m_stencilTestEnable ) << 0 )
		) }
	{
	}
}
