#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

namespace castor3d
{
	const uint32_t BillboardUbo::BindingPoint = 11u;
	castor::String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	castor::String const BillboardUbo::Dimensions = cuT( "c3d_dimensions" );
}
