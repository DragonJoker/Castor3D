/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/Viewport.hpp"

namespace renderer
{
	Viewport::Viewport( uint32_t width
		, uint32_t height
		, int32_t x
		, int32_t y
		, float minZ
		, float maxZ )
		: m_offset{ x, y }
		, m_size{ width, height }
		, m_depthBounds{ minZ, maxZ }
	{
	}
}
