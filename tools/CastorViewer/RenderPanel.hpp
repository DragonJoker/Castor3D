/*
See LICENSE file in root folder
*/
#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <GuiCommon/System/CubeBoxManager.hpp>
#include <GuiCommon/System/NodeState.hpp>

#include <Castor3D/Engine.hpp>

#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

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

	CU_DeclareCUSmartPtr( CastorViewer, MouseNodeEvent, );
	CU_DeclareCUSmartPtr( CastorViewer, TranslateNodeEvent, );

	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * parent
			, wxWindowID id
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize
			, long style = wxDEFAULT_FRAME_STYLE );
		~RenderPanel()override;

		void reset();
		void setTarget( castor3d::RenderTargetSPtr target );
		void onKeyUp( wxKeyEvent & event );

		inline castor3d::RenderWindow & getRenderWindow()const
		{
			return *m_renderWindow;
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
		float doTransformX( int x );
		float doTransformY( int y );
		int doTransformX( float x );
		int doTransformY( float y );
		void doUpdateSelectedGeometry( castor3d::GeometrySPtr geometry
			, castor3d::SubmeshSPtr submesh );
		GuiCommon::NodeState & doAddNodeState( castor3d::SceneNodeRPtr node
			, bool camera );
		void doUpdateMaxSpeed( float factor );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onTimerFwd( wxTimerEvent & event );
		void onTimerBck( wxTimerEvent & event );
		void onTimerLft( wxTimerEvent & event );
		void onTimerRgt( wxTimerEvent & event );
		void onTimerUp( wxTimerEvent & event );
		void onTimerDwn( wxTimerEvent & event );
		void onTimerMouse( wxTimerEvent & event );
		void onTimerMovement( wxTimerEvent & event );
		void onSize( wxSizeEvent & event );
		void onMove( wxMoveEvent & event );
		void onPaint( wxPaintEvent & event );
		void onEnterWindow( wxMouseEvent & event );
		void onLeaveWindow( wxMouseEvent & event );
		void onEraseBackground( wxEraseEvent & event );
		void onSetFocus( wxFocusEvent & event );
		void onKillFocus( wxFocusEvent & event );
		void onKeyDown( wxKeyEvent & event );
		void onChar( wxKeyEvent & event );
		void onMouseLDClick( wxMouseEvent & event );
		void onMouseLdown( wxMouseEvent & event );
		void onMouseLUp( wxMouseEvent & event );
		void onMouseMdown( wxMouseEvent & event );
		void onMouseMUp( wxMouseEvent & event );
		void onMouseRdown( wxMouseEvent & event );
		void onMouseRUp( wxMouseEvent & event );
		void onMouseMove( wxMouseEvent & event );
		void onMouseWheel( wxMouseEvent & event );
		void onMenuClose( wxCommandEvent & event );

	public:
		float m_x{ 0.0f };
		float m_y{ 0.0f };
		float m_oldX{ 0.0f };
		float m_oldY{ 0.0f };
		bool m_altdown{ false };
		bool m_mouseLeftDown{ false };
		bool m_mouseRightDown{ false };
		bool m_mouseMiddleDown{ false };
		bool m_resizeWindow{ true };
		bool m_extraSpeed{ false };
		std::atomic_bool m_movementStarted{ false };
		std::array< wxTimer *, eTIMER_ID_COUNT > m_timers;
		castor3d::RenderWindowPtr m_renderWindow;
		wxCursor * m_cursorArrow;
		wxCursor * m_cursorHand;
		wxCursor * m_cursorNone;

		castor3d::SceneNodeRPtr m_lightsNode{ nullptr };
		castor3d::SceneNodeRPtr m_currentNode{ nullptr };
		castor::RangedValue< float > m_camSpeed;
		std::unique_ptr< GuiCommon::CubeBoxManager > m_cubeManager;

		std::map< castor::String, GuiCommon::NodeStatePtr > m_nodesStates;
		GuiCommon::NodeState * m_currentState{ nullptr };
		castor3d::GeometrySPtr m_selectedGeometry;
		castor3d::SubmeshSPtr m_selectedSubmesh;

		castor3d::CameraRPtr m_camera{};
		castor3d::SceneRPtr m_scene{};
		castor3d::FrameListenerSPtr m_listener;
	};
}

#endif
