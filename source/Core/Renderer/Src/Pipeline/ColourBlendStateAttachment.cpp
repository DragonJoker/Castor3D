/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/ColourBlendStateAttachment.hpp"

namespace renderer
{
	ColourBlendStateAttachment::ColourBlendStateAttachment( bool blendEnable
		, BlendFactor srcColorBlendFactor
		, BlendFactor dstColorBlendFactor
		, BlendOp colorBlendOp
		, BlendFactor srcAlphaBlendFactor
		, BlendFactor dstAlphaBlendFactor
		, BlendOp alphaBlendOp
		, ColourComponentFlags colorWriteMask )
		: m_blendEnable{ blendEnable }
		, m_srcColorBlendFactor{ srcColorBlendFactor }
		, m_dstColorBlendFactor{ dstColorBlendFactor }
		, m_colorBlendOp{ colorBlendOp }
		, m_srcAlphaBlendFactor{ srcAlphaBlendFactor }
		, m_dstAlphaBlendFactor{ dstAlphaBlendFactor }
		, m_alphaBlendOp{ alphaBlendOp }
		, m_colorWriteMask{ colorWriteMask }
		, m_hash
		{ uint64_t(
			( uint64_t( m_blendEnable ) << 31 )
			| ( uint64_t( m_srcColorBlendFactor ) << 26 )
			| ( uint64_t( m_dstColorBlendFactor ) << 21 )
			| ( uint64_t( m_colorBlendOp ) << 18 )
			| ( uint64_t( m_srcAlphaBlendFactor ) << 13 )
			| ( uint64_t( m_dstAlphaBlendFactor ) << 8 )
			| ( uint64_t( m_alphaBlendOp ) << 5 )
			| ( uint64_t( m_colorWriteMask ) << 1 )
		) }
	{
	}
}
