#include "FrameListener.hpp"
#include "RenderPanel.hpp"

using namespace Castor3D;
using namespace CastorShape;

SelectObjectFrameEvent::SelectObjectFrameEvent( SceneSPtr p_scene, GeometrySPtr p_geometry, SubmeshSPtr p_submesh, FaceSPtr p_face, VertexSPtr p_vertex, CameraSPtr p_camera, RenderPanel * p_panel,  int p_x, int p_y )
	: FrameEvent( eEVENT_TYPE_POST_RENDER )
	, m_scene( p_scene )
	, m_geometry( p_geometry )
	, m_submesh( p_submesh )
	, m_face( p_face )
	, m_vertex( p_vertex.get() )
	, m_camera( p_camera )
	, m_panel( p_panel )
	, m_x( p_x )
	, m_y( p_y )
{
}

SelectObjectFrameEvent::~SelectObjectFrameEvent()
{
}

bool SelectObjectFrameEvent::Apply()
{
	Ray l_ray( m_x, m_y, *m_camera.lock() );
	bool l_wantGeo = !m_geometry.expired();
	bool l_wantVertex = m_vertex != NULL;
	FaceSPtr l_pFace = m_face.lock();
	GeometrySPtr l_pGeometry = m_geometry.lock();
	SubmeshSPtr l_submesh = m_submesh.lock();
	m_scene.lock()->Select( &l_ray, l_pGeometry, l_submesh, &l_pFace, m_vertex );

	if ( l_wantGeo )
	{
		m_panel->SelectGeometry( l_pGeometry );
	}

	if ( l_wantVertex )
	{
		m_panel->SelectVertex( m_vertex );
	}

	return true;
}
