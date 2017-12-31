/* See LICENSE file in root folder */
#ifndef ___CS_RENDER_PANEL_H___
#define ___CS_RENDER_PANEL_H___

#include "CastorShapePrerequisites.hpp"

namespace CastorShape
{
	class CameraRotateEvent;
	class CameraTranslateEvent;
	class MainFrame;

	class RenderPanel : public wxPanel
	{
	public:
		RenderPanel( Castor3D::ViewportType p_renderType,
					 Castor3D::SceneSPtr p_scene,
					 wxWindow * parent, wxWindowID p_id,
					 wxPoint const & pos = wxDefaultPosition,
					 wxSize const & size = wxDefaultSize,
					 long style = wxDEFAULT_FRAME_STYLE );
		~RenderPanel();

		void Focus();
		void UnFocus();

		void DrawOneFrame();
		void SelectGeometry( Castor3D::GeometrySPtr m_geometry );
		void SelectVertex( Castor3D::Vertex * p_vertex );
		void DestroyRenderWindow();
		void InitialiseRenderWindow();

		inline void SetSelectionType( SelectionType p_type )
		{
			m_selectionType = p_type;
		}
		inline void SetActionType( ActionType p_type )
		{
			m_actionType = p_type;
		}

		void OnKeyDown( wxKeyEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnMouseLDown( wxMouseEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseMDown( wxMouseEvent & p_event );
		void OnMouseMUp( wxMouseEvent & p_event );
		void OnMouseRDown( wxMouseEvent & p_event );
		void OnMouseRUp( wxMouseEvent & p_event );
		void OnMouseMove( wxMouseEvent & p_event );
		void OnMouseWheel( wxMouseEvent & p_event );

	private:
		void DoSelectGeometry( int p_x, int p_y );
		void DoSelectVertex( int p_x, int p_y );
		void DoSelectAll();
		void DoCloneSelected();

		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnClose( wxCloseEvent & p_event );
		void OnEnterWindow( wxMouseEvent & p_event );
		void OnLeaveWindow( wxMouseEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
		void OnSetFocus( wxFocusEvent & p_event );
		void OnKillFocus( wxFocusEvent & p_event );

		void OnMenuClose( wxCommandEvent & p_event );

		DECLARE_EVENT_TABLE()

	public:
		Castor::real m_deltaX;
		Castor::real m_deltaY;
		Castor::real m_x;
		Castor::real m_y;
		Castor::real m_oldX;
		Castor::real m_oldY;
		Castor::real m_rZoom;
		bool m_mouseLeftDown;	//!< The left mouse button status
		bool m_mouseRightDown;	//!< The right mouse button status
		bool m_mouseMiddleDown;	//!< The middle mouse button status

		Castor3D::ViewportType m_renderType;
		Castor3D::RenderWindowWPtr m_pRenderWindow;
		Castor3D::SceneSPtr m_mainScene;
		Castor3D::FrameListenerSPtr m_listener;

		std::unique_ptr< CameraRotateEvent > m_pRotateCamEvent;
		std::unique_ptr< CameraTranslateEvent > m_pTranslateCamEvent;

		ActionType m_actionType;
		SelectionType m_selectionType;
		Castor3D::PointLightWPtr m_light1;
		Castor3D::PointLightWPtr m_light2;
		Castor3D::MaterialWPtr m_material;
		Castor3D::MaterialWPtr m_materialNoTex;
	};
}
#endif
