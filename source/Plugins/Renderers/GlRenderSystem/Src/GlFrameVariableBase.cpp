#include "GlFrameVariableBase.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameVariableBase::GlFrameVariableBase( OpenGl & p_gl, uint32_t * p_uiProgram )
		:	m_iGlIndex( uint32_t( eGL_INVALID_INDEX ) )
		,	m_uiParentProgram( p_uiProgram )
		,	m_bPresentInProgram( true )
		,	m_gl( p_gl )
	{
	}

	GlFrameVariableBase::~GlFrameVariableBase()
	{
	}

	void GlFrameVariableBase::GetVariableLocation( char const * p_pVarName )
	{
		if ( m_uiParentProgram )
		{
			m_iGlIndex = m_gl.GetUniformLocation( *m_uiParentProgram, p_pVarName );
		}
	}
}
