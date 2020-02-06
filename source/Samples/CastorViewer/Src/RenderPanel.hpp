#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <Castor3DPrerequisites.hpp>

#include <Point.hpp>
#include <Quaternion.hpp>

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>

namespace CastorViewer
{
	class MouseCameraEvent;
	class CameraRotateEvent;
	class CameraTranslateEvent;
	class FirstPersonCameraRotateEvent;
	class FirstPersonCameraTranslateEvent;
	class KeyboardEvent;

	DECLARE_SMART_PTR( MouseCameraEvent );
	DECLARE_SMART_PTR( CameraRotateEvent );
	DECLARE_SMART_PTR( CameraTranslateEvent );
	DECLARE_SMART_PTR( FirstPersonCameraRotateEvent );
	DECLARE_SMART_PTR( FirstPersonCameraTranslateEvent );
	DECLARE_SMART_PTR( KeyboardEvent );

	typedef enum eCAMERA_MODE
	{
		eCAMERA_MODE_FIXED,
		eCAMERA_MODE_MOBILE,
		eCAMERA_MODE_COUNT,
	}	eCAMERA_MODE;

	class RenderPanel : public wxPanel
	{
	private:
		typedef enum eTIMER_ID : int
		{
			eTIMER_ID_FORWARD,
			eTIMER_ID_BACK,
			eTIMER_ID_LEFT,
			eTIMER_ID_RIGHT,
			eTIMER_ID_UP,
			eTIMER_ID_DOWN,
			eTIMER_ID_COUNT,
		}	eTIMER_ID;
	public:
		Castor::real m_deltaX;
		Castor::real m_deltaY;
		Castor::real m_x;
		Castor::real m_y;
		Castor::real m_oldX;
		Castor::real m_oldY;
		Castor::real m_rZoom;
		Castor::Point3r m_ptOriginalPosition;
		Castor::Quaternion m_qOriginalOrientation;
		bool m_mouseLeftDown;	//!< The left mouse button status
		bool m_mouseRightDown;	//!< The right mouse button status
		bool m_mouseMiddleDown;	//!< The middle mouse button status
		eCAMERA_MODE m_eCameraMode;
		wxTimer * m_pTimer[eTIMER_ID_COUNT];
		Castor3D::RenderWindowWPtr m_pRenderWindow;
		Castor3D::FrameListenerSPtr m_pListener;
		wxCursor * m_pCursorArrow;
		wxCursor * m_pCursorHand;
		wxCursor * m_pCursorNone;

		MouseCameraEventSPtr m_pRotateCamEvent;
		CameraTranslateEventSPtr m_pTranslateCamEvent;
		FirstPersonCameraRotateEventSPtr m_pFpRotateCamEvent;
		FirstPersonCameraTranslateEventSPtr m_pFpTranslateCamEvent;
		KeyboardEventSPtr m_pKeyboardEvent;
		Castor::real m_rFpCamSpeed;

	public:
		RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE );
		virtual ~RenderPanel();

		virtual void Focus();
		virtual void UnFocus();

		virtual void DrawOneFrame();
		void SetRenderWindow( Castor3D::RenderWindowSPtr p_pWindow );

		inline Castor3D::RenderWindowSPtr	GetRenderWindow()const
		{
			return m_pRenderWindow.lock();
		}

	private:
		virtual void DoStartTimer( int p_iId );
		virtual void DoStopTimer( int p_iId );

		DECLARE_EVENT_TABLE()
		virtual void OnTimerFwd( wxTimerEvent & p_event );
		virtual void OnTimerBck( wxTimerEvent & p_event );
		virtual void OnTimerLft( wxTimerEvent & p_event );
		virtual void OnTimerRgt( wxTimerEvent & p_event );
		virtual void OnTimerUp( wxTimerEvent & p_event );
		virtual void OnTimerDwn( wxTimerEvent &	p_event );
		virtual void OnSize( wxSizeEvent & p_event );
		virtual void OnMove( wxMoveEvent & p_event );
		virtual void OnPaint( wxPaintEvent & p_event );
		virtual void OnEnterWindow( wxMouseEvent & p_event );
		virtual void OnLeaveWindow( wxMouseEvent & p_event );
		virtual void OnEraseBackground( wxEraseEvent & p_event );
		virtual void OnSetFocus( wxFocusEvent & p_event );
		virtual void OnKillFocus( wxFocusEvent & p_event );
		virtual void OnKeyDown( wxKeyEvent & p_event );
		virtual void OnKeyUp( wxKeyEvent & p_event );
		virtual void OnMouseLDClick( wxMouseEvent & p_event );
		virtual void OnMouseLDown( wxMouseEvent & p_event );
		virtual void OnMouseLUp( wxMouseEvent & p_event );
		virtual void OnMouseMDown( wxMouseEvent & p_event );
		virtual void OnMouseMUp( wxMouseEvent & p_event );
		virtual void OnMouseRDown( wxMouseEvent & p_event );
		virtual void OnMouseRUp( wxMouseEvent & p_event );
		virtual void OnMouseMove( wxMouseEvent & p_event );
		virtual void OnMouseWheel( wxMouseEvent & p_event );
		virtual void OnMenuClose( wxCommandEvent & p_event );
	};
}

#endif
