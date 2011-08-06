#ifndef ___CS_FrameListener___
#define ___CS_FrameListener___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class SelectObjectFrameEvent : public Castor3D::FrameEvent
	{
	private:
		Castor3D::ScenePtr m_scene;
		Castor3D::GeometryPtr m_geometry;
		Castor3D::SubmeshPtr m_submesh;
		Castor3D::FacePtr m_face;
		Castor3D::Vertex * m_vertex;
		Castor3D::CameraPtr m_camera;
		RenderPanel * m_panel;
		int m_x;
		int m_y;

	public:
		SelectObjectFrameEvent( Castor3D::ScenePtr p_scene, Castor3D::GeometryPtr p_geometry,
								  Castor3D::SubmeshPtr p_submesh, Castor3D::FacePtr p_face,
								  Castor3D::VertexPtr p_vertex, Castor3D::CameraPtr p_camera,
								  RenderPanel * p_panel,  int p_x, int p_y);
		virtual ~SelectObjectFrameEvent();

		virtual bool Apply();
	};
}

#endif