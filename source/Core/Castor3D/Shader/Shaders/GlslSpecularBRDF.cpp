#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

CU_ImplementSmartPtr( castor3d::shader, SpecularBRDF )

namespace castor3d::shader
{
	SpecularBRDF::SpecularBRDF( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}
}
