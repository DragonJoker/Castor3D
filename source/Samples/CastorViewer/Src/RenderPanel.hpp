#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <Castor3DPrerequisites.hpp>

#include <Point.hpp>
#include <Quaternion.hpp>

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>

#if HAS_CASTORGUI
#	include <CastorGuiPrerequisites.hpp>
#endif

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

	class RenderPanel
		: public wxPanel
		, public Castor::AlignedFrom< Castor::Point3r >
	{
	private:
		typedef enum eTIMER_ID :
		int
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
		RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE );
		virtual ~RenderPanel();

		virtual void Focus();
		virtual void UnFocus();

		virtual void DrawOneFrame();
		void SetRenderWindow( Castor3D::RenderWindowSPtr p_pWindow );

		inline Castor3D::RenderWindowSPtr GetRenderWindow()const
		{
			return m_pRenderWindow.lock();
		}

	private:
		void DoStartTimer( int p_iId );
		void DoStopTimer( int p_iId );
		void DoResetCamera();
		void DoSwitchPrimitiveType();
		void DoReloadScene();

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

#if HAS_CASTORGUI
		CastorGui::ControlsManagerRPtr m_controlsManager;
#endif
	};
}

#endif
