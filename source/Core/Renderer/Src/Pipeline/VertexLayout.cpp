/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/VertexLayout.hpp"

namespace renderer
{
	VertexLayout::VertexLayout( uint32_t bindingSlot
		, uint32_t stride )
		: m_bindingSlot{ bindingSlot }
		, m_stride{ stride }
	{
	}
}
