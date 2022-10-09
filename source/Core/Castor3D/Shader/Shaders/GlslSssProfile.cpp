#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"

namespace castor3d::shader
{
	SssProfiles::SssProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< SssProfile >{ writer
			, "C3D_SssProfiles"
			, "c3d_sssProfiles"
			, binding
			, set
			, enable }
	{
	}
}
