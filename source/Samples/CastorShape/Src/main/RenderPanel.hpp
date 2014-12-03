#ifndef ___CS_RENDERPANEL___
#define ___CS_RENDERPANEL___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class CameraRotateEvent;
	class CameraTranslateEvent;
	class MainFrame;

	class RenderPanel : public wxPanel
	{
	public:
		Castor::real							m_deltaX;
		Castor::real							m_deltaY;
		Castor::real							m_x;
		Castor::real							m_y;
		Castor::real							m_oldX;
		Castor::real							m_oldY;
		Castor::real							m_rZoom;
		bool									m_mouseLeftDown;	//!< The left mouse button status
		bool									m_mouseRightDown;	//!< The right mouse button status
		bool									m_mouseMiddleDown;	//!< The middle mouse button status

		Castor3D::eVIEWPORT_TYPE				mDoRenderType;
		Castor3D::ePROJECTION_DIRECTION			m_lookAt;
		Castor3D::RenderWindowWPtr				m_pRenderWindow;
		Castor3D::SceneSPtr						m_mainScene;
		Castor3D::FrameListenerSPtr				m_listener;

		std::shared_ptr<CameraRotateEvent>		m_pRotateCamEvent;
		std::shared_ptr<CameraTranslateEvent>	m_pTranslateCamEvent;

		MainFrame 				*				m_pMainFrame;
		ActionType								m_actionType;
		SelectionType							m_selectionType;
		Castor3D::PointLightWPtr				m_light1;
		Castor3D::PointLightWPtr				m_light2;
		Castor3D::MaterialWPtr					m_material;
		Castor3D::MaterialWPtr					m_materialNoTex;

	public:
		RenderPanel( wxWindow * parent, wxWindowID p_id,
					 Castor3D::eVIEWPORT_TYPE pDoRenderType,
					 Castor3D::SceneSPtr p_scene,
					 wxPoint const & pos = wxDefaultPosition,
					 wxSize const & size = wxDefaultSize,
					 Castor3D::ePROJECTION_DIRECTION p_look = Castor3D::ePROJECTION_DIRECTION_FRONT,
					 long style = wxDEFAULT_FRAME_STYLE );
		~RenderPanel();

		void Focus();
		void UnFocus();

		void DrawOneFrame();
		void SelectGeometry( Castor3D::GeometrySPtr m_geometry );
		void SelectVertex( Castor3D::Vertex * p_vertex );
		void InitialiseRenderWindow();

	public:
		inline void SetSelectionType( SelectionType p_type )
		{
			m_selectionType = p_type;
		}
		inline void SetActionType( ActionType p_type )
		{
			m_actionType = p_type;
		}

		void _onKeyDown( wxKeyEvent		& p_event );
		void _onKeyUp( wxKeyEvent		& p_event );
		void _onMouseLDown( wxMouseEvent		& p_event );
		void _onMouseLUp( wxMouseEvent		& p_event );
		void _onMouseMDown( wxMouseEvent		& p_event );
		void _onMouseMUp( wxMouseEvent		& p_event );
		void _onMouseRDown( wxMouseEvent		& p_event );
		void _onMouseRUp( wxMouseEvent		& p_event );
		void _onMouseMove( wxMouseEvent		& p_event );
		void _onMouseWheel( wxMouseEvent		& p_event );

	private:
		void _selectGeometry( int p_x, int p_y );
		void _selectVertex( int p_x, int p_y );
		void _selectAll();
		void _cloneSelected();

		void _onSize( wxSizeEvent		& p_event );
		void _onMove( wxMoveEvent		& p_event );
		void _onPaint( wxPaintEvent		& p_event );
		void _onClose( wxCloseEvent		& p_event );
		void _onEnterWindow( wxMouseEvent		& p_event );
		void _onLeaveWindow( wxMouseEvent		& p_event );
		void _onEraseBackground( wxEraseEvent		& p_event );
		void _onSetFocus( wxFocusEvent		& p_event );
		void _onKillFocus( wxFocusEvent		& p_event );

		void _onMenuClose( wxCommandEvent	& p_event );

		DECLARE_EVENT_TABLE()
	};
}
#endif
