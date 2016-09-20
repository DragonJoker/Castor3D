#include "Shader/GlFrameVariableBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlFrameVariableBase::GlFrameVariableBase( OpenGl & p_gl, GlShaderProgram const & p_program )
		: Holder( p_gl )
		, m_glName( uint32_t( eGL_INVALID_INDEX ) )
		, m_program( p_program )
		, m_presentInProgram( true )
	{
	}

	GlFrameVariableBase::~GlFrameVariableBase()
	{
	}

	void GlFrameVariableBase::GetVariableLocation( char const * p_varName )
	{
		m_glName = GetOpenGl().GetUniformLocation( m_program.GetGlName(), p_varName );
	}
}
