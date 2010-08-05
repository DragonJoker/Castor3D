#ifndef ___CS_FrameListener___
#define ___CS_FrameListener___
/*
#include <Castor3D/main/FrameListener.h>
#include <Castor3D/camera/Module_Camera.h>
#include <Castor3D/geometry/Module_Geometry.h>
#include <Castor3D/math/Module_Math.h>
#include <Castor3D/math/Point.h>
*/
#include "Module_GUI.h"

class CSSelectObjectFrameEvent : public Castor3D::FrameEvent
{
private:
	Castor3D::Scene * m_scene;
	Castor3D::Geometry * m_geometry;
	Castor3D::Submesh * m_submesh;
	Castor3D::Face * m_face;
	Vector3f * m_vertex;
	Castor3D::Camera * m_camera;
	CSRenderPanel * m_panel;
	int m_x;
	int m_y;

public:
	CSSelectObjectFrameEvent( Castor3D::Scene * p_scene, Castor3D::Geometry * p_geometry,
							  Castor3D::Submesh * p_submesh, Castor3D::Face * p_face,
							  Vector3f * p_vertex, Castor3D::Camera * p_camera,
							  CSRenderPanel * p_panel,  int p_x, int p_y)
		:	Castor3D::FrameEvent( Castor3D::fetQueueRender),
			m_scene( p_scene),
			m_geometry( p_geometry),
			m_submesh( p_submesh),
			m_face( p_face),
			m_vertex( p_vertex),
			m_camera( p_camera),
			m_panel( p_panel),
			m_x( p_x),
			m_y( p_y)
	{}
	virtual ~CSSelectObjectFrameEvent();

	virtual bool Apply();
};

#endif