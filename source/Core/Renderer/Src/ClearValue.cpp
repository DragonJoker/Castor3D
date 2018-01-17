/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "ClearValue.hpp"

namespace renderer
{
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
