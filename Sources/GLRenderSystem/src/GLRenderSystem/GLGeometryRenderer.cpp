//******************************************************************************
#include "PrecompiledHeader.h"

#include "GLGeometryRenderer.h"
#include "GLContext.h"
#include "GLRenderSystem.h"
#include "GLMaterialRenderer.h"
#include "GLSubmeshRenderer.h"
#include "Module_GL.h"
#include "GLBuffer.h"
#include <Castor3D/main/Root.h>
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

GLGeometryRenderer :: GLGeometryRenderer( GLRenderSystem * p_rs)
	:	GeometryRenderer( p_rs),
		m_glRenderSystem( p_rs)
{
}

//******************************************************************************

GLGeometryRenderer :: ~GLGeometryRenderer()
{
	Cleanup();
}

//******************************************************************************

void GLGeometryRenderer :: Cleanup()
{
/*
	unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_target->GetMesh()->GetNbSubmeshes());
	for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
	{
		m_target->GetMesh()->GetSubmesh( i)->GetRenderer()->Cleanup();
	}
*/
}

//******************************************************************************

void GLGeometryRenderer :: Initialise()
{
	Cleanup();

	if (GLRenderSystem::UseVBO())
	{
		unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_target->GetMesh()->GetNbSubmeshes());

		for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
		{
			m_target->GetMesh()->GetSubmesh( i)->GetRenderer()->GetTrianglesNormals()->Initialise();
			m_target->GetMesh()->GetSubmesh( i)->GetRenderer()->GetLinesNormals()->Initialise();
		}
	}
	m_listCreated = m_target->GetMesh()->GetNbSubmeshes() > 0;
}

//******************************************************************************

void GLGeometryRenderer :: Draw( DrawType p_displayMode)
{
	if ( ! m_target->HasListsCreated())
	{
		return;
	}
	m_target->GetOrientation().ToRotationMatrix( m_matrix);

	ApplyTransformations( *m_target->GetCenter(), m_matrix);

	unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_target->GetMesh()->GetNbSubmeshes());
	Submesh * l_submesh;

	for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
	{
		l_submesh = m_target->GetMesh()->GetSubmesh( i);
		l_submesh->GetMaterial()->Apply( l_submesh, p_displayMode);
		l_submesh->GetMaterial()->Remove();
	}

	RemoveTransformations();
}

//******************************************************************************

void GLGeometryRenderer :: DrawKillBox( Vector3f * p_min, Vector3f * p_max)
{
	glColor3f( 0.0f,0.7f,0.7f);

	//Cube du bas    
	glBegin( GL_LINE_LOOP);
	glVertex3f(p_min->x, p_min->y, p_max->z);
	glVertex3f(p_min->x, p_min->y, p_min->z);
	glVertex3f(p_max->x, p_min->y, p_min->z);
	glVertex3f(p_max->x, p_min->y, p_max->z);
	glEnd();

	//Cube du haut
	glBegin(GL_LINE_LOOP);
	glVertex3f(p_min->x, p_max->y, p_max->z);
	glVertex3f(p_min->x, p_max->y, p_min->z);
	glVertex3f(p_max->x, p_max->y, p_min->z);
	glVertex3f(p_max->x, p_max->y, p_max->z);
	glEnd();


	//les arretes latérales
	glBegin(GL_LINES);
	glVertex3f(p_min->x, p_min->y, p_max->z);
	glVertex3f(p_min->x, p_max->y, p_max->z);

	glVertex3f(p_min->x, p_min->y, p_min->z);
	glVertex3f(p_min->x, p_max->y, p_min->z);

	glVertex3f(p_max->x, p_min->y, p_min->z);
	glVertex3f(p_max->x, p_max->y, p_min->z);

	glVertex3f(p_max->x, p_min->y, p_max->z);
	glVertex3f(p_max->x, p_max->y, p_max->z);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
}

//******************************************************************************

void GLGeometryRenderer :: ApplyTransformations( const Vector3f & p_position,
												 float * p_orientation)
{
	glPushMatrix();
	glTranslatef( p_position.x, p_position.y, p_position.z);
	glMultMatrixf( p_orientation);
}

//******************************************************************************

void GLGeometryRenderer :: RemoveTransformations()
{
	glPopMatrix();
}

//******************************************************************************