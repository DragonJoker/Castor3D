#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/CgFrameVariable.hpp"

using namespace Castor3D;
using namespace CgShader;

//***********************************************************************************************************************

CgFrameVariable :: CgFrameVariable( CGprogram p_cgProgram )
	:	m_cgProgram	( p_cgProgram	)
{
}

CgFrameVariable :: CgFrameVariable( CgFrameVariable const & p_rVariable )
	:	m_cgProgram	( p_rVariable.m_cgProgram	)
{
}

CgFrameVariable :: CgFrameVariable( CgFrameVariable && p_rVariable )
	:	m_cgProgram( std::move( p_rVariable.m_cgProgram ) )
{
}

CgFrameVariable :: ~CgFrameVariable()
{
}

//***********************************************************************************************************************