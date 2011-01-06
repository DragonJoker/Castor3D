#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/main/FrameListener.h"
#include "CastorShape/main/RenderPanel.h"

using namespace Castor3D;
using namespace CastorShape;

SelectObjectFrameEvent :: SelectObjectFrameEvent( ScenePtr p_scene, GeometryPtr p_geometry,
													  SubmeshPtr p_submesh, FacePtr p_face,
													  VertexPtr p_vertex, CameraPtr p_camera,
													  RenderPanel * p_panel,  int p_x, int p_y)
	:	FrameEvent( FrameEvent::eQueueRender),
		m_scene( p_scene),
		m_geometry( p_geometry),
		m_submesh( p_submesh),
		m_face( p_face),
		m_vertex( p_vertex.get()),
		m_camera( p_camera),
		m_panel( p_panel),
		m_x( p_x),
		m_y( p_y)
{}

SelectObjectFrameEvent :: ~SelectObjectFrameEvent()
{
}

bool SelectObjectFrameEvent :: Apply()
{
	Ray l_ray( m_x, m_y, * m_camera);
	bool l_wantGeo = ! m_geometry == NULL;
	bool l_wantSubmesh =! m_submesh == NULL;
	bool l_wantFace = m_face != NULL;
	bool l_wantVertex = m_vertex != NULL;
	m_scene->Select( & l_ray, m_geometry, m_submesh, & m_face, m_vertex);

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