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
			eNewIcosahedron,
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
			eRender,
		};

		typedef enum eBMP
		{
			eBMP_FICHIER		= GuiCommon::wxGeometriesListFrame::eBMP_COUNT
								  ,	eBMP_AJOUTER
			,	eBMP_PARAMETRES
			,	eBMP_GEOMETRIES
			,	eBMP_MATERIALS
		}	eBMP;

		typedef enum eEVENT
		{
			eEVENT_INIT3D
		}	eEVENT;

	private:
		RenderPanel					*					m_3dFrame;
		bool											m_bMultiFrames;
		RenderPanel					*					m_2dFrameHD;
		RenderPanel					*					m_2dFrameBG;
		RenderPanel					*					m_2dFrameBD;
		RenderPanel					*					m_selectedFrame;
		wxPanel						*					m_separator1;
		wxPanel						*					m_separator2;
		wxPanel						*					m_separator3;
		wxPanel						*					m_separator4;
		wxMenu						*					m_pFileMenu;
		wxMenu						*					m_pSettingsMenu;
		wxMenu						*					m_pNewMenu;
		Castor3D::GeometrySPtr							m_repereX;
		Castor3D::GeometrySPtr							m_repereY;
		Castor3D::GeometrySPtr							m_repereZ;
		Castor3D::SceneSPtr								m_mainScene;
		Castor3D::GeometrySPtr							m_selectedGeometry;
		std::vector<MaterialInfos *>					m_selectedGeometryMaterials;
		MaterialInfos									m_selectedMaterial;
		std::vector<Castor3D::SubdividerSPtr>			m_arraySubdividers;
		std::vector<Castor3D::SubdividerSPtr>			m_arraySubdividersDummy;

		GuiCommon::wxImagesLoader			*			m_pImagesLoader;

		bool											m_bWireFrame;
		wxTimer 					*				 	m_timer;
		GuiCommon::wxSplashScreen			*			m_pSplashScreen;
		Castor3D::Engine								m_castor3D;
		int												m_iNbGeometries;
		Castor::Path									m_strFilePath;
		Castor3D::eRENDERER_TYPE						m_eRenderer;
		wxTimer 					*					m_pTimerInit;

	public:
		MainFrame( wxWindow * parent, wxString const & title, Castor3D::eRENDERER_TYPE p_eRenderer );
		~MainFrame();

		bool Initialise();
		void SelectGeometry( Castor3D::GeometrySPtr p_geometry );
		void SelectVertex( Castor3D::Vertex * p_vertex );
		void ShowPanels();
		void SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_pValue );
		void LoadScene( wxString const & p_strFileName );

		inline Castor3D::Engine *	GetEngine()
		{
			return &m_castor3D;
		}

	private:
		static void _endSubdivision( void * p_pThis, Castor3D::Subdivider * p_pDivider );
		bool DoInitialise3D();
		bool DoInitialiseImages();
		void _populateToolbar();
		void _setSelectionType( SelectionType p_eType );
		void _setActionType( ActionType p_eType );
		void DoCreateGeometry( Castor3D::eMESH_TYPE p_meshType, Castor::String p_name, Castor::String const & p_meshStrVars, wxString const & p_baseName, Castor3D::SceneSPtr p_scene, NewGeometryDialog * p_dialog, uint32_t i = 0, uint32_t j = 0, Castor::real a = 0.0, Castor::real b = 0.0, Castor::real c = 0.0 );

		DECLARE_EVENT_TABLE()
		void _onTimer( wxTimerEvent		& p_event );
		void _onPaint( wxPaintEvent		& p_event );
		void _onSize( wxSizeEvent		& p_event );
		void _onMove( wxMoveEvent		& p_event );
		void _onClose( wxCloseEvent		& p_event );
		void _onEnterWindow( wxMouseEvent		& p_event );
		void _onLeaveWindow( wxMouseEvent		& p_event );
		void _onEraseBackground( wxEraseEvent		& p_event );
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
		void _onMenuClose( wxCommandEvent	& p_event );
		void _onSaveScene( wxCommandEvent	& p_event );
		void _onLoadScene( wxCommandEvent	& p_event );
		void _onNewCone( wxCommandEvent	& p_event );
		void _onNewCube( wxCommandEvent	& p_event );
		void _onNewCylinder( wxCommandEvent	& p_event );
		void _onNewIcosahedron( wxCommandEvent	& p_event );
		void _onNewPlane( wxCommandEvent	& p_event );
		void _onNewSphere( wxCommandEvent	& p_event );
		void _onNewTorus( wxCommandEvent	& p_event );
		void _onNewProjection( wxCommandEvent	& p_event );
		void _onNewMaterial( wxCommandEvent	& p_event );
		void _onShowGeometriesList( wxCommandEvent	& p_event );
		void _onShowMaterialsList( wxCommandEvent	& p_event );
		void _onSelectGeometries( wxCommandEvent	& p_event );
		void _onSelectPoints( wxCommandEvent	& p_event );
		void _onCloneSelection( wxCommandEvent	& p_event );
		void _onSelectNone( wxCommandEvent	& p_event );
		void _onSelectAll( wxCommandEvent	& p_event );
		void _onSelect( wxCommandEvent	& p_event );
		void _onModify( wxCommandEvent	& p_event );
		void _onNothing( wxCommandEvent	& p_event );
		void _onSubdivideAllPNTriangles( wxCommandEvent	& p_event );
		void _onSubdivideAllLoop( wxCommandEvent	& p_event );
		void _onRender( wxCommandEvent	& p_event );
		void _onFileMenu( wxCommandEvent	& p_event );
		void _onNewMenu( wxCommandEvent	& p_event );
		void _onSettingsMenu( wxCommandEvent	& p_event );
		void OnInit3D( wxTimerEvent &	p_event );
	};
}

#endif
