#ifndef ___RenderPanel___
#define ___RenderPanel___

namespace CastorViewer
{
	class CameraRotateEvent;
	class CameraTranslateEvent;

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

		Castor3D::eVIEWPORT_TYPE m_renderType;
		Castor3D::ePROJECTION_DIRECTION m_lookAt;
		weak_ptr<Castor3D::RenderWindow> m_renderWindow;
		Castor3D::ScenePtr m_mainScene;
		Castor3D::FrameListenerPtr m_listener;

		shared_ptr<CameraRotateEvent> m_pRotateCamEvent;
		shared_ptr<CameraTranslateEvent> m_pTranslateCamEvent;

	public:
		RenderPanel( wxWindow * parent, wxWindowID p_id,
					 Castor3D::eVIEWPORT_TYPE p_renderType,
					 Castor3D::ScenePtr p_scene,
					 const wxPoint & pos = wxDefaultPosition,
					 const wxSize & size = wxDefaultSize,
					 Castor3D::ePROJECTION_DIRECTION p_look = Castor3D::ePROJECTION_DIRECTION_FRONT,
					 long style = wxDEFAULT_FRAME_STYLE);
		virtual ~RenderPanel();

		virtual void Focus();
		virtual void UnFocus();

		virtual void DrawOneFrame();

	private:
		virtual void _initialiseRenderWindow();

		DECLARE_EVENT_TABLE()

		virtual void _onSize			( wxSizeEvent & event);
		virtual void _onMove			( wxMoveEvent & event);
		virtual void _onPaint			( wxPaintEvent & event);
		virtual void _onClose			( wxCloseEvent & event);
		virtual void _onEnterWindow		( wxMouseEvent & event);
		virtual void _onLeaveWindow		( wxMouseEvent & event);
		virtual void _onEraseBackground	( wxEraseEvent & event);
		virtual void _onSetFocus		( wxFocusEvent & event);
		virtual void _onKillFocus		( wxFocusEvent & event);

		virtual void _onKeyDown			( wxKeyEvent & event);
		virtual void _onKeyUp			( wxKeyEvent & event);
		virtual void _onMouseLDown		( wxMouseEvent & event);
		virtual void _onMouseLUp		( wxMouseEvent & event);
		virtual void _onMouseMDown		( wxMouseEvent & event);
		virtual void _onMouseMUp		( wxMouseEvent & event);
		virtual void _onMouseRDown		( wxMouseEvent & event);
		virtual void _onMouseRUp		( wxMouseEvent & event);
		virtual void _onMouseMove		( wxMouseEvent & event);
		virtual void _onMouseWheel		( wxMouseEvent & event);

		virtual void _onMenuClose		( wxCommandEvent & event);
	};
}

#endif
