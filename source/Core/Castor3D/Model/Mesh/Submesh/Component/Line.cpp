#include "Castor3D/Model/Mesh/Submesh/Component/Line.hpp"

namespace c3d
{
	Line::Line( uint32_t a, uint32_t b )noexcept
		: m_line{ { { a, b } } }
	{
	}
}
