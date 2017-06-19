#pragma once

#include <wx/panel.h>
#include <wx/timer.h>

#include <NodeState.hpp>

#include <Engine.hpp>

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
		void SetRenderWindow( Castor3D::RenderWindowSPtr p_window );

		inline Castor3D::RenderWindowSPtr GetRenderWindow()const
		{
			return m_renderWindow.lock();
		}

	private:
		Castor::real DoTransformX( int x );
		Castor::real DoTransformY( int y );
		int DoTransformX( Castor::real x );
		int DoTransformY( Castor::real y );
		void DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry );
		void DoUpgradeTowerDamage();
		void DoUpgradeTowerSpeed();
		void DoUpgradeTowerRange();
		void DoStartTimer( TimerID p_id );
		void DoStopTimer( TimerID p_id );

		DECLARE_EVENT_TABLE()
		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnSetFocus( wxFocusEvent & p_event );
		void OnKillFocus( wxFocusEvent & p_event );
		void OnKeyDown( wxKeyEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnMouseLDown( wxMouseEvent & p_event );
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
		Castor::real m_x{ 0.0_r };
		Castor::real m_y{ 0.0_r };
		Castor::real m_oldX{ 0.0_r };
		Castor::real m_oldY{ 0.0_r };
		bool m_mouseLeftDown{ false };
		std::array< wxTimer *, size_t( TimerID::eCount ) > m_timers;
		GuiCommon::NodeStatePtr m_cameraState;
		Castor3D::RenderWindowWPtr m_renderWindow;
		Castor3D::FrameListenerSPtr m_listener;
		Castor3D::GeometryWPtr m_selectedGeometry;
		Castor3D::SceneNodeSPtr m_marker;
		Game & m_game;
		TowerPtr m_selectedTower;
		LongRangeTower m_longRange;
		ShortRangeTower m_shortRange;
	};
}
