/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/VertexLayout.hpp"

#include "Shader/Attribute.hpp"

namespace renderer
{
	VertexLayout::VertexLayout( uint32_t bindingSlot
		, uint32_t stride
		, VertexInputRate inputRate )
		: m_bindingSlot{ bindingSlot }
		, m_stride{ stride }
		, m_inputRate{ inputRate }
	{
	}

	Attribute VertexLayout::createAttribute( uint32_t location
		, Format format
		, uint32_t offset )
	{
		m_attributes.emplace_back( *this
			, format
			, location
			, offset );
		return m_attributes.back();
	}
}
