#ifndef ___CS_RENDERPANEL___
#define ___CS_RENDERPANEL___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class CameraRotateEvent;
	class CameraTranslateEvent;

	class RenderPanel : public wxPanel
	{
	public:
		ActionType m_actionType;
		SelectionType m_selectionType;
		real m_deltaX;
		real m_deltaY;
		real m_x;
		real m_y;
		real m_oldX;
		real m_oldY;
		bool m_mouseLeftDown;					//!< The left mouse button status
		bool m_mouseRightDown;					//!< The right mouse button status
		bool m_mouseMiddleDown;					//!< The middle mouse button status
		/**/
		Castor3D::PointLightPtr m_light1;
		Castor3D::PointLightPtr m_light2;
		Castor3D::MaterialPtr m_material;
		Castor3D::MaterialPtr m_materialNoTex;
		/**/

		wxTimer * m_timer;
		Castor3D::Viewport::eTYPE m_renderType;
		Castor3D::ePROJECTION_DIRECTION m_lookAt;
		weak_ptr<Castor3D::RenderWindow> m_renderWindow;
		Castor3D::ScenePtr m_mainScene;
		Castor3D::FrameListenerPtr m_listener;

		shared_ptr<CameraRotateEvent> m_pRotateCamEvent;
		shared_ptr<CameraTranslateEvent> m_pTranslateCamEvent;

	public:
		RenderPanel( wxWindow * parent, wxWindowID p_id,
					   Castor3D::Viewport::eTYPE p_renderType,
					   Castor3D::ScenePtr p_scene,
					   const wxPoint & pos = wxDefaultPosition,
					   const wxSize & size = wxDefaultSize,
					   Castor3D::ePROJECTION_DIRECTION p_look = Castor3D::pdLookToFront,
					   long style = wxDEFAULT_FRAME_STYLE);
		~RenderPanel();

		void Focus();
		void UnFocus();

		void DrawOneFrame();
		void SelectGeometry( Castor3D::GeometryPtr m_geometry);
		void SelectVertex( Castor3D::Vertex * p_vertex);

	public:
		inline void SetSelectionType( SelectionType p_type) { m_selectionType = p_type; }
		inline void SetActionType( ActionType p_type) { m_actionType = p_type; }

		void _onKeyDown			( wxKeyEvent & event);
		void _onKeyUp			( wxKeyEvent & event);
		void _onMouseLDown		( wxMouseEvent & event);
		void _onMouseLUp		( wxMouseEvent & event);
		void _onMouseMDown		( wxMouseEvent & event);
		void _onMouseMUp		( wxMouseEvent & event);
		void _onMouseRDown		( wxMouseEvent & event);
		void _onMouseRUp		( wxMouseEvent & event);
		void _onMouseMove		( wxMouseEvent & event);
		void _onMouseWheel		( wxMouseEvent & event);

	private:
		void _initialiseRenderWindow();
		void _selectGeometry( int p_x, int p_y);
		void _selectVertex( int p_x, int p_y);
		void _selectAll();
		void _cloneSelected();

		void _onTimer			( wxTimerEvent & WXUNUSED(event));

		void _onSize			( wxSizeEvent & event);
		void _onMove			( wxMoveEvent & event);
		void _onPaint			( wxPaintEvent & WXUNUSED(event));
		void _onClose			( wxCloseEvent & event);
		void _onEnterWindow		( wxMouseEvent & event);
		void _onLeaveWindow		( wxMouseEvent & event);
		void _onEraseBackground	( wxEraseEvent & event);
		void _onSetFocus		( wxFocusEvent & event);
		void _onKillFocus		( wxFocusEvent & event);

		void _onMenuClose		( wxCommandEvent & event);

		DECLARE_EVENT_TABLE()
	};
}
#endif
