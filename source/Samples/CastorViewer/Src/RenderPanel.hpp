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
#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <Miscellaneous/PickingPass.hpp>

#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>

namespace CastorViewer
{
	typedef enum eTIMER_ID
	{
		eTIMER_ID_FORWARD = 1,
		eTIMER_ID_BACK,
		eTIMER_ID_LEFT,
		eTIMER_ID_RIGHT,
		eTIMER_ID_UP,
		eTIMER_ID_DOWN,
		eTIMER_ID_COUNT,
	}	eTIMER_ID;

	class MouseNodeEvent;
	class RotateNodeEvent;
	class TranslateNodeEvent;
	class KeyboardEvent;

	DECLARE_SMART_PTR( MouseNodeEvent );
	DECLARE_SMART_PTR( RotateNodeEvent );
	DECLARE_SMART_PTR( TranslateNodeEvent );
	DECLARE_SMART_PTR( KeyboardEvent );

	class RenderPanel
		: public wxPanel
		, public Castor::AlignedFrom< Castor::Point3r >
	{
	public:
		RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE );
		virtual ~RenderPanel();

		virtual void Focus();
		virtual void UnFocus();

		virtual void DrawOneFrame();
		void SetRenderWindow( Castor3D::RenderWindowSPtr p_window );

		inline Castor3D::RenderWindowSPtr GetRenderWindow()const
		{
			return m_renderWindow.lock();
		}

	private:
		void DoResetTimers();
		void DoStartTimer( int p_iId );
		void DoStopTimer( int p_iId );
		void DoResetCamera();
		void DoTurnCamera();
		void DoChangeCamera();
		void DoReloadScene();
		Castor::real DoTransformX( int x );
		Castor::real DoTransformY( int y );
		int DoTransformX( Castor::real x );
		int DoTransformY( Castor::real y );
		void DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry, Castor3D::SubmeshSPtr p_submesh );

		DECLARE_EVENT_TABLE()
		void OnTimerFwd( wxTimerEvent & p_event );
		void OnTimerBck( wxTimerEvent & p_event );
		void OnTimerLft( wxTimerEvent & p_event );
		void OnTimerRgt( wxTimerEvent & p_event );
		void OnTimerUp( wxTimerEvent & p_event );
		void OnTimerDwn( wxTimerEvent &	p_event );
		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnEnterWindow( wxMouseEvent & p_event );
		void OnLeaveWindow( wxMouseEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
		void OnSetFocus( wxFocusEvent & p_event );
		void OnKillFocus( wxFocusEvent & p_event );
		void OnKeyDown( wxKeyEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnChar( wxKeyEvent & p_event );
		void OnMouseLDClick( wxMouseEvent & p_event );
		void OnMouseLDown( wxMouseEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseMDown( wxMouseEvent & p_event );
		void OnMouseMUp( wxMouseEvent & p_event );
		void OnMouseRDown( wxMouseEvent & p_event );
		void OnMouseRUp( wxMouseEvent & p_event );
		void OnMouseMove( wxMouseEvent & p_event );
		void OnMouseWheel( wxMouseEvent & p_event );
		void OnMenuClose( wxCommandEvent & p_event );

	public:
		Castor::real m_x;
		Castor::real m_y;
		Castor::real m_oldX;
		Castor::real m_oldY;
		Castor::Point3r m_ptOriginalPosition;
		Castor::Quaternion m_qOriginalOrientation;
		bool m_mouseLeftDown;	//!< The left mouse button status
		bool m_mouseRightDown;	//!< The right mouse button status
		bool m_mouseMiddleDown;	//!< The middle mouse button status
		wxTimer * m_pTimer[eTIMER_ID_COUNT];
		Castor3D::RenderWindowWPtr m_renderWindow;
		Castor3D::CameraWPtr m_camera;
		Castor3D::SceneWPtr m_scene;
		Castor3D::PickingPass m_picking;
		Castor3D::FrameListenerSPtr m_listener;
		Castor3D::GeometryWPtr m_selectedGeometry;
		Castor3D::SubmeshWPtr m_selectedSubmesh;
		Castor3D::MaterialSPtr m_selectedSubmeshMaterialOrig;
		Castor3D::MaterialSPtr m_selectedSubmeshMaterialClone;
		wxCursor * m_pCursorArrow;
		wxCursor * m_pCursorHand;
		wxCursor * m_pCursorNone;

		Castor3D::SceneNodeSPtr m_lightsNode;
		Castor3D::SceneNodeSPtr m_currentNode;
		KeyboardEventUPtr m_keyboardEvent;
		Castor::real m_camSpeed;
	};
}

#endif
