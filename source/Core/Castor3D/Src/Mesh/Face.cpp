#include "Face.hpp"

using namespace Castor;

namespace Castor3D
{
	Face::Face( uint32_t a, uint32_t b, uint32_t c )
		: m_face{ { { a, b, c } } }
	{
	}
}
