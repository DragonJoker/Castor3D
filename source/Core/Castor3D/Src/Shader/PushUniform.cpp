#include "PushUniform.hpp"

using namespace Castor;

namespace Castor3D
{
	PushUniform::PushUniform( ShaderProgram & p_program )
		: m_program{ p_program }
	{
	}

	PushUniform::~PushUniform()
	{
	}
}
