#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/main/MainFrame.hpp"
#include "CastorShape/main/CastorShape.hpp"
#include "CastorShape/main/RenderPanel.hpp"

#include "CastorShape/geometry/NewConeDialog.hpp"
#include "CastorShape/geometry/NewCubeDialog.hpp"
#include "CastorShape/geometry/NewCylinderDialog.hpp"
#include "CastorShape/geometry/NewIcosaedronDialog.hpp"
#include "CastorShape/geometry/NewPlaneDialog.hpp"
#include "CastorShape/geometry/NewSphereDialog.hpp"
#include "CastorShape/geometry/NewTorusDialog.hpp"
#include "CastorShape/material/MaterialsFrame.hpp"
#include "CastorShape/material/NewMaterialDialog.hpp"

#include "xpms/fichier.xpm"
#include "xpms/ajouter.xpm"
#include "xpms/parametres.xpm"
#include "xpms/geo_blanc.xpm"
#include "xpms/mat_blanc.xpm"
#include "xpms/castor_transparent.xpm"

#define ID_NEW_WINDOW 10000

using namespace Castor3D;
using namespace CastorShape;
using namespace GuiCommon;

unsigned int g_nbGeometries;
MainFrame * g_mainFrame;
DECLARE_APP( CastorShapeApp)

MainFrame :: MainFrame( wxWindow * parent, const wxString & title,
						    const wxPoint & pos, const wxSize & size,
						    long style, wxString name)
	:	wxFrame( parent, wxID_ANY, title, pos, size, style, name)
	,	m_3dFrame			( nullptr)
	,	m_2dFrameHD			( nullptr)
	,	m_2dFrameBG			( nullptr)
	,	m_2dFrameBD			( nullptr)
	,	m_selectedFrame		( nullptr)
	,	m_bWireFrame		( false)
	,	m_bMultiFrames		( true)
	,	m_pImagesLoader		( new ImagesLoader)
	,	m_timer				( nullptr)
{
	wxGetApp().GetSplashScreen()->Step( "Initialisation des images", 1);
	ImagesLoader::AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm);
	m_selectedMaterial.m_ambient[0] = 0.2f;
	m_selectedMaterial.m_ambient[1] = 0.0f;
	m_selectedMaterial.m_ambient[2] = 0.0f;
	m_selectedMaterial.m_emissive[0] = 0.2f;
	m_selectedMaterial.m_emissive[1] = 0.0f;
	m_selectedMaterial.m_emissive[2] = 0.0f;
	g_mainFrame = this;

	Logger::SetFileName( "CastorShape.log");

	CreateStatusBar();
	_populateToolbar();
	wxGetApp().GetSplashScreen()->Step( "Chargement des plugins", 1);
	_initialise3D();
	wxGetApp().DestroySplashScreen();
}

MainFrame :: ~MainFrame()
{
}

void MainFrame :: SelectGeometry( GeometryPtr p_geometry)
{
	if (p_geometry == m_selectedGeometry)
	{
		return;
	}

	if (m_selectedGeometry)
	{
		MeshPtr l_mesh = m_selectedGeometry->GetMesh();
		MaterialInfos * l_infos;

		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_infos = m_selectedGeometryMaterials[i];
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetAmbient( l_infos->m_ambient);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetEmissive( l_infos->m_emissive);
		}

		ClearContainer( m_selectedGeometryMaterials);
	}

	m_selectedGeometry = p_geometry;

	if (m_selectedGeometry)
	{
		MeshPtr l_mesh = m_selectedGeometry->GetMesh();
		MaterialInfos * l_infos;
		for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
		{
			l_infos = new MaterialInfos;
			l_infos->m_ambient[0] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[0];
			l_infos->m_ambient[1] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[1];
			l_infos->m_ambient[2] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetAmbient()[2];
			l_infos->m_emissive[0] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[0];
			l_infos->m_emissive[1] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[1];
			l_infos->m_emissive[2] = l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->GetEmissive()[2];
			m_selectedGeometryMaterials.push_back( l_infos);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetAmbient( m_selectedMaterial.m_ambient);
			l_mesh->GetSubmesh( i)->GetMaterial()->GetPass( 0)->SetEmissive( m_selectedMaterial.m_emissive);
		}
	}
}

void MainFrame :: SelectVertex( Vertex * p_vertex)
{
}

void MainFrame :: ShowPanels()
{
	if (m_3dFrame)
	{
		m_3dFrame->DrawOneFrame();

		if (m_bMultiFrames)
		{
			m_2dFrameHD->DrawOneFrame();
			m_2dFrameBG->DrawOneFrame();
			m_2dFrameBD->DrawOneFrame();
		}
	}
}

void MainFrame :: SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_pValue)
{
	if (p_pValue == NULL)
	{
		if (m_selectedFrame == p_pCheck)
		{
			m_selectedFrame = nullptr;
		}
	}
	else
	{
		m_selectedFrame = p_pValue;
	}
}

void MainFrame :: _initialise3D()
{
	Logger::LogMessage( cuT( "Initialising Castor3D"));

	m_castor3D.Initialise( 25);

	StringArray l_arrayFiles;
	File::ListDirectoryFiles( File::DirectoryGetCurrent(), l_arrayFiles);

	if (l_arrayFiles.size() > 0)
	{
		for (size_t i = 0 ; i < l_arrayFiles.size() ; i++)
		{
			Path l_file = l_arrayFiles[i];

			if (l_file.GetExtension() == CASTOR_DLL_EXT)
			{
				wxGetApp().GetSplashScreen()->SubStatus( l_file.GetFileName().c_str());
				m_castor3D.LoadPlugin( l_file);
			}
		}
	}

	try
	{
		wxStandardPaths * l_paths = (wxStandardPaths *) & wxStandardPaths::Get();
		wxString l_dataDir = l_paths->GetDataDir();
		bool l_bRendererInit = false;
		wxRendererSelector m_dialog( this, wxT( "Castor Shape"));
		wxGetApp().GetSplashScreen()->Step( "Initialisation de la vue", 1);

		int l_iReturn = m_dialog.ShowModal();

		if (l_iReturn != wxID_CANCEL)
		{
			l_bRendererInit = m_castor3D.LoadRenderer( eRENDERER_TYPE( l_iReturn));

			if (l_bRendererInit)
			{
				m_mainScene = Factory<Scene>::Create( "MainScene");

				Logger::LogMessage( cuT( "Castor3D Initialised"));
				int l_width = GetClientSize().x / 2;
				int l_height = GetClientSize().y / 2;
				m_3dFrame = new RenderPanel( this, wxID_ANY, Viewport::e3DView, m_mainScene,
											   wxPoint( 0, 0),
											   wxSize( l_width - 1, l_height - 1));
				wxGetApp().GetSplashScreen()->Step( 1);

				if (m_bMultiFrames)
				{
					m_2dFrameHD = new RenderPanel( this, wxID_ANY, Viewport::e2DView, m_mainScene,
													 wxPoint( l_width + 1, 0),
													 wxSize( l_width - 1, l_height - 1));
					wxGetApp().GetSplashScreen()->Step( 1);
					m_2dFrameBG = new RenderPanel( this, wxID_ANY, Viewport::e2DView, m_mainScene,
													 wxPoint( 0, l_height + 1),
													 wxSize( l_width - 1, l_height - 1), pdLookToLeft);
					wxGetApp().GetSplashScreen()->Step( 1);
					m_2dFrameBD = new RenderPanel( this, wxID_ANY, Viewport::e2DView, m_mainScene,
													 wxPoint( l_width + 1, l_height + 1),
													 wxSize( l_width - 1, l_height - 1), pdLookToTop);
					wxGetApp().GetSplashScreen()->Step( 1);

					m_separator1 = new wxPanel( this, wxID_ANY, wxPoint( 0, l_height - 1),
												wxSize( l_width, 2));
					m_separator2 = new wxPanel( this, wxID_ANY, wxPoint( l_width, l_height - 1),
												wxSize( l_width, 2));
					m_separator3 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, 0),
												wxSize( 2, l_height));
					m_separator4 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, l_height),
												wxSize( 2, l_height));
				}

				m_3dFrame->Show();

				if (m_bMultiFrames)
				{
					m_2dFrameHD->Show();
					m_2dFrameBG->Show();
					m_2dFrameBD->Show();
				}

				ShowPanels();

				DirectionalLightPtr l_light1 = static_pointer_cast<DirectionalLight, Light>( m_mainScene->CreateLight<DirectionalLight>( "Light1", m_mainScene->CreateSceneNode( "Light1Node")));
				if (l_light1)
				{
					l_light1->GetParent()->SetPosition( 0.0f, 0.0f, 1.0f);
					l_light1->SetDiffuse( 1.0f, 1.0f, 1.0f);
					l_light1->SetSpecular( 1.0f, 1.0f, 1.0f);
					l_light1->SetEnabled( true);
				}

				DirectionalLightPtr l_light2 = static_pointer_cast<DirectionalLight, Light>( m_mainScene->CreateLight<DirectionalLight>( "Light2", m_mainScene->CreateSceneNode( "Light2Node")));
				if (l_light2)
				{
					l_light2->GetParent()->SetPosition( 0.0f, -1.0f, 1.0f);
					l_light2->SetDiffuse( 1.0f, 1.0f, 1.0f);
					l_light2->SetSpecular( 1.0f, 1.0f, 1.0f);
					l_light2->SetEnabled( true);
				}

				DirectionalLightPtr l_light3 = static_pointer_cast<DirectionalLight, Light>( m_mainScene->CreateLight<DirectionalLight>( "Light3", m_mainScene->CreateSceneNode( "Light3Node")));
				if (l_light3)
				{
					l_light3->GetParent()->SetPosition( -1.0f, -1.0f, -1.0f);
					l_light3->SetDiffuse( 1.0f, 1.0f, 1.0f);
					l_light3->SetSpecular( 1.0f, 1.0f, 1.0f);
					l_light3->SetEnabled( true);
				}

				if (m_timer == NULL)
				{
					m_timer = new wxTimer( this, 1);
					m_timer->Start( 40);
				}

/*
				OverlayPtr l_pOverlay1 = Factory<Overlay>::Create( m_mainScene, "Overlay1", eOverlayPanel, nullptr, Point2r( 0.25f, 0.25f), Point2r( 0.5f, 0.5f));
				l_pOverlay1->SetMaterial( Factory<Material>::Create( "Overlay1", 1));
				OverlayPtr l_pOverlay2 = Factory<Overlay>::Create( m_mainScene, "Overlay2", eOverlayPanel, l_pOverlay, Point2r( 0.25f, 0.25f), Point2r( 0.5f, 0.5f));
				l_pOverlay2->SetMaterial( Factory<Material>::Create( "Overlay2", 1));
*/
			}
		}
	}
	catch (Castor::Utils::Exception & p_exc)
	{
		wxMessageBox( p_exc.GetDescription(), "Exception", wxOK | wxCENTRE | wxICON_ERROR);
		Close( true);

	}
	catch (...)
	{
		wxMessageBox( cuT( "Problème survenu lors de l'initialisation de Castor3D"), "Exception", wxOK | wxCENTRE | wxICON_ERROR);
		Close( true);
	}
}

void MainFrame :: _populateToolbar()
{
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL);

	l_pToolbar->SetBackgroundColour( * wxWHITE);
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32));

	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append( eSaveScene,						cuT( "&Sauver la Scene\tCTRL+F+S"));
	m_pFileMenu->Append( eLoadScene,						cuT( "&Ouvrir une Scene\tCTRL+F+O"));
	m_pFileMenu->Append( eRender,							cuT( "&Rendu de scène\tCTRL+F+R"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append( eExit,								cuT( "&Quitter\tALT+F4"));

	m_pNewMenu = new wxMenu();
	wxMenu * l_pSubMenu = new wxMenu();
	l_pSubMenu->Append( eNewCone,							cuT( "C&one"));
	l_pSubMenu->Append( eNewCube,							cuT( "C&ube"));
	l_pSubMenu->Append( eNewCylinder,						cuT( "C&ylindre"));
	l_pSubMenu->Append( eNewIcosaedron,						cuT( "&Icosaedre"));
	l_pSubMenu->Append( eNewPlane,							cuT( "&Plan"));
	l_pSubMenu->Append( eNewSphere,							cuT( "&Sphere"));
	l_pSubMenu->Append( eNewTorus,							cuT( "&Torre"));
	l_pSubMenu->Append( eNewProjection,						cuT( "&Projection"));
	m_pNewMenu->AppendSubMenu( l_pSubMenu,					cuT( "Nouvelle &Geometrie\tCTRL+N+G"));
	m_pNewMenu->Append( eNewMaterial,						cuT( "&Material\tCTRL+N+M"));

	m_pSettingsMenu = new wxMenu();
	m_pSettingsMenu->AppendCheckItem( eSelect,				cuT( "&Sélectionner\tCTRL+E+S"));
	m_pSettingsMenu->AppendCheckItem( eModify,				cuT( "&Modifier\tCTRL+E+M"));
	m_pSettingsMenu->Append( eNone,							cuT( "&Annuler\tCTRL+E+C"));
	m_pSettingsMenu->AppendSeparator();
	m_pSettingsMenu->AppendRadioItem( eSelectGeometries,	cuT( "&Géométrie\tCTRL+E+G"))->Enable( false);
	m_pSettingsMenu->AppendRadioItem( eSelectPoints,		cuT( "&Point\tCTRL+E+P"))->Enable( false);
	m_pSettingsMenu->AppendSeparator();
	m_pSettingsMenu->Append( eCloneSelection,				cuT( "&Dupliquer\tCTRL+E+D"));
	l_pSubMenu = new wxMenu();
	l_pSubMenu->Append( eSubdividePNTriangles,				cuT( "PN &Triangles\tCTRL+E+S+T"));
	l_pSubMenu->Append( eSubdivideLoop,						cuT( "&Loop\tCTRL+E+S+L"));
	m_pSettingsMenu->AppendSubMenu( l_pSubMenu,				cuT( "&Subdiviser"));
	m_pSettingsMenu->Append( eSelectNone,					cuT( "&Aucune\tCTRL+E+A"));

	l_pToolbar->AddTool( eFile,			wxT( "Fichier"),	ImagesLoader::AddBitmap( eBmpFichier, fichier_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Menu fichier"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddTool( eNew,			wxT( "Nouveau"),	ImagesLoader::AddBitmap( eBmpAjouter, ajouter_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Nouvel élément"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddTool( eSettings,		wxT( "Paramètres"),	ImagesLoader::AddBitmap( eBmpParametres, parametres_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Menu Paramètres"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eGeometries,	wxT( "Géométries"),	ImagesLoader::AddBitmap( eBmpGeometries, geo_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Afficher les géométries"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddTool( eMaterials,	wxT( "Matériaux"),	ImagesLoader::AddBitmap( eBmpMaterials, mat_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Afficher les matériaux"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddSeparator();

	l_pToolbar->Realize();
}

void MainFrame :: _createGeometry( eMESH_TYPE p_meshType, String p_name,
									 const String & p_meshStrVars,
									 const String & p_baseName, ScenePtr p_scene,
									 NewGeometryDialog * p_dialog, unsigned int i,
									 unsigned int j, real a, real b, real c)
{
	if (p_name.empty() || p_name == cuT( "Geometry Name"))
	{
		xchar l_buffer[256];
		Sprintf( l_buffer, 255, cuT( "%s%d"), p_baseName.c_str(), g_nbGeometries);
		p_name = l_buffer;
	}

	SceneNodePtr l_sceneNode = p_scene->CreateSceneNode( cuT( "SN_") + p_name);

	if (l_sceneNode)
	{
		UIntArray l_faces;
		FloatArray l_dimensions;
		l_faces.push_back( i);
		l_faces.push_back( j);
		l_dimensions.push_back( a);
		l_dimensions.push_back( b);
		l_dimensions.push_back( c);
		GeometryPtr l_geometry = p_scene->CreatePrimitive( p_name, p_meshType, p_baseName + cuT( "_") + p_meshStrVars, l_faces, l_dimensions);

		if (l_geometry)
		{
			String l_materialName = p_dialog->GetMaterialName();
			MeshPtr l_mesh = l_geometry->GetMesh();
			Collection<Material, String> l_mtlCollection;

			for (size_t i = 0 ; i < l_mesh->GetNbSubmeshes() ; i++)
			{
				l_mesh->GetSubmesh( i)->SetMaterial( l_mtlCollection.GetElement( l_materialName));
			}

			l_sceneNode->AttachObject( l_geometry.get());
			l_sceneNode->SetVisible( true);
			g_nbGeometries++;
		}
	}
}

void MainFrame :: _setSelectionType( SelectionType p_eType)
{
	m_3dFrame->SetSelectionType( p_eType);

	if (m_bMultiFrames)
	{
		m_2dFrameHD->SetSelectionType( p_eType);
		m_2dFrameBG->SetSelectionType( p_eType);
		m_2dFrameBD->SetSelectionType( p_eType);
	}
}
void MainFrame :: _setActionType( ActionType p_eType)
{
	m_3dFrame->SetActionType( p_eType);

	if (m_bMultiFrames)
	{
		m_2dFrameHD->SetActionType( p_eType);
		m_2dFrameBG->SetActionType( p_eType);
		m_2dFrameBD->SetActionType( p_eType);
	}
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_TIMER(	1,			MainFrame::_onTimer)
	EVT_SIZE(				MainFrame::_onSize)
	EVT_MOVE(				MainFrame::_onMove)
	EVT_CLOSE(				MainFrame::_onClose)
	EVT_ENTER_WINDOW(		MainFrame::_onEnterWindow)
	EVT_LEAVE_WINDOW(		MainFrame::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	MainFrame::_onEraseBackground)

    EVT_MENU( wxID_EXIT,				MainFrame::_onMenuClose)
	EVT_MENU( eExit,					MainFrame::_onMenuClose)
	EVT_MENU( eSaveScene,				MainFrame::_onSaveScene)
	EVT_MENU( eLoadScene,				MainFrame::_onLoadScene)
	EVT_MENU( eNewCone,					MainFrame::_onNewCone)
	EVT_MENU( eNewCube,					MainFrame::_onNewCube)
	EVT_MENU( eNewCylinder,				MainFrame::_onNewCylinder)
	EVT_MENU( eNewIcosaedron,			MainFrame::_onNewIcosaedron)
	EVT_MENU( eNewPlane,				MainFrame::_onNewPlane)
	EVT_MENU( eNewSphere,				MainFrame::_onNewSphere)
	EVT_MENU( eNewTorus,				MainFrame::_onNewTorus)
	EVT_MENU( eNewProjection,			MainFrame::_onNewProjection)
	EVT_MENU( eNewMaterial,				MainFrame::_onNewMaterial)
	EVT_MENU( eGeometries,				MainFrame::_onShowGeometriesList)
	EVT_MENU( eMaterials,				MainFrame::_onShowMaterialsList)
	EVT_MENU( eSelectGeometries,		MainFrame::_onSelectGeometries)
	EVT_MENU( eSelectPoints,			MainFrame::_onSelectPoints)
	EVT_MENU( eCloneSelection,			MainFrame::_onCloneSelection)
	EVT_MENU( eSelectNone,				MainFrame::_onSelectNone)
	EVT_MENU( eSubdividePNTriangles,	MainFrame::_onSubdivideAllPNTriangles)
	EVT_MENU( eSubdivideLoop,			MainFrame::_onSubdivideAllLoop)
	EVT_MENU( eSelect,					MainFrame::_onSelect)
	EVT_MENU( eModify,					MainFrame::_onModify)
	EVT_MENU( eNone,					MainFrame::_onNothing)
	EVT_MENU( eRender,					MainFrame::_onRender)
	EVT_TOOL( eFile,					MainFrame::_onFileMenu)
	EVT_TOOL( eNew,						MainFrame::_onNewMenu)
	EVT_TOOL( eSettings,				MainFrame::_onSettingsMenu)

	EVT_KEY_DOWN(						MainFrame::_onKeyDown)
	EVT_KEY_UP(							MainFrame::_onKeyUp)
	EVT_LEFT_DOWN(						MainFrame::_onMouseLDown)
	EVT_LEFT_UP(						MainFrame::_onMouseLUp)
	EVT_MIDDLE_DOWN(					MainFrame::_onMouseMDown)
	EVT_MIDDLE_UP(						MainFrame::_onMouseMUp)
	EVT_RIGHT_DOWN(						MainFrame::_onMouseRDown)
	EVT_RIGHT_UP(						MainFrame::_onMouseRUp)
	EVT_MOTION(							MainFrame::_onMouseMove)
	EVT_MOUSEWHEEL(						MainFrame::_onMouseWheel)
END_EVENT_TABLE()

void MainFrame :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC l_dc( this);
	m_castor3D.RenderOneFrame();
}

void MainFrame :: _onTimer( wxTimerEvent & WXUNUSED(event))
{
	m_castor3D.RenderOneFrame();
}

void MainFrame :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);

	int l_width = GetClientSize().x / 2;
	int l_height = GetClientSize().y / 2;

	if (m_3dFrame)
	{
		if ( ! m_bMultiFrames)
		{
			m_3dFrame->SetSize( GetClientSize());
			m_3dFrame->SetPosition( wxPoint( 0, 0));
		}
		else
		{
			m_3dFrame->SetSize( l_width - 1, l_height - 1);
			m_3dFrame->SetPosition( wxPoint( 0, 0));
			m_2dFrameHD->SetSize( l_width - 1, l_height - 1);
			m_2dFrameHD->SetPosition( wxPoint( l_width + 1, 0));
			m_2dFrameBG->SetSize( l_width - 1, l_height - 1);
			m_2dFrameBG->SetPosition( wxPoint( 0, l_height + 1));
			m_2dFrameBD->SetSize( l_width - 1, l_height - 1);
			m_2dFrameBD->SetPosition( wxPoint( l_width + 1, l_height + 1));

			m_separator1->SetSize( l_width, 2);
			m_separator1->SetPosition( wxPoint( 0, l_height - 1));
			m_separator2->SetSize( l_width, 2);
			m_separator2->SetPosition( wxPoint( l_width, l_height - 1));
			m_separator3->SetSize( 2, l_height);
			m_separator3->SetPosition( wxPoint( l_width - 1, 0));
			m_separator4->SetSize( 2, l_height);
			m_separator4->SetPosition( wxPoint( l_width - 1, l_height));
		}
	}

	ShowPanels();
}

void MainFrame :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
	ShowPanels();
}

void MainFrame :: _onClose( wxCloseEvent & event)
{
	delete m_pImagesLoader;
	m_arraySubdividersDummy.clear();
	m_selectedGeometry.reset();
	m_repereX.reset();
	m_repereY.reset();
	m_repereZ.reset();
	m_mainScene.reset();

	Hide();

	if (m_timer)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = nullptr;
	}

	vector::deleteAll( m_selectedGeometryMaterials);

	if (m_mainScene)
	{
		m_mainScene.reset();
	}

	m_castor3D.EndRendering();

	if (m_3dFrame)
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true);
		m_3dFrame = nullptr;

		if (m_bMultiFrames)
		{
			m_2dFrameHD->UnFocus();
			m_2dFrameHD->Close( true);
			m_2dFrameHD = nullptr;

			m_2dFrameBG->UnFocus();
			m_2dFrameBG->Close( true);
			m_2dFrameBG = nullptr;

			m_2dFrameBD->UnFocus();
			m_2dFrameBD->Close( true);
			m_2dFrameBD = nullptr;
		}
	}

	DestroyChildren();
	m_castor3D.Clear();
	Destroy();
}

void MainFrame :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
    SetFocus();
}

void MainFrame :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
}

void MainFrame :: _onEraseBackground(wxEraseEvent& event)
{
}

void MainFrame :: _onKeyDown( wxKeyEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onKeyDown( event);
	}
}

void MainFrame :: _onKeyUp( wxKeyEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onKeyUp( event);
	}
}

void MainFrame :: _onMouseLDown( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseLDown( event);
	}
}

void MainFrame :: _onMouseLUp( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseLUp( event);
	}
}

void MainFrame :: _onMouseMDown( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseMDown( event);
	}
}

void MainFrame :: _onMouseMUp( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseMUp( event);
	}
}

void MainFrame :: _onMouseRDown( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseRDown( event);
	}
}

void MainFrame :: _onMouseRUp( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseRUp( event);
	}
}

void MainFrame :: _onMouseMove( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseMove( event);
	}
}

void MainFrame :: _onMouseWheel( wxMouseEvent & event)
{
	if (m_selectedFrame)
	{
		m_selectedFrame->_onMouseWheel( event);
	}
}

void MainFrame :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}

void MainFrame :: _onSaveScene( wxCommandEvent & event)
{
	wxFileDialog * l_fileDialog = new wxFileDialog( this, cuT( "Enregistrer une scene"), wxEmptyString, wxEmptyString, cuT( "Castor Shape files (*.cscn)|*.cscn"));

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		File l_file( (const char *)l_fileDialog->GetPath().c_str(), File::eWrite);
		Path l_filePath = (const char *)l_fileDialog->GetPath().c_str();
		Collection<Scene, String> l_scnManager;

		if (Root::GetSingleton()->WriteMaterials( l_file))
		{
			Logger::LogMessage( cuT( "Materials written"));
		}
		else
		{
			Logger::LogMessage( cuT( "Can't write materials"));
			return;
		}

		if (Root::GetSingleton()->SaveMeshes( l_file))
		{
			Logger::LogMessage( cuT( "Meshes written"));
		}
		else
		{
			Logger::LogMessage( cuT( "Can't write meshes"));
			return;
		}
		if (Loader<Scene>::Write( * l_scnManager.GetElement( cuT( "MainScene")), l_file))
		{
			Logger::LogMessage( cuT( "Save Successfull"));
		}
		else
		{
			Logger::LogMessage( cuT( "Save Failed"));
		}
	}
}

void MainFrame :: _onLoadScene( wxCommandEvent & event)
{
	wxString l_wildcard = wxT( "Castor3D scene files (*.cscn)|*.cscn");

	for (PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePluginImporter) ; l_it != m_castor3D.PluginsEnd( ePluginImporter) ; ++l_it)
	{
		ImporterPluginPtr l_pPlugin = static_pointer_cast< ImporterPlugin >( l_it->second);
		l_wildcard += wxT( "|") + l_pPlugin->GetExtension().upper_case() + wxT( " files (*.") + l_pPlugin->GetExtension().lower_case() + wxT( ")|*.") + l_pPlugin->GetExtension().lower_case();
	}

	wxFileDialog * l_fileDialog = new wxFileDialog( this, cuT( "Ouvrir une scene"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		ImporterPluginPtr l_pPlugin;
		Importer * l_pImporter;
		Path l_file = (const char *)l_fileDialog->GetPath().c_str();
		Collection<Material, String> l_mtlCollection;
		Collection<Mesh, String> l_mshCollection;

		if (l_file.GetExtension() == cuT( "CSCN"))
		{
			m_mainScene->ClearScene();
			Logger::LogMessage( cuT( "Scene cleared"));
			l_mshCollection.Clear();
			Logger::LogMessage( cuT( "Mesh manager cleared"));
			l_mtlCollection.Clear();
			Logger::LogMessage( cuT( "Material manager cleared"));
			Logger::LogMessage( (const char *)l_fileDialog->GetPath().c_str());

			Path l_filePath = (const char *)l_fileDialog->GetPath().c_str();

			Path l_matFilePath = l_filePath;
			l_matFilePath.replace( cuT( "cscn"), cuT( "cmtl"));
			File l_file( l_matFilePath, File::eRead);

			if (Root::GetSingleton()->ReadMaterials( l_file))
			{
				Logger::LogMessage( cuT( "Materials read"));
			}
			else
			{
				Logger::LogMessage( cuT( "Can't read materials"));
				return;
			}

			SceneFileParser l_parser;
			l_parser.ParseFile( l_filePath.c_str());
		}
		else
		{
			for (PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePluginImporter) ; l_it != m_castor3D.PluginsEnd( ePluginImporter) ; ++l_it)
			{
				l_pPlugin = static_pointer_cast<ImporterPlugin, PluginBase>( l_it->second);

				if (l_file.GetExtension().lower_case() == l_pPlugin->GetExtension().lower_case())
				{
					l_pImporter = l_pPlugin->CreateImporter();
					m_mainScene->ImportExternal( l_file, l_pImporter);
				}
			}
		}
	}

	ShowPanels();
}

void MainFrame :: _onNewCone( wxCommandEvent & event)
{
	NewConeDialog * l_dialog = new NewConeDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_radius = l_dialog->GetConeRadius();
			real l_height = l_dialog->GetConeHeight();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_height != 0.0  && l_nbFaces >= 1)
			{
				_createGeometry( eCone, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 String( cuT( "Cone")), m_mainScene, static_cast<NewGeometryDialog *>( l_dialog), (unsigned int)l_nbFaces, (unsigned int)0, l_height, l_radius, real( 0));
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewCube( wxCommandEvent & event)
{
	NewCubeDialog * l_dialog = new NewCubeDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_width = l_dialog->GetCubeWidth();
			real l_height = l_dialog->GetCubeHeight();
			real l_depth = l_dialog->GetCubeDepth();

			if (l_width != 0.0 && l_height != 0.0 && l_depth != 0.0)
			{
				_createGeometry( eCube, l_dialog->GetGeometryName(), cuEmptyString,
								 cuT( "Cube"), m_mainScene, l_dialog, 0, 0, l_width, l_height, l_depth);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewCylinder( wxCommandEvent & event)
{
	NewCylinderDialog * l_dialog = new NewCylinderDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_radius = l_dialog->GetCylinderRadius();
			real l_height = l_dialog->GetCylinderHeight();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_height != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( eCylinder, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 cuT( "Cylinder"), m_mainScene, l_dialog, l_nbFaces, 0, l_height, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewIcosaedron( wxCommandEvent & event)
{
	NewIcosaedronDialog * l_dialog = new NewIcosaedronDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_radius = l_dialog->GetIcosaedronRadius();
			int l_nbFaces = l_dialog->GetNbSubdiv();

			if (l_radius != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( eIcosaedron, l_dialog->GetGeometryName(), l_dialog->GetNbSubdivStr(),
								 cuT( "Icosaedron"), m_mainScene, l_dialog, l_nbFaces, 0, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewPlane( wxCommandEvent & event)
{
	NewPlaneDialog * l_dialog = new NewPlaneDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_width = l_dialog->GetGeometryWidth();
			real l_depth = l_dialog->GetGeometryDepth();
			int l_nbWidthSubdiv = l_dialog->GetNbWidthSubdiv();
			int l_nbDepthSubdiv = l_dialog->GetNbDepthSubdiv();

			if (l_width != 0.0 && l_depth != 0.0 && l_nbWidthSubdiv >= 0 && l_nbDepthSubdiv >= 0)
			{
				_createGeometry( ePlane, l_dialog->GetGeometryName(), l_dialog->GetNbDepthSubdivStr() + cuT( "x") + l_dialog->GetNbWidthSubdivStr(),
								 cuT( "Plane"), m_mainScene, l_dialog, l_nbDepthSubdiv, l_nbWidthSubdiv, l_width, l_depth);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewSphere( wxCommandEvent & event)
{
	NewSphereDialog * l_dialog = new NewSphereDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_radius = l_dialog->GetSphereRadius();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_radius != 0.0 && l_nbFaces >= 3)
			{
				_createGeometry( eSphere, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 cuT( "Sphere"), m_mainScene, l_dialog, l_nbFaces, 0, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewTorus( wxCommandEvent & event)
{
	NewTorusDialog * l_dialog = new NewTorusDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_width = l_dialog->GetInternalRadius();
			real l_radius = l_dialog->GetExternalRadius();
			int l_nbRadiusSubdiv = l_dialog->GetExternalNbFaces();
			int l_nbWidthSubdiv = l_dialog->GetInternalNbFaces();

			if (l_width != 0.0 && l_radius != 0.0 && l_nbRadiusSubdiv >= 1 && l_nbWidthSubdiv >= 1)
			{
				_createGeometry( eTorus, l_dialog->GetGeometryName(), l_dialog->GetExternalNbFacesStr() + cuT( "x") + l_dialog->GetInternalNbFacesStr(),
								 cuT( "Torus"), m_mainScene, l_dialog, l_nbWidthSubdiv, l_nbRadiusSubdiv, l_width, l_radius);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewProjection( wxCommandEvent & event)
{
	NewSphereDialog * l_dialog = new NewSphereDialog( this, wxID_ANY);
	l_dialog->Initialise();

	if (l_dialog->ShowModal() == wxID_OK)
	{
		if (m_mainScene)
		{
			real l_fDepth = l_dialog->GetSphereRadius();
			int l_nbFaces = l_dialog->GetFacesNumber();

			if (l_fDepth != 0.0 && l_nbFaces >= 1)
			{
				_createGeometry( eProjection, l_dialog->GetGeometryName(), l_dialog->GetFacesNumberStr(),
								 cuT( "Projection"), m_mainScene, l_dialog, l_nbFaces, 0, l_fDepth, 0.0);
			}
		}

		ShowPanels();
	}

	l_dialog->Destroy();
}

void MainFrame :: _onNewMaterial( wxCommandEvent & event)
{
	NewMaterialDialog * l_dialog = new NewMaterialDialog( this, wxID_ANY);

	if (l_dialog->ShowModal() == wxID_OK)
	{
		Logger::LogMessage( cuT( "Material Created"));
	}

	l_dialog->Destroy();
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	wxGeometriesListFrame * l_listFrame = new wxGeometriesListFrame( this, cuT( "Geometries"), m_mainScene, wxDefaultPosition, wxSize( 200, 300));
	l_listFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	CastorShape::wxMaterialsFrame * l_listFrame = new CastorShape::wxMaterialsFrame( this, cuT( "Materiaux"), wxDefaultPosition);
	l_listFrame->Show();
}

void MainFrame :: _onSelectGeometries( wxCommandEvent & event)
{
	SelectGeometry( GeometryPtr());
	_setSelectionType( stGeometry);
}

void MainFrame :: _onSelectPoints( wxCommandEvent & event)
{
	SelectGeometry( GeometryPtr());
	_setSelectionType( stVertex);
}

void MainFrame :: _onCloneSelection( wxCommandEvent & event)
{
	_setSelectionType( stClone);
}

void MainFrame :: _onSelectNone( wxCommandEvent & event)
{
	SelectGeometry( GeometryPtr());
	_setSelectionType( stNone);
}

void MainFrame :: _onSelectAll( wxCommandEvent & event)
{
	_setSelectionType( stAll);
}

void MainFrame :: _onSelect( wxCommandEvent & event)
{
	m_pSettingsMenu->FindItem( eModify)->Check( false);

	if (event.IsChecked())
	{
		m_pSettingsMenu->FindItem( eSelectGeometries)->Enable( true);
		m_pSettingsMenu->FindItem( eSelectPoints)->Enable( true);

		_setActionType( atSelect);

		SelectGeometry( GeometryPtr());

		if (m_pSettingsMenu->FindItem( eSelectGeometries)->IsChecked())
		{
			_setSelectionType( stGeometry);
		}
		else
		{
			_setSelectionType( stVertex);
		}
	}
	else
	{
		_setActionType( atNone);
	}
}

void MainFrame :: _onModify( wxCommandEvent & event)
{
	m_pSettingsMenu->FindItem( eSelect)->Check( false);
	m_pSettingsMenu->FindItem( eSelectGeometries)->Enable( false);
	m_pSettingsMenu->FindItem( eSelectPoints)->Enable( false);

	if (event.IsChecked())
	{
		_setActionType( atModify);
	}
	else
	{
		_setActionType( atNone);
	}
}

void MainFrame :: _onNothing( wxCommandEvent & event)
{
	m_pSettingsMenu->FindItem( eSelect)->Check( false);
	m_pSettingsMenu->FindItem( eModify)->Check( false);
	m_pSettingsMenu->FindItem( eSelectGeometries)->Enable( false);
	m_pSettingsMenu->FindItem( eSelectPoints)->Enable( false);

	SelectGeometry( GeometryPtr());
	_setActionType( atNone);

}

void MainFrame :: _onSubdivideAllPNTriangles( wxCommandEvent & event)
{
	if (m_selectedGeometry)
	{
		size_t l_nbSubmeshes = m_selectedGeometry->GetMesh()->GetNbSubmeshes();

		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			SubdividerPtr l_pDivider( new PnTrianglesDivider( m_selectedGeometry->GetMesh()->GetSubmesh( i).get()));
			m_arraySubdividers.push_back( l_pDivider);
			l_pDivider->SetThreadEndFunction( & _endSubdivision, this);
			m_selectedGeometry->Subdivide( i, l_pDivider, true);
		}

		m_pSettingsMenu->FindItem( eSubdividePNTriangles)->Enable( false);
		m_pSettingsMenu->FindItem( eSubdivideLoop)->Enable( false);
	}
}

void MainFrame :: _onSubdivideAllLoop( wxCommandEvent & event)
{
	if (m_selectedGeometry)
	{
		size_t l_nbSubmeshes = m_selectedGeometry->GetMesh()->GetNbSubmeshes();
		DividerPluginPtr l_pPlugin = static_pointer_cast<DividerPlugin, PluginBase>( m_castor3D.LoadPlugin( "LoopDivider.dll", ""));

		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			SubdividerPtr l_pDivider( l_pPlugin->CreateDivider( m_selectedGeometry->GetMesh()->GetSubmesh( i).get()));
			m_arraySubdividers.push_back( l_pDivider);
			l_pDivider->SetThreadEndFunction( & _endSubdivision, this);
			m_selectedGeometry->Subdivide( i, l_pDivider, true);
		}

		m_pSettingsMenu->FindItem( eSubdividePNTriangles)->Enable( false);
		m_pSettingsMenu->FindItem( eSubdivideLoop)->Enable( false);
	}
}

void MainFrame :: _endSubdivision( void * p_pThis, Subdivider * p_pDivider)
{
	MainFrame * l_pThis = static_cast<MainFrame *>( p_pThis);
	bool l_bFound = false;
	size_t i = 0;
	SubdividerPtr l_pDivider;

	while (i < l_pThis->m_arraySubdividers.size() && ! l_bFound)
	{
		if (l_pThis->m_arraySubdividers[i].get() == p_pDivider)
		{
			l_bFound = true;
			l_pDivider = l_pThis->m_arraySubdividers[i];
			l_pThis->m_arraySubdividers.erase( l_pThis->m_arraySubdividers.begin() + i);
		}
	}

	if (l_pThis->m_arraySubdividers.size() == 0)
	{
		l_pThis->m_pSettingsMenu->FindItem( eSubdividePNTriangles)->Enable( true);
		l_pThis->m_pSettingsMenu->FindItem( eSubdivideLoop)->Enable( true);
	}

	l_pThis->m_arraySubdividersDummy.push_back( l_pDivider);
	l_pDivider->Cleanup();
}

void MainFrame :: _onRender( wxCommandEvent & event)
{
	RenderEngine renderEngine( cuT( "Scene.tga"), m_mainScene);
	renderEngine.Draw();
}

void MainFrame :: _onFileMenu( wxCommandEvent & p_event)
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eFile);

	if (l_pTool)
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse);
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pFileMenu, l_ptMouse.x, l_pToolbar->GetToolSize().GetHeight());
	}
	else
	{
		PopupMenu( m_pFileMenu);
	}

	p_event.Skip();
}

void MainFrame :: _onNewMenu( wxCommandEvent & p_event)
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eNew);

	if (l_pTool)
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse);
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pNewMenu, l_ptMouse.x, l_pToolbar->GetToolSize().GetHeight());
	}
	else
	{
		PopupMenu( m_pNewMenu);
	}

	p_event.Skip();
}

void MainFrame :: _onSettingsMenu( wxCommandEvent & p_event)
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eSettings);

	if (l_pTool)
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse);
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pSettingsMenu, l_ptMouse.x, l_pToolbar->GetToolSize().GetHeight());
	}
	else
	{
		PopupMenu( m_pSettingsMenu);
	}

	p_event.Skip();
}
