#include "PrecompiledHeader.h"

#include "GLCameraRenderer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLPipeline.h"

using namespace Castor3D;

void GLCameraRenderer :: ApplyTransformations( const Point3r & p_position, real * p_matrix)
{
	Pipeline::PushMatrix();
	Pipeline::Translate( p_position);
	Pipeline::MultMatrix( p_matrix);
}

void GLCameraRenderer :: RemoveTransformations()
{
	Pipeline::PopMatrix();
}

bool GLCameraRenderer :: Select( ScenePtr p_scene, Camera::eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};				

	glSelectBuffer( 32, l_selectBuffer);
	CheckGLError( "GLCameraRenderer :: Select - glSelectBuffer");
	glGetIntegerv( GL_VIEWPORT, l_viewportCoords);
	CheckGLError( "GLCameraRenderer :: Select - glGetIntegerv");
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::PushMatrix();

	glRenderMode( GL_SELECT);
	CheckGLError( "GLCameraRenderer :: Select - glRenderMode");
	Pipeline::LoadIdentity();

	gluPickMatrix( x, l_viewportCoords[3] - y, 2, 2, l_viewportCoords);
	CheckGLError( "GLCameraRenderer :: Select - gluPickMatrix");
	Pipeline::Perspective( m_target->GetViewport()->GetFOVY(), m_target->GetViewport()->GetRatio(),
		m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	Pipeline::MatrixMode( Pipeline::eModelView);
	p_scene->Render( DTTriangles, 0.0);

	l_objectsFound = glRenderMode( GL_RENDER);

	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::PopMatrix();

	Pipeline::MatrixMode( Pipeline::eModelView);

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

void GLViewportRenderer :: Apply( Viewport::eTYPE p_type)
{
	glViewport( 0, 0, m_target->GetWindowWidth(), m_target->GetWindowHeight());
	CheckGLError( "GLViewportRenderer :: Apply - glViewport");
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::LoadIdentity();

	if (p_type == Viewport::pt3DView)
	{
		Pipeline::Perspective( m_target->GetFOVY(), real( m_target->GetWindowWidth()) / real( m_target->GetWindowHeight()), m_target->GetNearView(),
							   m_target->GetFarView());
	}
	else if (p_type == Viewport::pt2DView)
	{
		Pipeline::Ortho( m_target->GetLeft(), m_target->GetRight(), m_target->GetBottom ()/ m_target->GetRatio(),
						 m_target->GetTop() / m_target->GetRatio(), m_target->GetNearView(), m_target->GetFarView());
	}

	Pipeline::MatrixMode( Pipeline::eModelView);
	Pipeline::LoadIdentity();
}

Point<real,3> GLViewportRenderer :: GetDirection( const Point<int, 2> & p_mouse)
{
	double l_modelMatrix[16];
	double l_projMatrix[16];
	int l_viewport[4] = { 0, 0, m_target->GetWindowWidth(), m_target->GetWindowHeight()};
	real l_depth[1];
	glGetDoublev( GL_MODELVIEW_MATRIX, l_modelMatrix);
	CheckGLError( "GLViewportRenderer :: GetDirection - glGetDoublev");
	glGetDoublev( GL_PROJECTION_MATRIX, l_projMatrix);
	CheckGLError( "GLViewportRenderer :: GetDirection - glGetDoublev");
	glReadPixels( p_mouse[0], p_mouse[1], 1, 1, GL_DEPTH_COMPONENT, GL_REAL, l_depth);
	CheckGLError( "GLViewportRenderer :: GetDirection - glReadPixels");
	double l_x = 0, l_y = 0, l_z = 0;
	gluUnProject( p_mouse[0], p_mouse[1], l_depth[0], l_modelMatrix, l_projMatrix, l_viewport, & l_x, & l_y, & l_z);
	CheckGLError( "GLViewportRenderer :: GetDirection - gluUnProject");
	return Point3r( -real( l_x), -real( l_y), -real( l_z));
}