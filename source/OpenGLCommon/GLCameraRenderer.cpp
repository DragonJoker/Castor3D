#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlCameraRenderer.h"

using namespace Castor3D;

bool GlCameraRenderer :: Select( ScenePtr p_scene, Camera::eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};				

	CheckGlError( glSelectBuffer( 32, l_selectBuffer), CU_T( "GlCameraRenderer :: Select - glSelectBuffer"));
	CheckGlError( glGetIntegerv( GL_VIEWPORT, l_viewportCoords), CU_T( "GlCameraRenderer :: Select - glGetIntegerv"));
	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PushMatrix();

	CheckGlError( glRenderMode( GL_SELECT), CU_T( "GlCameraRenderer :: Select - glRenderMode"));
	Pipeline::LoadIdentity();

	Pipeline::PickMatrix( real( x), real( l_viewportCoords[3] - y), real( 2), real( 2), l_viewportCoords);
	Pipeline::Perspective( m_target->GetViewport()->GetFOVY(), m_target->GetViewport()->GetRatio(),
						   m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);
	p_scene->Render( eTriangles, 0.0);

	l_objectsFound = glRenderMode( GL_RENDER);

	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PopMatrix();

	Pipeline::MatrixMode( Pipeline::eMatrixModelView);

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

void GlCameraRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
}

void GlCameraRenderer :: EndRender()
{
}