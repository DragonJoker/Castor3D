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
	Castor3D::ScenePtr m_scene;
	Castor3D::GeometryPtr m_geometry;
	Castor3D::SubmeshPtr m_submesh;
	Castor3D::FacePtr m_face;
	Point3rPtr m_vertex;
	Castor3D::CameraPtr m_camera;
	CSRenderPanel * m_panel;
	int m_x;
	int m_y;

public:
	CSSelectObjectFrameEvent( Castor3D::ScenePtr p_scene, Castor3D::GeometryPtr p_geometry,
							  Castor3D::SubmeshPtr p_submesh, Castor3D::FacePtr p_face,
							  Point3rPtr p_vertex, Castor3D::CameraPtr p_camera,
							  CSRenderPanel * p_panel,  int p_x, int p_y)
		:	Castor3D::FrameEvent( Castor3D::FrameEvent::eQueueRender),
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