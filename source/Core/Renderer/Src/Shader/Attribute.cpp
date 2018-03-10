/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Shader/Attribute.hpp"

namespace renderer
{
	Attribute::Attribute( VertexLayout const & layout
		, Format format
		, uint32_t location
		, uint32_t offset )
		: m_layout{ layout }
		, m_format{ format }
		, m_location{ location }
		, m_offset{ offset }
	{
	}
}
