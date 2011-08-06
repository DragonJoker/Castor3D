#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlCameraRenderer.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

bool GlCameraRenderer :: Select( ScenePtr p_scene, Camera::eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	unsigned int l_selectBuffer[32] = {0};				

	OpenGl::SelectBuffer( 32, l_selectBuffer);
	OpenGl::GetIntegerv( GL_VIEWPORT, l_viewportCoords);
	Pipeline::MatrixMode( Pipeline::eMODE_PROJECTION);
	Pipeline::PushMatrix();

	OpenGl::RenderMode( GL_SELECT);
	Pipeline::LoadIdentity();

	Pipeline::PickMatrix( real( x), real( l_viewportCoords[3] - y), real( 2), real( 2), l_viewportCoords);
	Pipeline::Perspective( m_target->GetViewport()->GetFovY(), m_target->GetViewport()->GetRatio(),
						   m_target->GetViewport()->GetNearView(), m_target->GetViewport()->GetFarView());
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);
	p_scene->Render( ePRIMITIVE_TYPE_TRIANGLES, 0.0);

	l_objectsFound = OpenGl::RenderMode( GL_RENDER);

	Pipeline::MatrixMode( Pipeline::eMODE_PROJECTION);
	Pipeline::PopMatrix();

	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);

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
	CASTOR_TRACK;
}

void GlCameraRenderer :: EndRender()
{
	CASTOR_TRACK;
}