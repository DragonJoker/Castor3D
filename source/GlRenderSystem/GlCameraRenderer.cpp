#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlCameraRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

#include <Castor3D/Viewport.hpp>
#include <Castor3D/Scene.hpp>

using namespace GlRender;
using namespace Castor3D;

GlCameraRenderer :: GlCameraRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	CameraRenderer	( p_pRenderSystem	)
	,	m_gl			( p_gl				)
{
}

GlCameraRenderer :: ~GlCameraRenderer()
{
}

bool GlCameraRenderer :: Select( SceneSPtr p_scene, Camera::eSELECTION_MODE CU_PARAM_UNUSED( p_mode ), int x, int y, stSELECT_RESULT & CU_PARAM_UNUSED( p_found ) )
{
	int l_objectsFound = 0;
	int	l_viewportCoords[4] = {0};
	uint32_t l_selectBuffer[32] = {0};

	m_gl.SelectBuffer( 32, l_selectBuffer);
	m_gl.GetIntegerv( eGL_GETINTEGER_PARAM_VIEWPORT, l_viewportCoords);
	m_pRenderSystem->GetPipeline()->MatrixMode( eMTXMODE_PROJECTION );
	m_pRenderSystem->GetPipeline()->PushMatrix();

	m_gl.RenderMode( eGL_RENDER_MODE_SELECT);
	m_pRenderSystem->GetPipeline()->LoadIdentity();

	m_pRenderSystem->GetPipeline()->PickMatrix( real( x), real( l_viewportCoords[3] - y), real( 2), real( 2), l_viewportCoords);
	m_pRenderSystem->GetPipeline()->Perspective( m_target->GetViewport()->GetFovY(), m_target->GetViewport()->GetRatio(), m_target->GetViewport()->GetNear(), m_target->GetViewport()->GetFar());
	p_scene->Render( eTOPOLOGY_TRIANGLES, 0.0, *m_target );

	l_objectsFound = m_gl.RenderMode( eGL_RENDER_MODE_RENDER);

	m_pRenderSystem->GetPipeline()->PopMatrix();
	m_pRenderSystem->GetPipeline()->MatrixMode( eMTXMODE_MODEL );

	if (l_objectsFound > 0)
	{
		uint32_t l_lowestDepth = l_selectBuffer[1];

		for (int i = 1; i < l_objectsFound; i++)
		{
			if (l_selectBuffer[(i * 4) + 1] < l_lowestDepth)
			{
				l_lowestDepth = l_selectBuffer[(i * 4) + 1];
			}
		}

		return true;
	}

	return false;
}

void GlCameraRenderer :: Render()
{
}

void GlCameraRenderer :: EndRender()
{
}
