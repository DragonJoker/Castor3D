#ifndef ___CS_FrameListener___
#define ___CS_FrameListener___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class SelectObjectFrameEvent : public Castor3D::FrameEvent
	{
	private:
		Castor3D::SceneWPtr m_scene;
		Castor3D::GeometryWPtr m_geometry;
		Castor3D::SubmeshWPtr m_submesh;
		Castor3D::FaceWPtr m_face;
		Castor3D::Vertex * m_vertex;
		Castor3D::CameraWPtr m_camera;
		RenderPanel * m_panel;
		int m_x;
		int m_y;

	public:
		SelectObjectFrameEvent( Castor3D::SceneSPtr p_scene, Castor3D::GeometrySPtr p_geometry,
								Castor3D::SubmeshSPtr p_submesh, Castor3D::FaceSPtr p_face,
								Castor3D::VertexSPtr p_vertex, Castor3D::CameraSPtr p_camera,
								RenderPanel * p_panel,  int p_x, int p_y );
		virtual ~SelectObjectFrameEvent();

		virtual bool Apply();
	};
}

#endif