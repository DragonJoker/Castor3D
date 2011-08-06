#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

//***********************************************************************************************************************

void GlFrameVariableBase :: GetVariableLocation( const GLchar * p_pVarName)
{
	m_iGlIndex = OpenGl::GetUniformLocation( * m_uiParentProgram, p_pVarName);
}

GlFrameVariableBase :: GlFrameVariableBase( GLuint * p_uiProgram)
	:	m_iGlIndex( GL_INVALID_INDEX)
	,	m_uiParentProgram( p_uiProgram)
	,	m_bPresentInProgram( true)
{
}

GlFrameVariableBase :: ~GlFrameVariableBase()
{
}

//***********************************************************************************************************************