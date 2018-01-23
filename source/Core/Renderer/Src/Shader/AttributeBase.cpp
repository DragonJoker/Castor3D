/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Shader/AttributeBase.hpp"

namespace renderer
{
	AttributeBase::AttributeBase( VertexLayout & layout
		, AttributeFormat format
		, uint32_t location
		, uint32_t offset )
		: m_format{ format }
		, m_location{ location }
		, m_offset{ offset }
	{
	}
}
