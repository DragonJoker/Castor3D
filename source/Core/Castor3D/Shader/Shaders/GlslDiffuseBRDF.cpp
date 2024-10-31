#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"

CU_ImplementSmartPtr( castor3d::shader, DiffuseBRDF )

namespace castor3d::shader
{
	DiffuseBRDF::DiffuseBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{
	}
}
