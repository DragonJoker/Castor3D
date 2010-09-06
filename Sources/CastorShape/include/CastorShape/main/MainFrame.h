//******************************************************************************
#ifndef ___CSMAINFRAME___
#define ___CSMAINFRAME___
//******************************************************************************
#include "Module_GUI.h"
#include "../geometry/NewGeometryDialog.h"
//#include <Castor3D/Log.h>
//#include <Castor3D/math/Module_Math.h>
//#include <Castor3D/material/Module_Material.h>
//******************************************************************************
class CSMainFrame : public wxFrame
{
private:
	enum eMENU_BAR_IDs
	{
		mbExit,
		mbSaveScene,
		mLoadScene,
		mbNewGeometry,
		mbNewCone,
		mbNewCube,
		mbNewCylinder,
		mbNewIcosaedron,
		mbNewPlane,
		mbNewSphere,
		mbNewTorus,
		mbNewProjection,
		mbNewMaterial,
		mbGeometries,
		mbMaterials,
		mbSelectGeometries,
		mbSelectPoints,
		mbCloneSelection,
		mbSubdividePNTriangles,
		mbSubdivideLoop,
		mbSelectNone,
		mbSelect,
		mbModify,
		mbNone,
		mbRender
	};

	enum eFRAME_IDs
	{
		mainFrame,
		frame3d,
		frame2d,
		mfNewGeometry,
		mfNewMaterial,
		mfGeometries,
		mfMaterials
	};

private:
	CSRenderPanel * m_3dFrame;
	CSRenderPanel * m_2dFrameHD;
	CSRenderPanel * m_2dFrameBG;
	CSRenderPanel * m_2dFrameBD;
	CSRenderPanel * m_selectedFrame;
	wxPanel * m_separator1;
	wxPanel * m_separator2;
	wxPanel * m_separator3;
	wxPanel * m_separator4;
	Castor3D::Geometry * m_repereX;
	Castor3D::Geometry * m_repereY;
	Castor3D::Geometry * m_repereZ;
	Castor3D::Scene * m_mainScene;
	Castor3D::Root * m_castor3D;
	Castor3D::Geometry * m_selectedGeometry;
	std::vector <MaterialInfos *> m_selectedGeometryMaterials;
	MaterialInfos m_selectedMaterial;

	bool m_bWireFrame;

	Castor3D::Log * m_log;

public:
	CSMainFrame( wxWindow * parent, const wxString & title, 
				 const wxPoint & pos = wxDefaultPosition, 
				 const wxSize & size = wxDefaultSize, 
				 long style = wxDEFAULT_FRAME_STYLE, 
				 wxString name = wxString( (wxChar *)"CSMainFrame", 11));
	~CSMainFrame();

	void SelectGeometry( Castor3D::Geometry * p_geometry);
	void SelectVertex( Vector3f * p_vertex);
	void ShowPanels();

private:
	void _onPaint			( wxPaintEvent & event);
	void _onSize			( wxSizeEvent & event);
	void _onMove			( wxMoveEvent & event);
	void _onClose			( wxCloseEvent & event);
    void _onEnterWindow		( wxMouseEvent & event);
	void _onLeaveWindow		( wxMouseEvent & event);
    void _onEraseBackground	( wxEraseEvent & event);

    void _onKeyDown		( wxKeyEvent & event);
    void _onKeyUp		( wxKeyEvent & event);
    void _onMouseLDown	( wxMouseEvent & event);
    void _onMouseLUp	( wxMouseEvent & event);
    void _onMouseMDown	( wxMouseEvent & event);
    void _onMouseMUp	( wxMouseEvent & event);
    void _onMouseRDown	( wxMouseEvent & event);
    void _onMouseRUp	( wxMouseEvent & event);
	void _onMouseMove	( wxMouseEvent & event);
	void _onMouseWheel	( wxMouseEvent & event);

	void _onMenuClose				( wxCommandEvent & event);
	void _onSaveScene				( wxCommandEvent & event);
	void _onLoadScene				( wxCommandEvent & event);
	void _onNewCone					( wxCommandEvent & event);
	void _onNewCube					( wxCommandEvent & event);
	void _onNewCylinder				( wxCommandEvent & event);
	void _onNewIcosaedron			( wxCommandEvent & event);
	void _onNewPlane				( wxCommandEvent & event);
	void _onNewSphere				( wxCommandEvent & event);
	void _onNewTorus				( wxCommandEvent & event);
	void _onNewProjection			( wxCommandEvent & event);
	void _onNewMaterial				( wxCommandEvent & event);
	void _onShowGeometriesList		( wxCommandEvent & event);
	void _onShowMaterialsList		( wxCommandEvent & event);
	void _onSelectGeometries		( wxCommandEvent & event);
	void _onSelectPoints			( wxCommandEvent & event);
	void _onCloneSelection			( wxCommandEvent & event);
	void _onSelectNone				( wxCommandEvent & event);
	void _onSelectAll				( wxCommandEvent & event);
	void _onSelect					( wxCommandEvent & event);
	void _onModify					( wxCommandEvent & event);
	void _onNothing					( wxCommandEvent & event);
	void _onSubdivideAllPNTriangles	( wxCommandEvent & event);
	void _onSubdivideAllLoop		( wxCommandEvent & event);
	void _onRender					( wxCommandEvent & event);

private:
	void _initialise3D();
	void _buildMenuBar();
	void _createGeometry( Castor3D::Mesh::eTYPE p_meshType, String & p_name,
						  const String & p_meshStrVars,
						  const String & p_baseName, Castor3D::Scene * p_scene,
						  CSNewGeometryDialog * p_dialog, unsigned int i=0,
						  unsigned int j=0, float a=0.0, float b=0.0, float c=0.0);

DECLARE_EVENT_TABLE()
};
DECLARE_APP( CSCastorShape)
//******************************************************************************
#endif
//******************************************************************************