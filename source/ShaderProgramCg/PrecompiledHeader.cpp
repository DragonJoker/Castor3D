#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/Module_CgShader.hpp"

using namespace Castor;
using namespace Castor3D;

void CgShader :: CgCheckError( String const & p_strText )
{
	CGerror l_errorCode = cgGetError();

	while (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + cuT( " - ") + str_utils::from_str( cgGetErrorString( l_errorCode ) ), false );
	}
}