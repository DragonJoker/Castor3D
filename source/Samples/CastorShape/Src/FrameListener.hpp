/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CS_FRAME_LISTENER_H___
#define ___CS_FRAME_LISTENER_H___

#include "CastorShapePrerequisites.hpp"

#include <Event/Frame/FrameEvent.hpp>

namespace CastorShape
{
	class SelectObjectFrameEvent
		: public Castor3D::FrameEvent
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
