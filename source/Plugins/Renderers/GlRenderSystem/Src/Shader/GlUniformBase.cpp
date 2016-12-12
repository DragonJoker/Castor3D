#include "Shader/GlUniformBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlUniformBase::GlUniformBase( OpenGl & p_gl, GlShaderProgram const & p_program )
		: Holder( p_gl )
		, m_glName( uint32_t( GlInvalidIndex ) )
		, m_program( p_program )
		, m_presentInProgram( true )
	{
	}

	GlUniformBase::~GlUniformBase()
	{
	}

	void GlUniformBase::GetVariableLocation( char const * p_varName )
	{
		m_glName = GetOpenGl().GetUniformLocation( m_program.GetGlName(), p_varName );
	}
}
