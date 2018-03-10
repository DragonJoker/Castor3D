/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/ClearValue.hpp"

namespace renderer
{
	ClearValue::ClearValue()
		: m_colour{}
		, m_isColour{ true }
	{
	}

	ClearValue::ClearValue( RgbaColour const & colour )
		: m_colour{ colour }
		, m_isColour{ true }
	{
	}

	ClearValue::ClearValue( DepthStencilClearValue const & depthStencil )
		: m_depthStencil{ depthStencil }
	{
	}
}
