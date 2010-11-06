#include "PrecompiledHeaders.h"

#include "main/FrameListener.h"
#include "main/RenderPanel.h"

using namespace Castor3D;

CSSelectObjectFrameEvent :: ~CSSelectObjectFrameEvent()
{
}

bool CSSelectObjectFrameEvent :: Apply()
{
	Ray l_ray( m_x, m_y, *m_camera);
	bool l_wantGeo = ! m_geometry.null();
	bool l_wantSubmesh =! m_submesh.null();
	bool l_wantFace = ! m_face.null();
	bool l_wantVertex = ! m_vertex.null();
	m_scene->Select( & l_ray, & m_geometry, & m_submesh, & m_face, & m_vertex);

	if (l_wantGeo)
	{
		m_panel->SelectGeometry( m_geometry);
	}

	if (l_wantVertex)
	{
		m_panel->SelectVertex( m_vertex);
	}

	return true;
}