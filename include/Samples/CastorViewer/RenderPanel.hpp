/*
See LICENSE file in root folder
*/
#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <GuiCommon/CubeBoxManager.hpp>
#include <GuiCommon/NodeState.hpp>

#include <Castor3D/Engine.hpp>

#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

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
		eTIMER_ID_MOUSE,
		eTIMER_ID_MOVEMENT,
		eTIMER_ID_COUNT,
	}	eTIMER_ID;

	class MouseNodeEvent;
	class TranslateNodeEvent;
	class KeyboardEvent;

	CU_DeclareSmartPtr( MouseNodeEvent );
	CU_DeclareSmartPtr( TranslateNodeEvent );
	CU_DeclareSmartPtr( KeyboardEvent );

	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * parent
			, wxWindowID id
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize
			, long style = wxDEFAULT_FRAME_STYLE );
		virtual ~RenderPanel();

		void resetRenderWindow();
		void setRenderWindow( castor3d::RenderWindowSPtr window );

		inline castor3d::RenderWindowSPtr getRenderWindow()const
		{
			return m_renderWindow.lock();
		}

		inline void disableWindowResize()
		{
			m_resizeWindow = false;
		}

		inline void enableWindowResize()
		{
			m_resizeWindow = true;
		}

	private:
		void doResetTimers();
		void doStartMovement();
		void doStartTimer( int p_iId );
		void doStopMovement();
		void doStopTimer( int p_iId );
		void doResetNode();
		void doTurnCameraHoriz();
		void doTurnCameraVertic();
		void doChangeCamera();
		void doReloadScene();
		castor::real doTransformX( int x );
		castor::real doTransformY( int y );
		int doTransformX( castor::real x );
		int doTransformY( castor::real y );
		void doUpdateSelectedGeometry( castor3d::GeometrySPtr geometry
			, castor3d::SubmeshSPtr submesh );
		GuiCommon::NodeState & doAddNodeState( castor3d::SceneNodeSPtr node
			, bool camera );

		DECLARE_EVENT_TABLE()
		void OnTimerFwd( wxTimerEvent & event );
		void OnTimerBck( wxTimerEvent & event );
		void OnTimerLft( wxTimerEvent & event );
		void OnTimerRgt( wxTimerEvent & event );
		void OnTimerUp( wxTimerEvent & event );
		void OnTimerDwn( wxTimerEvent & event );
		void OnTimerMouse( wxTimerEvent & event );
		void OnTimerMovement( wxTimerEvent & event );
		void OnSize( wxSizeEvent & event );
		void OnMove( wxMoveEvent & event );
		void OnPaint( wxPaintEvent & event );
		void OnEnterWindow( wxMouseEvent & event );
		void OnLeaveWindow( wxMouseEvent & event );
		void OnEraseBackground( wxEraseEvent & event );
		void OnsetFocus( wxFocusEvent & event );
		void OnKillFocus( wxFocusEvent & event );
		void onKeydown( wxKeyEvent & event );
		void OnKeyUp( wxKeyEvent & event );
		void OnChar( wxKeyEvent & event );
		void OnMouseLDClick( wxMouseEvent & event );
		void OnMouseLdown( wxMouseEvent & event );
		void OnMouseLUp( wxMouseEvent & event );
		void OnMouseMdown( wxMouseEvent & event );
		void OnMouseMUp( wxMouseEvent & event );
		void OnMouseRdown( wxMouseEvent & event );
		void OnMouseRUp( wxMouseEvent & event );
		void OnMouseMove( wxMouseEvent & event );
		void OnMouseWheel( wxMouseEvent & event );
		void OnMenuClose( wxCommandEvent & event );

	public:
		castor::real m_x{ 0.0_r };
		castor::real m_y{ 0.0_r };
		castor::real m_oldX{ 0.0_r };
		castor::real m_oldY{ 0.0_r };
		bool m_altdown{ false };
		bool m_mouseLeftDown{ false };
		bool m_mouseRightDown{ false };
		bool m_mouseMiddleDown{ false };
		bool m_resizeWindow{ true };
		std::atomic_bool m_movementStarted{ false };
		wxTimer * m_timers[eTIMER_ID_COUNT];
		castor3d::RenderWindowWPtr m_renderWindow;
		castor3d::CameraWPtr m_camera;
		castor3d::SceneWPtr m_scene;
		castor3d::FrameListenerSPtr m_listener;
		wxCursor * m_cursorArrow;
		wxCursor * m_cursorHand;
		wxCursor * m_cursorNone;

		castor3d::SceneNodeSPtr m_lightsNode;
		castor3d::SceneNodeSPtr m_currentNode;
		KeyboardEventUPtr m_keyboardEvent;
		castor::RangedValue< castor::real > m_camSpeed;
		std::unique_ptr< GuiCommon::CubeBoxManager > m_cubeManager;

		std::map< castor::String, GuiCommon::NodeStatePtr > m_nodesStates;
		GuiCommon::NodeState * m_currentState{ nullptr };
		castor3d::GeometrySPtr m_selectedGeometry;
		castor3d::SubmeshSPtr m_selectedSubmesh;
	};
}

#endif
