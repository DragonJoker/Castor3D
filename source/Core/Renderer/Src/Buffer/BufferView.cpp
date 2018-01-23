#include "Buffer/BufferView.hpp"

namespace renderer
{
	BufferView::BufferView( Device const & device
		, BufferBase const & buffer
		, PixelFormat format
		, uint32_t offset
		, uint32_t range )
		: m_device{ device }
		, m_buffer{ buffer }
		, m_format{ format }
		, m_offset{ offset }
		, m_range{ range }
	{
	}
}
