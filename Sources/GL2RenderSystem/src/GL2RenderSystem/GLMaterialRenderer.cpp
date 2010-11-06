#include "PrecompiledHeader.h"

#include "GLMaterialRenderer.h"
#include "GLSubmeshRenderer.h"
#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"

using namespace Castor3D;

void GLPassRenderer :: Cleanup()
{
}

void GLPassRenderer :: Initialise()
{
}

void GLPassRenderer :: Apply( eDRAW_TYPE p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glEnable( GL_BLEND)");
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CheckGLError( "GLPassRenderer :: Apply - glBlendFunc");

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_target->GetAmbient());
	CheckGLError( "GLPassRenderer :: Apply - glMaterialfv( GL_AMBIENT)");
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_target->GetDiffuse());
	CheckGLError( "GLPassRenderer :: Apply - glMaterialfv( GL_DIFFUSE)");
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_target->GetSpecular());
	CheckGLError( "GLPassRenderer :: Apply - glMaterialfv( GL_SPECULAR)");
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_target->GetEmissive());
	CheckGLError( "GLPassRenderer :: Apply - glMaterialfv( GL_EMISSION)");
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_target->GetShininess());
	CheckGLError( "GLPassRenderer :: Apply - glMaterialf( GL_SHININESS)");

	if ( ! m_target->IsDoubleFace())
	{
		glCullFace( GL_FRONT);
		glEnable( GL_CULL_FACE);
	}
}

void GLPassRenderer :: Apply2D( eDRAW_TYPE p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glEnable( GL_BLEND)");
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4fv( m_target->GetAmbient());
}

void GLPassRenderer :: Remove()
{
	glDisable( GL_CULL_FACE);
	CheckGLError( "GLPassRenderer :: Apply - glDisable( GL_CULL_FACE)");
	glDisable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glDisable( GL_BLEND)");
}