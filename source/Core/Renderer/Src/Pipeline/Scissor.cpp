/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/Scissor.hpp"

namespace renderer
{
	Scissor::Scissor( int32_t x
		, int32_t y
		, uint32_t width
		, uint32_t height )
		: m_offset{ x, y }
		, m_size{ width, height }
	{
	}
}
