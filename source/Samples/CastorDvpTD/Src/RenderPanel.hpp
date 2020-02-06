#pragma once

#include <wx/panel.h>

#include <Engine.hpp>

#include "TowerCategory.hpp"

namespace castortd
{
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

		DECLARE_EVENT_TABLE()
		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseRUp( wxMouseEvent & p_event );
		void OnNewLongRangeTower( wxCommandEvent & p_event );
		void OnNewShortRangeTower( wxCommandEvent & p_event );
		void OnUpgradeTowerSpeed( wxCommandEvent & p_event );
		void OnUpgradeTowerRange( wxCommandEvent & p_event );
		void OnUpgradeTowerDamage( wxCommandEvent & p_event );

	private:
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
