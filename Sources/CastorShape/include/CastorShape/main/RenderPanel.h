//******************************************************************************
#ifndef ___CS_RENDERPANEL___
#define ___CS_RENDERPANEL___
//******************************************************************************
#include "Module_GUI.h"
/*
#include <Castor3D/render_system/Module_Render.h>
#include <Castor3D/scene/Module_Scene.h>
#include <Castor3D/camera/Module_Camera.h>
#include <Castor3D/math/Point.h>
*/
//******************************************************************************
class CSRenderPanel : public wxPanel
{
public:
	ActionType m_actionType;
	SelectionType m_selectionType;
	float m_deltaX;
	float m_deltaY;
	float m_x;
	float m_y;
	float m_oldX;
	float m_oldY;
	bool m_mouseLeftDown;					//!< The left mouse button status
	bool m_mouseRightDown;					//!< The right mouse button status
	bool m_mouseMiddleDown;					//!< The middle mouse button status
	/**/
	Castor3D::PointLight * m_light1;
	Castor3D::PointLight * m_light2;
	Castor3D::Material * m_material;
	Castor3D::Material * m_materialNoTex;
	/**/

	wxTimer * m_timer;
	Castor3D::ProjectionType m_renderType;
	Castor3D::ProjectionDirection m_lookAt;
	Castor3D::RenderWindow * m_renderWindow;
	Castor3D::Scene * m_mainScene;
	Castor3D::FrameListener * m_listener;

public:
	CSRenderPanel( wxWindow * parent, wxWindowID p_id,
				   Castor3D::ProjectionType p_renderType,
				   Castor3D::Scene * p_scene,
				   const wxPoint & pos = wxDefaultPosition,
				   const wxSize & size = wxDefaultSize,
				   Castor3D::ProjectionDirection p_look = Castor3D::pdLookToFront,
				   long style = wxDEFAULT_FRAME_STYLE);
	~CSRenderPanel();

	void Focus();
	void UnFocus();

	void DrawOneFrame();
	void SelectGeometry( Castor3D::Geometry * m_geometry);
	void SelectVertex( Vector3f * p_vertex);

public:
	inline void SetSelectionType( SelectionType p_type) { m_selectionType = p_type; }
	inline void SetActionType( ActionType p_type) { m_actionType = p_type; }

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

	DECLARE_EVENT_TABLE()
};
DECLARE_APP( CSCastorShape)
//******************************************************************************
#endif
//******************************************************************************