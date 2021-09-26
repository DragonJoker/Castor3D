#pragma once

#include "TowerCategory.hpp"

#include <GuiCommon/System/NodeState.hpp>

#include <Castor3D/Engine.hpp>

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
		eMouse,
		eCount,
	};

	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * parent, wxSize const & size, Game & game );
		~RenderPanel()override;

		void reset();
		void setRenderTarget( castor3d::RenderTargetSPtr target );

		inline castor3d::RenderWindow const & getRenderWindow()const
		{
			return *m_renderWindow;
		}

	private:
		float doTransformX( int x );
		float doTransformY( int y );
		int doTransformX( float x );
		int doTransformY( float y );
		void doUpdateSelectedGeometry( castor3d::GeometrySPtr geometry );
		void doUpgradeTowerDamage();
		void doUpgradeTowerSpeed();
		void doUpgradeTowerRange();
		void doStartTimer( TimerID id );
		void doStopTimer( TimerID id );

		DECLARE_EVENT_TABLE()
		void OnSize( wxSizeEvent & event );
		void OnMove( wxMoveEvent & event );
		void OnPaint( wxPaintEvent & event );
		void OnsetFocus( wxFocusEvent & event );
		void OnKillFocus( wxFocusEvent & event );
		void onKeyDown( wxKeyEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void OnMouseLdown( wxMouseEvent & event );
		void OnMouseLUp( wxMouseEvent & event );
		void OnMouseRUp( wxMouseEvent & event );
		void OnMouseMove( wxMouseEvent & event );
		void OnMouseWheel( wxMouseEvent & event );
		void OnMouseTimer( wxTimerEvent & event );
		void OnTimerUp( wxTimerEvent & event );
		void OnTimerDown( wxTimerEvent & event );
		void OnTimerLeft( wxTimerEvent & event );
		void OnTimerRight( wxTimerEvent & event );
		void OnNewLongRangeTower( wxCommandEvent & event );
		void OnNewShortRangeTower( wxCommandEvent & event );
		void OnUpgradeTowerSpeed( wxCommandEvent & event );
		void OnUpgradeTowerRange( wxCommandEvent & event );
		void OnUpgradeTowerDamage( wxCommandEvent & event );

	private:
		float m_x{ 0.0f };
		float m_y{ 0.0f };
		float m_oldX{ 0.0f };
		float m_oldY{ 0.0f };
		bool m_mouseLeftDown{ false };
		std::array< wxTimer *, size_t( TimerID::eCount ) > m_timers;
		GuiCommon::NodeStatePtr m_cameraState;
		castor3d::RenderWindowPtr m_renderWindow;
		castor3d::FrameListenerSPtr m_listener;
		castor3d::GeometryWPtr m_selectedGeometry;
		castor3d::SceneNodeSPtr m_marker;
		Game & m_game;
		TowerPtr m_selectedTower;
		LongRangeTower m_longRange;
		ShortRangeTower m_shortRange;
	};
}
