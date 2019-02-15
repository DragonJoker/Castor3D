#pragma once

#include <wx/panel.h>
#include <wx/timer.h>

#include <GuiCommon/NodeState.hpp>

#include <Castor3D/Engine.hpp>

#include "TowerCategory.hpp"

namespace castortd
{
	enum class TimerID
	{
		eUp,
		eDown,
		eLeft,
		eRight,
		eMouse,
		eCount,
	};

	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * p_parent, wxSize const & p_size, Game & p_game );
		~RenderPanel();
		void setRenderWindow( castor3d::RenderWindowSPtr p_window );

		inline castor3d::RenderWindowSPtr getRenderWindow()const
		{
			return m_renderWindow.lock();
		}

	private:
		castor::real doTransformX( int x );
		castor::real doTransformY( int y );
		int doTransformX( castor::real x );
		int doTransformY( castor::real y );
		void doUpdateSelectedGeometry( castor3d::GeometrySPtr p_geometry );
		void doUpgradeTowerDamage();
		void doUpgradeTowerSpeed();
		void doUpgradeTowerRange();
		void doStartTimer( TimerID p_id );
		void doStopTimer( TimerID p_id );

		DECLARE_EVENT_TABLE()
		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnsetFocus( wxFocusEvent & p_event );
		void OnKillFocus( wxFocusEvent & p_event );
		void onKeyDown( wxKeyEvent & p_event );
		void onKeyUp( wxKeyEvent & p_event );
		void OnMouseLdown( wxMouseEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseRUp( wxMouseEvent & p_event );
		void OnMouseMove( wxMouseEvent & p_event );
		void OnMouseWheel( wxMouseEvent & p_event );
		void OnMouseTimer( wxTimerEvent & p_event );
		void OnTimerUp( wxTimerEvent & p_event );
		void OnTimerDown( wxTimerEvent & p_event );
		void OnTimerLeft( wxTimerEvent & p_event );
		void OnTimerRight( wxTimerEvent & p_event );
		void OnNewLongRangeTower( wxCommandEvent & p_event );
		void OnNewShortRangeTower( wxCommandEvent & p_event );
		void OnUpgradeTowerSpeed( wxCommandEvent & p_event );
		void OnUpgradeTowerRange( wxCommandEvent & p_event );
		void OnUpgradeTowerDamage( wxCommandEvent & p_event );

	private:
		castor::real m_x{ 0.0_r };
		castor::real m_y{ 0.0_r };
		castor::real m_oldX{ 0.0_r };
		castor::real m_oldY{ 0.0_r };
		bool m_mouseLeftDown{ false };
		std::array< wxTimer *, size_t( TimerID::eCount ) > m_timers;
		GuiCommon::NodeStatePtr m_cameraState;
		castor3d::RenderWindowWPtr m_renderWindow;
		castor3d::FrameListenerSPtr m_listener;
		castor3d::GeometryWPtr m_selectedGeometry;
		castor3d::SceneNodeSPtr m_marker;
		Game & m_game;
		TowerPtr m_selectedTower;
		LongRangeTower m_longRange;
		ShortRangeTower m_shortRange;
	};
}
