#pragma once

#include "TowerCategory.hpp"

#include <GuiCommon/System/NodeState.hpp>

#include <Castor3D/Engine.hpp>

#include <wx/defs.h>
#include <wx/panel.h>
#include <wx/timer.h>

namespace castortd
{
	enum class TimerID
	{
		eUp,
		eDown,
		eLeft,
		eRight,
		eCount,
	};

	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * parent, wxSize const & size, Game & game );
		~RenderPanel()override;

		void reset();
		void updateRenderWindow( c3d::RenderWindowDesc const & window );

		c3d::RenderWindow const & getRenderWindow()const
		{
			return *m_renderWindow;
		}

	private:
		float doTransformX( int x )const;
		float doTransformY( int y )const;
		int doTransformX( float x )const;
		int doTransformY( float y )const;
		void doUpdateSelectedGeometry( c3d::GeometryRPtr geometry );
		void doUpgradeTowerDamage();
		void doUpgradeTowerSpeed();
		void doUpgradeTowerRange();
		void doStartTimer( TimerID id );
		void doStopTimer( TimerID id );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
#pragma clang diagnostic ignored "-Wsuggest-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onSize( wxSizeEvent & event );
		void onMove( wxMoveEvent & event );
		void onPaint( wxPaintEvent & event );
		void onSetFocus( wxFocusEvent & event );
		void onKillFocus( wxFocusEvent & event );
		void onKeyDown( wxKeyEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void onMouseLDown( wxMouseEvent & event );
		void onMouseLUp( wxMouseEvent & event );
		void onMouseRUp( wxMouseEvent & event );
		void onMouseMove( wxMouseEvent & event );
		void onMouseWheel( wxMouseEvent & event );
		void onTimerUp( wxTimerEvent & event );
		void onTimerDown( wxTimerEvent & event );
		void onTimerLeft( wxTimerEvent & event );
		void onTimerRight( wxTimerEvent & event );
		void onNewLongRangeTower( wxCommandEvent & event );
		void onNewShortRangeTower( wxCommandEvent & event );
		void onUpgradeTowerSpeed( wxCommandEvent & event );
		void onUpgradeTowerRange( wxCommandEvent & event );
		void onUpgradeTowerDamage( wxCommandEvent & event );

	private:
		float m_x{ 0.0f };
		float m_y{ 0.0f };
		float m_oldX{ 0.0f };
		float m_oldY{ 0.0f };
		bool m_mouseLeftDown{ false };
		c3d::Array< c3d::RawUniquePtr< wxTimer >, size_t( TimerID::eCount ) > m_timers;
		GuiCommon::NodeStatePtr m_cameraState{};
		c3d::RenderWindowUPtr m_renderWindow{};
		c3d::FrameListenerRPtr m_listener{};
		c3d::GeometryRPtr m_selectedGeometry{};
		c3d::SceneNodeRPtr m_marker{};
		Game & m_game;
		TowerPtr m_selectedTower{};
		LongRangeTower m_longRange;
		ShortRangeTower m_shortRange;
	};
}
