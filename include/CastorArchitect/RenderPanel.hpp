#ifndef ___CA_RenderPanel___
#define ___CA_RenderPanel___

namespace CastorArchitect
{
	class RenderPanel : public wxPanel
	{
	public:
		real m_deltaX;
		real m_deltaY;
		real m_x;
		real m_y;
		real m_oldX;
		real m_oldY;
		bool m_mouseLeftDown;					//!< The left mouse button status
		bool m_mouseRightDown;					//!< The right mouse button status
		bool m_mouseMiddleDown;					//!< The middle mouse button status

		Castor3D::Viewport::eTYPE m_renderType;
		Castor3D::ePROJECTION_DIRECTION m_lookAt;
		Castor3D::RenderWindowPtr m_renderWindow;
		Castor3D::ScenePtr m_mainScene;
		Castor3D::FrameListenerPtr m_listener;

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

	private:
		void _initialiseRenderWindow();

		DECLARE_EVENT_TABLE()
		void _onSize			( wxSizeEvent & event);
		void _onMove			( wxMoveEvent & event);
		void _onPaint			( wxPaintEvent & WXUNUSED(event));
		void _onClose			( wxCloseEvent & event);
		void _onEnterWindow		( wxMouseEvent & event);
		void _onLeaveWindow		( wxMouseEvent & event);
		void _onEraseBackground	( wxEraseEvent & event);
		void _onSetFocus		( wxFocusEvent & event);
		void _onKillFocus		( wxFocusEvent & event);

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

		void _onMenuClose		( wxCommandEvent & event);
	};
}

#endif