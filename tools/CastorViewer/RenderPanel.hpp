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
#include <Castor3D/Render/RenderTarget.hpp>

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
		MOVEMENT,
		COUNT,
		CLIPBOARD_CHANGE,
	};

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
		void updateWindow( c3d::RenderWindowDesc const & window );
		void select( c3d::Geometry const * geometry, c3d::Submesh const * submesh );
		void select( c3d::LightInstance const * light );
		void select( c3d::SceneNode * node
			, bool cameraNode = false );
		void select( c3d::Camera * camera );
		void onKeyUp( wxKeyEvent & event );

		c3d::RenderWindow & getRenderWindow()const
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
		float doTransformX( int x )const;
		float doTransformY( int y )const;
		int doTransformX( float x )const;
		int doTransformY( float y )const;
		void doUpdateSelectedGeometry( c3d::Geometry const * geometry
			, c3d::Submesh const * submesh
			, bool forwardToMain );
		GuiCommon::NodeState & doAddNodeState( c3d::SceneNodeRPtr node
			, bool camera );
		void doUpdateSpeed( float factor = 1.0f );

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
		void onMouseLDown( wxMouseEvent & event );
		void onMouseLUp( wxMouseEvent & event );
		void onMouseMDown( wxMouseEvent & event );
		void onMouseMUp( wxMouseEvent & event );
		void onMouseRDown( wxMouseEvent & event );
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
		double m_speedMult{ 1.0 };
		std::atomic_bool m_movementStarted{};
		c3d::RenderWindowUPtr m_renderWindow{};
		c3d::Array< c3d::RawUniquePtr< wxTimer >, size_t( eTIMER_ID::COUNT ) > m_timers{};

		c3d::SceneNodeRPtr m_lightsNode{};
		c3d::SceneNodeRPtr m_currentNode{};
		c3d::RangedValue< float > m_camSpeed;
		c3d::RawUniquePtr< GuiCommon::DebugMeshManager > m_debugMeshManager{};

		c3d::StringMap< GuiCommon::NodeStatePtr > m_nodesStates{};
		GuiCommon::NodeState * m_currentState{};
		c3d::Geometry const * m_selectedGeometry{};
		c3d::Submesh const * m_selectedSubmesh{};

		c3d::CameraRPtr m_camera{};
		c3d::SceneRPtr m_scene{};
		c3d::FrameListenerRPtr m_listener{};

		c3d::MouseCursor m_cursor{};
		c3d::RawUniquePtr< wxClipboard > m_clipboard{};
		std::atomic_bool m_setClipboardText;
		std::promise< c3d::U32String > m_clipGet{};
		c3d::Mutex m_mtxClipSet{};
		c3d::U32String m_clipSet{};

		GuiCommon::I3DControllerUPtr m_3dController{};
		c3d::RenderTarget::OnInitialisedConnection m_onTargetInitialised;
	};
}

#endif
