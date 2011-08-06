#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9CameraRenderer.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

bool Dx9CameraRenderer :: Select( ScenePtr p_scene, Camera::eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
/*
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};

	glSelectBuffer( 32, l_selectBuffer);
	CheckDxError( cuT( "Dx9CameraRenderer :: Select - glSelectBuffer"));
	glGetIntegerv( GL_VIEWPORT, l_viewportCoords);
	CheckDxError( cuT( "Dx9CameraRenderer :: Select - glGetIntegerv"));
	Pipeline::MatrixMode( Pipeline::eMatrixProjection);
	Pipeline::PushMatrix();

	glRenderMode( GL_SELECT);
	CheckDxError( cuT( "Dx9CameraRenderer :: Select - glRenderMode"));
	Pipeline::LoadIdentity();

	Pipeline::PickMatrix( real( x), real( l_viewportCoords[3] - y), real( 2), real( 2), l_viewportCoords);
	CheckDxError( cuT( "Dx9CameraRenderer :: Select - gluPickMatrix"));
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
*/
	return false;
}

void Dx9CameraRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	m_target->GetViewport()->Render( p_displayMode);

	if (m_bResize)
	{
		m_bResize = false;
	}
}

void Dx9CameraRenderer :: EndRender()
{
}