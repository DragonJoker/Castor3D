/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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

		static void Add( Castor3D::FrameEventSPtr p_pThis, Castor3D::FrameListenerSPtr p_pListener, Castor::real p_deltaX, Castor::real p_deltaY, Castor::real p_deltaZ );
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
