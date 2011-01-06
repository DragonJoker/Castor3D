#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLCameraRenderer.h"

using namespace Castor3D;

bool GLCameraRenderer :: Select( ScenePtr p_scene, Camera::eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};				

	glSelectBuffer( 32, l_selectBuffer);
	CheckGLError( CU_T( "GLCameraRenderer :: Select - glSelectBuffer"));
	glGetIntegerv( GL_VIEWPORT, l_viewportCoords);
	CheckGLError( CU_T( "GLCameraRenderer :: Select - glGetIntegerv"));
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::PushMatrix();

	glRenderMode( GL_SELECT);
	CheckGLError( CU_T( "GLCameraRenderer :: Select - glRenderMode"));
	Pipeline::LoadIdentity();

	gluPickMatrix( x, l_viewportCoords[3] - y, 2, 2, l_viewportCoords);
	CheckGLError( CU_T( "GLCameraRenderer :: Select - gluPickMatrix"));
	Pipeline::Perspective( m_target->GetViewport()->GetFOVY(), m_target->GetViewport()->GetRatio(),
						   m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	Pipeline::MatrixMode( Pipeline::eModelView);
	p_scene->Render( eTriangles, 0.0);

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

void GLCameraRenderer :: Render( eDRAW_TYPE p_displayMode)
{
	glBegin( GL_TRIANGLES);
		glColor3f( 0, 0, 0);
		glVertex3f( 0, 0, 0);
		glVertex3f( 1, 0, 0);
		glVertex3f( 1, 1, 0);
		glVertex3f( 0, 0, 0);
		glVertex3f( 1, 1, 0);
		glVertex3f( 0, 1, 0);
	glEnd();
}

void GLCameraRenderer :: EndRender()
{
}