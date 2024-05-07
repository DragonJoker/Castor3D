/*
See LICENSE file in root folder
*/
#ifndef ___RenderPanel___
#define ___RenderPanel___

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>

#include <future>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <GuiCommon/System/DebugMeshManager.hpp>
#include <GuiCommon/System/NodeState.hpp>

#include <Castor3D/Engine.hpp>

#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace CastorViewer
{
	enum class eTIMER_ID
	{
		FORWARD = 1,
		BACK,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		MOUSE,
		MOVEMENT,
		COUNT,
		CLIPBOARD_CHANGE,
	};

	class MouseNodeEvent;
	class TranslateNodeEvent;

	CU_DeclareSmartPtr( CastorViewer, MouseNodeEvent, );
	CU_DeclareSmartPtr( CastorViewer, TranslateNodeEvent, );

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
		void updateWindow( castor3d::RenderWindowDesc const & window );
		void onKeyUp( wxKeyEvent & event );

		castor3d::RenderWindow & getRenderWindow()const
		{
			return *m_renderWindow;
		}

		void disableWindowResize()
		{
			m_resizeWindow = false;
		}

		void enableWindowResize()
		{
			m_resizeWindow = true;
		}

	private:
		void doResetTimers();
		void doStartMovement();
		void doStartTimer( eTIMER_ID id );
		void doStopMovement();
		void doStopTimer( eTIMER_ID id );
		float doGetRealSpeed()const noexcept;
		void doResetNode();
		void doTurnCameraHoriz();
		void doTurnCameraVertic();
		void doChangeCamera();
		void doReloadScene();
		float doTransformX( int x );
		float doTransformY( int y );
		int doTransformX( float x );
		int doTransformY( float y );
		void doUpdateSelectedGeometry( castor3d::GeometryRPtr geometry
			, castor3d::Submesh const * submesh );
		GuiCommon::NodeState & doAddNodeState( castor3d::SceneNodeRPtr node
			, bool camera );
		void doUpdateSpeed( float factor = 1.0f );
		void doSetCamera( castor3d::Camera & camera );

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
		void onClipboardText( wxCommandEvent & event );

	public:
		float m_x{};
		float m_y{};
		float m_oldX{};
		float m_oldY{};
		bool m_altdown{};
		bool m_mouseLeftDown{};
		bool m_mouseRightDown{};
		bool m_mouseMiddleDown{};
		bool m_resizeWindow{ true };
		bool m_extraSpeed{};
		std::atomic_bool m_movementStarted{};
		castor3d::RenderWindowUPtr m_renderWindow{};
		castor::Array< wxTimer *, size_t( eTIMER_ID::COUNT ) > m_timers{};

		castor3d::SceneNodeRPtr m_lightsNode{};
		castor3d::SceneNodeRPtr m_currentNode{};
		castor::RangedValue< float > m_camSpeed;
		castor::RawUniquePtr< GuiCommon::DebugMeshManager > m_debugMeshManager{};

		castor::StringMap< GuiCommon::NodeStatePtr > m_nodesStates{};
		GuiCommon::NodeState * m_currentState{};
		castor3d::GeometryRPtr m_selectedGeometry{};
		castor3d::Submesh const * m_selectedSubmesh{};

		castor3d::CameraRPtr m_camera{};
		castor3d::SceneRPtr m_scene{};
		castor3d::FrameListenerRPtr m_listener{};

		castor3d::MouseCursor m_cursor{};
		castor::RawUniquePtr< wxClipboard > m_clipboard{};
		std::atomic_bool m_setClipboardText;
		std::promise< castor::U32String > m_clipGet{};
		castor::Mutex m_mtxClipSet{};
		castor::U32String m_clipSet{};

		GuiCommon::I3DControllerUPtr m_3dController{};
	};
}

#endif
