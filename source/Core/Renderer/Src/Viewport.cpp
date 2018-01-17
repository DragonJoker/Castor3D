/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Viewport.hpp"

namespace renderer
{
	Viewport::Viewport( uint32_t width
		, uint32_t height
		, int32_t x
		, int32_t y )
		: m_offset{ x, y }
		, m_size{ width, height }
	{
	}
}
