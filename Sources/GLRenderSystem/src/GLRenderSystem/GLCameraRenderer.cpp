#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLCameraRenderer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
#include "../../include/GLRenderSystem/GLRenderSystem.h"
//#include <Castor3D/math/Vector3f.h>
using namespace Castor3D;

void GLCameraRenderer :: ApplyTransformations( const Vector3f & p_position, float * p_matrix)
{
	glPushMatrix();
	glTranslatef( p_position.x, p_position.y, p_position.z);
	glMultMatrixf( p_matrix);
}

void GLCameraRenderer :: RemoveTransformations()
{
	glPopMatrix();
}

bool GLCameraRenderer :: Select( Scene * p_scene, SelectionMode p_mode, void ** p_found, int x, int y)
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};				

	glSelectBuffer( 32, l_selectBuffer);
	glGetIntegerv( GL_VIEWPORT, l_viewportCoords);
	glMatrixMode( GL_PROJECTION);
	glPushMatrix();

	glRenderMode( GL_SELECT);
	glLoadIdentity();

	gluPickMatrix( x, l_viewportCoords[3] - y, 2, 2, l_viewportCoords);
	gluPerspective( m_target->GetViewport()->GetFOVY(), m_target->GetViewport()->GetRatio(),
					m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	glMatrixMode( GL_MODELVIEW);
	p_scene->Render( DTTriangles, 0.0);

	l_objectsFound = glRenderMode(GL_RENDER);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	if (l_objectsFound > 0)
	{		
		unsigned int l_lowestDepth = l_selectBuffer[1];
		int l_selectedObject = l_selectBuffer[3];

		for (int i = 1; i < l_objectsFound; i++)
		{
			if (l_selectBuffer[(i * 4) + 1] < l_lowestDepth)
			{
				l_lowestDepth = l_selectBuffer[(i * 4) + 1];
				l_selectedObject = l_selectBuffer[(i * 4) + 3];
			}
		}

		return true;
	}

	return false;
}

void GLViewportRenderer :: Apply( ProjectionType p_type)
{
	glViewport( 0, 0, m_target->GetWindowWidth(), m_target->GetWindowHeight());
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();

	if (p_type == pt3DView)
	{
		gluPerspective( m_target->GetFOVY(), float( m_target->GetWindowWidth()) / float( m_target->GetWindowHeight()), m_target->GetNearView(),
						m_target->GetFarView());
	}
	else if (p_type == pt2DView)
	{
		glOrtho( m_target->GetLeft(), m_target->GetRight(), m_target->GetBottom ()/ m_target->GetRatio(),
				 m_target->GetTop() / m_target->GetRatio(), m_target->GetNearView(), m_target->GetFarView());
	}

	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
}

Vector3f GLViewportRenderer :: GetDirection( const Point2D<int> & p_mouse)
{
	double l_modelMatrix[16];
	double l_projMatrix[16];
	int l_viewport[4] = { 0, 0, m_target->GetWindowWidth(), m_target->GetWindowHeight()};
	float l_depth[1];
//	glGetIntegerv( GL_VIEWPORT, l_viewport);
	glGetDoublev( GL_MODELVIEW_MATRIX, l_modelMatrix);
	GLRenderSystem::GL_CheckError( "Retrieving model matrix");
	glGetDoublev( GL_PROJECTION_MATRIX, l_projMatrix);
	GLRenderSystem::GL_CheckError( "Retrieving projection matrix");
	glReadPixels( p_mouse.x, p_mouse.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, l_depth);
	GLRenderSystem::GL_CheckError( "Retrieving depth");
	double l_x = 0, l_y = 0, l_z = 0;
	gluUnProject( p_mouse.x, p_mouse.y, l_depth[0], l_modelMatrix, l_projMatrix, l_viewport, & l_x, & l_y, & l_z);
	GLRenderSystem::GL_CheckError( "Retrieving coordinates");
	return Vector3f( -(float)l_x, -(float)l_y, -(float)l_z);
}