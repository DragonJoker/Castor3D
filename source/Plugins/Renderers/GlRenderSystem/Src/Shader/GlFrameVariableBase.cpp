#include "Shader/GlFrameVariableBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameVariableBase::GlFrameVariableBase( OpenGl & p_gl, uint32_t * p_programId )
		: Holder( p_gl )
		, m_glName( uint32_t( eGL_INVALID_INDEX ) )
		, m_parentProgramId( p_programId )
		, m_presentInProgram( true )
	{
	}

	GlFrameVariableBase::~GlFrameVariableBase()
	{
	}

	void GlFrameVariableBase::GetVariableLocation( char const * p_varName )
	{
		if ( m_parentProgramId )
		{
			m_glName = GetOpenGl().GetUniformLocation( *m_parentProgramId, p_varName );
		}
	}
}
