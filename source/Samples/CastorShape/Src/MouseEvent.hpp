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
#ifndef ___CS_MOUSE_EVENT_H___
#define ___CS_MOUSE_EVENT_H___

#include "RenderPanel.hpp"

#include <Event/Frame/FrameEvent.hpp>

namespace CastorShape
{
	class MouseCameraEvent
		: public Castor3D::FrameEvent
	{
	protected:
		Castor3D::SceneNode * m_pNode;
		RenderPanel * m_panel;
		Castor::real m_rDeltaX;
		Castor::real m_rDeltaY;
		Castor::real m_rDeltaZ;

	public:
		MouseCameraEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~MouseCameraEvent();

		static void Add( Castor3D::FrameEventUPtr && p_pThis, Castor3D::FrameListenerSPtr p_pListener, Castor::real p_deltaX, Castor::real p_deltaY, Castor::real p_deltaZ );
	};

	class CameraRotateEvent : public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraRotateEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};

	class CameraTranslateEvent : public MouseCameraEvent
	{
	public:
		CameraTranslateEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraTranslateEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraTranslateEvent();

		virtual bool Apply();
	};
}

#endif
