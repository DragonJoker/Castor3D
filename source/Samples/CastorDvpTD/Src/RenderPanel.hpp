#pragma once

#include <wx/panel.h>

#include <Engine.hpp>
#include <Miscellaneous/PickingPass.hpp>

#include "TowerCategory.hpp"

namespace castortd
{
	typedef enum eTIMER_ID
	{
		eTIMER_ID_FORWARD = 1,
		eTIMER_ID_BACK,
		eTIMER_ID_LEFT,
		eTIMER_ID_RIGHT,
		eTIMER_ID_COUNT,
	}	eTIMER_ID;

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
		void DoResetTimers();
		void DoStartTimer( int p_iId );
		void DoStopTimer( int p_iId );
		void DoResetCamera();
		Castor::real DoTransformX( int x );
		Castor::real DoTransformY( int y );
		int DoTransformX( Castor::real x );
		int DoTransformY( Castor::real y );
		void DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry );
		void DoUpgradeTowerDamage();
		void DoUpgradeTowerSpeed();
		void DoUpgradeTowerRange();

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
		void OnKeyDown( wxKeyEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnMouseLDClick( wxMouseEvent & p_event );
		void OnMouseLDown( wxMouseEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseMDown( wxMouseEvent & p_event );
		void OnMouseMUp( wxMouseEvent & p_event );
		void OnMouseRDown( wxMouseEvent & p_event );
		void OnMouseRUp( wxMouseEvent & p_event );
		void OnMouseMove( wxMouseEvent & p_event );
		void OnMouseWheel( wxMouseEvent & p_event );
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
		bool m_mouseRightDown{ false };
		bool m_mouseMiddleDown{ false };
		wxTimer * m_timer[eTIMER_ID_COUNT];
		Castor3D::PickingPass m_picking;
		Castor3D::RenderWindowWPtr m_renderWindow;
		Castor3D::SceneNodeSPtr m_node;
		Castor3D::FrameListenerSPtr m_listener;
		Castor::Point3r m_originalPosition;
		Castor::Quaternion m_originalOrientation;
		Castor3D::GeometryWPtr m_selectedGeometry;
		Castor3D::SceneNodeSPtr m_marker;
		Castor::real m_camSpeed;
		wxCursor * m_cursorArrow;
		wxCursor * m_cursorNone;
		Game & m_game;
		TowerPtr m_selectedTower;
		LongRangeTower m_longRange;
		ShortRangeTower m_shortRange;
	};
}
