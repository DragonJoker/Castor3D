/* See LICENSE file in root folder */
#ifndef ___CS_MAIN_FRAME_H___
#define ___CS_MAIN_FRAME_H___

#include "CastorShapePrerequisites.hpp"

#include <wx/frame.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>

namespace CastorShape
{
	class RenderPanel;

	class MainFrame
		: public wxFrame
	{
	private:
		typedef enum eEVENT
		{
			eEVENT_INIT3D
		}	eEVENT;

	public:
		MainFrame( GuiCommon::SplashScreen * p_splashScreen, wxString const & title );
		~MainFrame();

		bool Initialise();
		void SelectGeometry( Castor3D::GeometrySPtr p_geometry );
		void SelectVertex( Castor3D::Vertex * p_vertex );
		void ShowPanels();
		void SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_value );
		void LoadScene( wxString const & p_strFileName );

	private:
		void DoInitialiseGUI();
		bool DoInitialise3D();
		void DoPopulateStatusBar();
		void DoPopulateToolbar();
		void DoPopulateMenus();
		void DoInitialisePerspectives();
		void DoSetSelectionType( SelectionType p_type );
		void DoSetActionType( ActionType p_type );
		void DoCreateGeometry( Castor3D::eMESH_TYPE p_meshType, Castor::String p_name, Castor::OutputStream const & p_meshStrVars, wxString const & p_baseName, Castor3D::SceneSPtr p_scene, NewGeometryDialog * p_dialog, uint32_t i = 0, uint32_t j = 0, Castor::real a = 0.0, Castor::real b = 0.0, Castor::real c = 0.0 );
		void DoSubdivideAll( Castor::String const & p_name );

		DECLARE_EVENT_TABLE()
		void OnTimer( wxTimerEvent & p_event );
		void OnPaint( wxPaintEvent & p_event );
		void OnSize( wxSizeEvent & p_event );
		void OnMove( wxMoveEvent & p_event );
		void OnClose( wxCloseEvent & p_event );
		void OnEnterWindow( wxMouseEvent & p_event );
		void OnLeaveWindow( wxMouseEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
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
		void OnMenuClose( wxCommandEvent & p_event );
		void OnSaveScene( wxCommandEvent & p_event );
		void OnLoadScene( wxCommandEvent & p_event );
		void OnNewCone( wxCommandEvent & p_event );
		void OnNewCube( wxCommandEvent & p_event );
		void OnNewCylinder( wxCommandEvent & p_event );
		void OnNewIcosahedron( wxCommandEvent & p_event );
		void OnNewPlane( wxCommandEvent & p_event );
		void OnNewSphere( wxCommandEvent & p_event );
		void OnNewTorus( wxCommandEvent & p_event );
		void OnNewProjection( wxCommandEvent & p_event );
		void OnNewMaterial( wxCommandEvent & p_event );
		void OnShowLists( wxCommandEvent & p_event );
		void OnShowProperties( wxCommandEvent & p_event );
		void OnSelectGeometries( wxCommandEvent & p_event );
		void OnSelectPoints( wxCommandEvent & p_event );
		void OnCloneSelection( wxCommandEvent & p_event );
		void OnSelectNone( wxCommandEvent & p_event );
		void OnSelectAll( wxCommandEvent & p_event );
		void OnSelect( wxCommandEvent & p_event );
		void OnModify( wxCommandEvent & p_event );
		void OnNothing( wxCommandEvent & p_event );
		void OnSubdivideAllPNTriangles( wxCommandEvent & p_event );
		void OnSubdivideAllLoop( wxCommandEvent & p_event );
		void OnFileMenu( wxCommandEvent & p_event );
		void OnNewMenu( wxCommandEvent & p_event );
		void OnSettingsMenu( wxCommandEvent & p_event );

	private:
		// Whole layout
		int m_iPropertiesWidth;
		wxAuiManager m_auiManager;
		wxAuiNotebook * m_sceneTabsContainer;
		GuiCommon::PropertiesHolder * m_propertiesContainer;
		wxPanel * m_renderPanelsContainer;
		wxAuiToolBar * m_toolBar;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;

		// Render panels
		RenderPanel * m_3dFrame;
		bool m_bMultiFrames;
		RenderPanel * m_2dFrameHD;
		RenderPanel * m_2dFrameBG;
		RenderPanel * m_2dFrameBD;
		RenderPanel * m_selectedFrame;
		wxPanel * m_separator1;
		wxPanel * m_separator2;
		wxPanel * m_separator3;
		wxPanel * m_separator4;

		// Objects and materials lists
		GuiCommon::SceneObjectsList * m_sceneObjectsList;
		GuiCommon::MaterialsList * m_materialsList;

		// Contextual menus
		wxMenu * m_pFileMenu;
		wxMenu * m_pSettingsMenu;
		wxMenu * m_pNewMenu;

		// Scene additions
		Castor3D::GeometrySPtr m_repereX;
		Castor3D::GeometrySPtr m_repereY;
		Castor3D::GeometrySPtr m_repereZ;
		Castor3D::SceneWPtr m_mainScene;
		Castor3D::GeometrySPtr m_selectedGeometry;
		std::vector<MaterialInfos *> m_selectedGeometryMaterials;
		MaterialInfos m_selectedMaterial;

		// Other
		bool m_bWireFrame;
		wxTimer * m_timer;
		GuiCommon::SplashScreen * m_pSplashScreen;
		int m_iNbGeometries;
		Castor::Path m_strFilePath;
	};
}

#endif
