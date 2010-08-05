//******************************************************************************
#include "PrecompiledHeader.h"

#include "GLMaterialRenderer.h"
#include "GLSubmeshRenderer.h"
#include "GLBuffer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void GLPassRenderer :: Cleanup()
{
}

//******************************************************************************

void GLPassRenderer :: Initialise()
{
}

//******************************************************************************

void GLPassRenderer :: Apply( Submesh * p_submesh, DrawType p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glEnable( GL_BLEND)");
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_target->GetAmbient());
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_target->GetDiffuse());
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_target->GetSpecular());
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_target->GetEmissive());
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_target->GetShininess());

	if ( ! m_target->IsDoubleFace())
	{
		glCullFace( GL_FRONT);
		glEnable( GL_CULL_FACE);
	}
}

void GLPassRenderer :: Apply( DrawType p_displayMode)
{
	glEnable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glEnable( GL_BLEND)");
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4fv( m_target->GetAmbient());
}

//******************************************************************************

void GLPassRenderer :: Remove()
{
	glDisable( GL_CULL_FACE);
	CheckGLError( "GLPassRenderer :: Apply - glDisable( GL_CULL_FACE)");
	glDisable( GL_BLEND);
	CheckGLError( "GLPassRenderer :: Apply - glDisable( GL_BLEND)");
}

//******************************************************************************