#include "Castor3D/Model/Mesh/Submesh/Component/Face.hpp"

using namespace castor;

namespace castor3d
{
	Face::Face( uint32_t a, uint32_t b, uint32_t c )
		: m_face{ { { a, b, c } } }
	{
	}
}
