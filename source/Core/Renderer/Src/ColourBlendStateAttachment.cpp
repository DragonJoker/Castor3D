/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "ColourBlendStateAttachment.hpp"

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
	{
	}
}
