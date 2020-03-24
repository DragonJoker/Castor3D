#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

namespace castor3d
{
	uint32_t const MorphingUbo::BindingPoint = 9u;
	castor::String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	castor::String const MorphingUbo::Time = cuT( "c3d_time" );
}
