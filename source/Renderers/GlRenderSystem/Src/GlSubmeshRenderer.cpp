#include "GlSubmeshRenderer.hpp"
#include "GlRenderSystem.hpp"
#include "GlBuffer.hpp"
#include "GlContext.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

#include <Material.hpp>
#include <Pass.hpp>

using namespace GlRender;
using namespace Castor3D;

GlSubmeshRenderer::GlSubmeshRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	SubmeshRenderer( p_pRenderSystem	)
	,	m_gl( p_gl	)
{
}

GlSubmeshRenderer::~GlSubmeshRenderer()
{
}

bool GlSubmeshRenderer::DoPrepareBuffers( Pass const & p_pass )
{
	ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();
	bool l_bUseShader = l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED;

	if ( l_bUseShader )
	{
		switch ( l_pProgram->GetLanguage() )
		{
		case eSHADER_LANGUAGE_GLSL:
			l_bUseShader = m_gl.IsProgram( std::static_pointer_cast< GlShaderProgram >( l_pProgram )->GetGlProgram() );
			break;
		}
	}

	return SubmeshRenderer::DoPrepareBuffers( p_pass );
}
