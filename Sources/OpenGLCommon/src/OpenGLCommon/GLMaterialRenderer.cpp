#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLMaterialRenderer.h"

using namespace Castor3D;

GLPassRenderer :: GLPassRenderer()
{
	m_pfnApply = & GLPassRenderer::_apply;
	m_pfnApply2D = & GLPassRenderer::_apply2D;
	m_pfnRemove = & GLPassRenderer::_remove;
}

void GLPassRenderer :: Cleanup()
{
}

void GLPassRenderer :: Initialise()
{
	m_pfnApply = & GLPassRenderer::_apply;
	m_pfnApply2D = & GLPassRenderer::_apply2D;
	m_pfnRemove = & GLPassRenderer::_remove;
}

void GLPassRenderer :: Render( eDRAW_TYPE p_displayMode)
{
	(this->*m_pfnApply)( p_displayMode);
}

void GLPassRenderer :: Render2D( eDRAW_TYPE p_displayMode)
{
	(this->*m_pfnApply2D)( p_displayMode);
}

void GLPassRenderer :: EndRender()
{
	(this->*m_pfnRemove)();
}

void GLPassRenderer :: _apply( eDRAW_TYPE p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glEnable( GL_BLEND)"));
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glBlendFunc"));

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_target->GetAmbient());
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glMaterialfv( GL_AMBIENT)"));
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_target->GetDiffuse());
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glMaterialfv( GL_DIFFUSE)"));
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_target->GetSpecular());
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glMaterialfv( GL_SPECULAR)"));
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_target->GetEmissive());
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glMaterialfv( GL_EMISSION)"));
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_target->GetShininess());
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glMaterialf( GL_SHININESS)"));

	if ( ! m_target->IsDoubleFace())
	{
		glCullFace( GL_FRONT);
		glEnable( GL_CULL_FACE);
	}
}

void GLPassRenderer :: _apply2D( eDRAW_TYPE p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glEnable( GL_BLEND)"));
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4fv( m_target->GetAmbient());
}

void GLPassRenderer :: _remove()
{
	glDisable( GL_CULL_FACE);
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glDisable( GL_CULL_FACE)"));
	glDisable( GL_BLEND);
	CheckGLError( CU_T( "GLPassRenderer :: Apply - glDisable( GL_BLEND)"));
}