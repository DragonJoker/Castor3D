#ifndef ___CS_MainFrame___
#define ___CS_MainFrame___

#include "Module_GUI.hpp"
#include "../geometry/NewGeometryDialog.hpp"

namespace CastorShape
{
	class MainFrame : public wxFrame
	{
	private:
		enum eMENU_BAR_IDs
		{
			eExit,
			eFile,
			eNew,
			eSettings,
			eSaveScene,
			eLoadScene,
			eNewGeometry,
			eNewCone,
			eNewCube,
			eNewCylinder,
			eNewIcosaedron,
			eNewPlane,
			eNewSphere,
			eNewTorus,
			eNewProjection,
			eNewMaterial,
			eGeometries,
			eMaterials,
			eSelectGeometries,
			eSelectPoints,
			eCloneSelection,
			eSubdividePNTriangles,
			eSubdivideLoop,
			eSelectNone,
			eSelect,
			eModify,
			eNone,
			eRender
		};

		enum eBMPs
		{
			eBmpFichier		= GuiCommon::wxGeometriesListFrame::eNbBmps,
			eBmpAjouter,
			eBmpParametres,
			eBmpGeometries,
			eBmpMaterials,
		};

	private:
		RenderPanel									*	m_3dFrame;
		bool											m_bMultiFrames;
		RenderPanel									*	m_2dFrameHD;
		RenderPanel									*	m_2dFrameBG;
		RenderPanel									*	m_2dFrameBD;
		RenderPanel									*	m_selectedFrame;
		wxPanel										*	m_separator1;
		wxPanel										*	m_separator2;
		wxPanel										*	m_separator3;
		wxPanel										*	m_separator4;
		wxMenu										*	m_pFileMenu;
		wxMenu										*	m_pSettingsMenu;
		wxMenu										*	m_pNewMenu;
		Castor3D::GeometryPtr							m_repereX;
		Castor3D::GeometryPtr							m_repereY;
		Castor3D::GeometryPtr							m_repereZ;
		Castor3D::ScenePtr								m_mainScene;
		Castor3D::GeometryPtr							m_selectedGeometry;
		Container<MaterialInfos *>::Vector				m_selectedGeometryMaterials;
		MaterialInfos									m_selectedMaterial;
		Container<Castor3D::SubdividerPtr>::Vector		m_arraySubdividers;
		Container<Castor3D::SubdividerPtr>::Vector		m_arraySubdividersDummy;

		GuiCommon::ImagesLoader						*	m_pImagesLoader;

		bool											m_bWireFrame;
		wxTimer 									* 	m_timer;
		Castor3D::Root									m_castor3D;

	public:
		MainFrame( wxWindow * parent, const wxString & title,
					 const wxPoint & pos = wxDefaultPosition,
					 const wxSize & size = wxDefaultSize,
					 long style = wxDEFAULT_FRAME_STYLE,
					 wxString name = wxString( (wxChar *)"MainFrame", 11));
		~MainFrame();

		void SelectGeometry( Castor3D::GeometryPtr p_geometry);
		void SelectVertex( Castor3D::Vertex * p_vertex);
		void ShowPanels();
		void SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_pValue);

	private:
		static void _endSubdivision( void * p_pThis, Castor3D::Subdivider * p_pDivider);
		void _initialise3D();
		void _populateToolbar();
		void _setSelectionType( SelectionType p_eType);
		void _setActionType( ActionType p_eType);
		void _createGeometry( Castor3D::eMESH_TYPE p_meshType, String p_name,
							  const String & p_meshStrVars,
							  const String & p_baseName, Castor3D::ScenePtr p_scene,
							  NewGeometryDialog * p_dialog, unsigned int i=0,
							  unsigned int j=0, real a=0.0, real b=0.0, real c=0.0);

		DECLARE_EVENT_TABLE()
		void _onTimer			( wxTimerEvent & event);
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
		void _onFileMenu				( wxCommandEvent & event);
		void _onNewMenu					( wxCommandEvent & event);
		void _onSettingsMenu			( wxCommandEvent & event);

	};
}

#endif
