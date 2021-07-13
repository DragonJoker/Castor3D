#include "Castor3D/Shader/Shaders/GlslReflection.hpp"

using namespace castor;

namespace castor3d::shader
{
	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils
		, PassFlags const & passFlags )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_passFlags{ passFlags }
	{
	}

	ReflectionModel::ReflectionModel( sdw::ShaderWriter & writer
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}
}
