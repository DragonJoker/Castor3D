#include "MainFrame.hpp"
#include "CastorShape.hpp"
#include "RenderPanel.hpp"

#include "../geometry/NewConeDialog.hpp"
#include "../geometry/NewCubeDialog.hpp"
#include "../geometry/NewCylinderDialog.hpp"
#include "../geometry/NewIcosahedronDialog.hpp"
#include "../geometry/NewPlaneDialog.hpp"
#include "../geometry/NewSphereDialog.hpp"
#include "../geometry/NewTorusDialog.hpp"
#include "../material/MaterialsFrame.hpp"
#include "../material/NewMaterialDialog.hpp"
#include "../main/RenderEngine.hpp"

#include <xpms/geo_blanc.xpm>
#include <xpms/scene_blanc.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/castor_transparent.xpm>
#include <xpms/castor_dark.xpm>
#include <xpms/geo_visible.xpm>
#include <xpms/geo_visible_sel.xpm>
#include <xpms/geo_cachee.xpm>
#include <xpms/geo_cachee_sel.xpm>
#include <xpms/dossier.xpm>
#include <xpms/dossier_sel.xpm>
#include <xpms/dossier_ouv.xpm>
#include <xpms/dossier_ouv_sel.xpm>
#include <xpms/submesh.xpm>
#include <xpms/submesh_sel.xpm>
#include <xpms/fichier.xpm>
#include <xpms/ajouter.xpm>
#include <xpms/parametres.xpm>

#include <ImporterPlugin.hpp>
#include <DividerPlugin.hpp>
#include <MaterialManager.hpp>

#define ID_NEW_WINDOW 10000

using namespace Castor3D;
using namespace Castor;
using namespace CastorShape;
using namespace GuiCommon;

static const int CASTOR_WANTED_FPS	= 35;

DECLARE_APP( CastorShapeApp )

MainFrame::MainFrame( wxWindow * parent, wxString const & p_strTitle, eRENDERER_TYPE p_eRenderer )
	:	wxFrame( parent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 ) )
	,	m_castor3D()
	,	m_3dFrame( NULL )
	,	m_2dFrameHD( NULL )
	,	m_2dFrameBG( NULL )
	,	m_2dFrameBD( NULL )
	,	m_selectedFrame( NULL )
	,	m_bWireFrame( false )
	,	m_bMultiFrames( false )
	,	m_pImagesLoader( new wxImagesLoader )
	,	m_timer( NULL )
	,	m_iNbGeometries( 0 )
	,	m_eRenderer( p_eRenderer )
	,	m_pTimerInit( NULL )
{
	m_selectedMaterial.m_ambient[0] = 0.2f;
	m_selectedMaterial.m_ambient[1] = 0.0f;
	m_selectedMaterial.m_ambient[2] = 0.0f;
	m_selectedMaterial.m_emissive[0] = 0.2f;
	m_selectedMaterial.m_emissive[1] = 0.0f;
	m_selectedMaterial.m_emissive[2] = 0.0f;
}

MainFrame::~MainFrame()
{
}

bool MainFrame::Initialise()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	wxString l_strCopyright;
	l_strCopyright << wxDateTime().Now().GetCurrentYear();
	wxSplashScreen l_splashScreen( this, wxT( "Castor\nShape" ), l_strCopyright + cuT( " " ) + _( " DragonJoker, All rights shared" ), wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( l_rect.width - 512 ) / 2, ( l_rect.height - 384 ) / 2 ), 9 );
	m_pSplashScreen = &l_splashScreen;
	bool l_bReturn = DoInitialiseImages();

	if ( l_bReturn )
	{
		CreateStatusBar();
		_populateToolbar();
		wxIcon l_icon = wxIcon( castor_dark_xpm );
		SetIcon( l_icon );
		l_bReturn = DoInitialise3D();
	}

	l_splashScreen.Close();
	Show( l_bReturn );
	return l_bReturn;
}

void MainFrame::SelectGeometry( GeometrySPtr p_geometry )
{
	if ( p_geometry != m_selectedGeometry )
	{
		if ( m_selectedGeometry )
		{
			MeshSPtr l_mesh = m_selectedGeometry->GetMesh();
			uint32_t l_uiIndex = 0;
			std::for_each( l_mesh->Begin(), l_mesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				MaterialInfos * l_infos = m_selectedGeometryMaterials[l_uiIndex++];
				m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->SetAmbient( Colour::from_rgb( l_infos->m_ambient ) );
				m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->SetEmissive( Colour::from_rgb( l_infos->m_emissive ) );
			} );
			clear_container( m_selectedGeometryMaterials );
		}

		m_selectedGeometry = p_geometry;

		if ( m_selectedGeometry )
		{
			MeshSPtr l_mesh = m_selectedGeometry->GetMesh();
			std::for_each( l_mesh->Begin(), l_mesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				MaterialInfos * l_infos = new MaterialInfos;
				l_infos->m_ambient[0] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetAmbient().red();
				l_infos->m_ambient[1] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetAmbient().green();
				l_infos->m_ambient[2] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetAmbient().blue();
				l_infos->m_emissive[0] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetEmissive().red();
				l_infos->m_emissive[1] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetEmissive().green();
				l_infos->m_emissive[2] = m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->GetEmissive().blue();
				m_selectedGeometryMaterials.push_back( l_infos );
				m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->SetAmbient( Colour::from_rgb( m_selectedMaterial.m_ambient ) );
				m_selectedGeometry->GetMaterial( p_pSubmesh )->GetPass( 0 )->SetEmissive( Colour::from_rgb( m_selectedMaterial.m_emissive ) );
			} );
		}
	}
}

void MainFrame::SelectVertex( Vertex * CU_PARAM_UNUSED( p_vertex ) )
{
}

void MainFrame::ShowPanels()
{
	if ( m_3dFrame )
	{
		m_3dFrame->DrawOneFrame();

		if ( m_bMultiFrames )
		{
			m_2dFrameHD->DrawOneFrame();
			m_2dFrameBG->DrawOneFrame();
			m_2dFrameBD->DrawOneFrame();
		}
	}
}

void MainFrame::SetCurrentPanel( RenderPanel * p_pCheck, RenderPanel * p_pValue )
{
	if ( p_pValue == NULL )
	{
		if ( m_selectedFrame == p_pCheck )
		{
			m_selectedFrame = NULL;
		}
	}
	else
	{
		m_selectedFrame = p_pValue;
	}
}

void MainFrame::LoadScene( wxString const & p_strFileName )
{
	if ( ! p_strFileName.empty() )
	{
		m_strFilePath = ( wxChar const * )p_strFileName.c_str();
	}

	if ( ! m_strFilePath.empty() )
	{
		MaterialManager & l_mtlManager = m_castor3D.GetMaterialManager();
		Collection<Mesh, String> l_mshCollection;
		Collection<Scene, String> l_scnCollection;
//		str_utils::to_lower_case( m_strFilePath );
		m_mainScene->ClearScene();
		Logger::LogInfo( cuT( "Scene cleared" ) );
		l_mshCollection.clear();
		Logger::LogInfo( cuT( "Mesh manager cleared" ) );
		l_mtlManager.clear();
		Logger::LogInfo( cuT( "Material manager cleared" ) );
		Logger::LogInfo( cuT( "Loading scene file : " ) + m_strFilePath );
		ImporterPluginSPtr l_pPlugin;
		ImporterSPtr l_pImporter;

		if ( str_utils::lower_case( m_strFilePath.GetExtension() ) == cuT( "cscn" ) )
		{
			Path l_matFilePath = m_strFilePath;
			str_utils::replace( l_matFilePath, cuT( "cscn" ), cuT( "cmtl" ) );

			if ( File::FileExists( l_matFilePath ) )
			{
				TextFile l_fileMat( l_matFilePath, File::eOPEN_MODE_READ );
				Logger::LogInfo( cuT( "Loading materials file : " ) + l_matFilePath );

				if ( l_mtlManager.Read( l_fileMat ) )
				{
					Logger::LogInfo( cuT( "Materials read" ) );
				}
				else
				{
					Logger::LogInfo( cuT( "Can't read materials" ) );
					return;
				}
			}

			Path l_meshFilePath = m_strFilePath;
			str_utils::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

			if ( File::FileExists( l_meshFilePath ) )
			{
				BinaryFile l_fileMesh( l_meshFilePath, File::eOPEN_MODE_READ );
				Logger::LogInfo( cuT( "Loading meshes file : " ) + l_meshFilePath );

				if ( m_castor3D.LoadMeshes( l_fileMesh ) )
				{
					Logger::LogInfo( cuT( "Meshes read" ) );
				}
				else
				{
					Logger::LogInfo( cuT( "Can't read meshes" ) );
					return;
				}
			}

			SceneFileParser l_parser( &m_castor3D );
			l_parser.ParseFile( m_strFilePath );
		}
		else
		{
			ImporterPlugin::ExtensionArray l_arrayExtensions;

			for ( PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != m_castor3D.PluginsEnd( ePLUGIN_TYPE_IMPORTER ) && !l_pImporter; ++l_it )
			{
				l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it->second );

				if ( l_pPlugin )
				{
					l_arrayExtensions = l_pPlugin->GetExtensions();

					for ( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExtensions.begin(); l_itExt != l_arrayExtensions.end() && !l_pImporter; ++l_itExt )
					{
						if ( str_utils::lower_case( m_strFilePath.GetExtension() ) == str_utils::lower_case( l_itExt->first ) )
						{
							l_pImporter = l_pPlugin->GetImporter();
						}
					}
				}
			}

			if ( l_pImporter )
			{
				m_mainScene->ImportExternal( m_strFilePath, *l_pImporter );
			}
		}
	}
}

bool MainFrame::DoInitialise3D()
{
	bool l_bReturn = true;
	m_pSplashScreen->Step( _( "Loading plugins" ), 1 );
	Logger::LogInfo( cuT( "Initialising Castor3D" ) );
	m_castor3D.Initialise( 1000 / CASTOR_WANTED_FPS );
	StringArray l_arrayFiles;
	File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );

	if ( l_arrayFiles.size() > 0 )
	{
		std::for_each( l_arrayFiles.begin(), l_arrayFiles.end(), [&]( String const & p_strFileName )
		{
			Path l_file = p_strFileName;

			if ( l_file.GetExtension() == CASTOR_DLL_EXT )
			{
				m_pSplashScreen->SubStatus( l_file.GetFileName() );

				try
				{
					m_castor3D.LoadPlugin( l_file );
				}
				catch ( bool )
				{
					// Exception caught, plugin isn't loaded so nothing to do
				}
			}
		} );
	}

	Logger::LogInfo( cuT( "Plugins loaded" ) );

	try
	{
		m_pSplashScreen->Step( _( "Initialising main window" ), 1 );

		if ( m_eRenderer == eRENDERER_TYPE_UNDEFINED )
		{
			wxRendererSelector m_dialog( &m_castor3D, this, wxT( "Castor Shape" ) );
			int l_iReturn = m_dialog.ShowModal();

			if ( l_iReturn == wxID_OK )
			{
				m_eRenderer = m_dialog.GetSelectedRenderer();
			}
			else
			{
				l_bReturn = false;
			}
		}
		else
		{
			l_bReturn = true;
		}

		if ( l_bReturn )
		{
			l_bReturn = m_castor3D.LoadRenderer( m_eRenderer );
		}

		if ( l_bReturn )
		{
			m_mainScene = m_castor3D.CreateScene( cuT( "MainScene" ) );
			m_mainScene->SetBackgroundColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
			Logger::LogInfo( cuT( "Castor3D Initialised" ) );
			int l_width = GetClientSize().x / ( m_bMultiFrames ? 2 : 1 );
			int l_height = GetClientSize().y / ( m_bMultiFrames ? 2 : 1 );
			m_3dFrame = new RenderPanel( this, wxID_ANY, eVIEWPORT_TYPE_3D, m_mainScene, wxPoint( 0, 0 ), wxSize( l_width - 1, l_height - 1 ) );

			if ( m_bMultiFrames )
			{
				m_2dFrameHD = new RenderPanel( this, wxID_ANY, eVIEWPORT_TYPE_2D, m_mainScene, wxPoint( l_width + 1, 0 ), wxSize( l_width - 1, l_height - 1 ) );
				m_2dFrameBG = new RenderPanel( this, wxID_ANY, eVIEWPORT_TYPE_2D, m_mainScene, wxPoint( 0, l_height + 1 ), wxSize( l_width - 1, l_height - 1 ), ePROJECTION_DIRECTION_LEFT );
				m_2dFrameBD = new RenderPanel( this, wxID_ANY, eVIEWPORT_TYPE_2D, m_mainScene, wxPoint( l_width + 1, l_height + 1 ), wxSize( l_width - 1, l_height - 1 ), ePROJECTION_DIRECTION_TOP );
				m_separator1 = new wxPanel( this, wxID_ANY, wxPoint( 0, 			l_height - 1	), wxSize( l_width,	2	) );
				m_separator2 = new wxPanel( this, wxID_ANY, wxPoint( l_width, 		l_height - 1	), wxSize( l_width,	2	) );
				m_separator3 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, 	0	), wxSize( 2,		l_height	) );
				m_separator4 = new wxPanel( this, wxID_ANY, wxPoint( l_width - 1, 	l_height	), wxSize( 2,		l_height	) );
			}

			if ( !m_pTimerInit )
			{
				m_pTimerInit = new wxTimer( this, eEVENT_INIT3D );
				m_pTimerInit->Start( 1000 );
			}
		}
	}
	catch ( Castor::Exception & p_exc )
	{
		wxMessageBox( str_utils::from_str( p_exc.GetDescription() ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
		l_bReturn = false;
	}
	catch ( ... )
	{
		wxMessageBox( _( "Problem occured during Castor3D initialisation" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
		l_bReturn = false;
	}

	return l_bReturn;
}

bool MainFrame::DoInitialiseImages()
{
	m_pSplashScreen->Step( _( "Loading images" ), 1 );
	wxImagesLoader::AddBitmap( CV_IMG_CASTOR,									castor_transparent_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE,				geo_visible_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE_SEL,			geo_visible_sel_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN,				geo_cachee_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN_SEL,			geo_cachee_sel_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY,			dossier_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_SEL,		dossier_sel_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN,		dossier_ouv_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN_SEL,	dossier_ouv_sel_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH,				submesh_xpm	);
	wxImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH_SEL,			submesh_sel_xpm	);
	wxImagesLoader::AddBitmap( eBMP_GEOMETRIES,									geo_blanc_xpm	);
	wxImagesLoader::AddBitmap( eBMP_MATERIALS,									mat_blanc_xpm	);
	wxImagesLoader::AddBitmap( eBMP_FICHIER,									fichier_xpm	);
	wxImagesLoader::AddBitmap( eBMP_AJOUTER,									ajouter_xpm	);
	wxImagesLoader::AddBitmap( eBMP_PARAMETRES,									parametres_xpm	);
	wxImagesLoader::WaitAsyncLoads();
	return true;
}

void MainFrame::_populateToolbar()
{
	m_pSplashScreen->Step( _( "Loading toolbar" ), 1 );
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL );
	l_pToolbar->SetBackgroundColour( * wxWHITE );
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32 ) );
	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append( eSaveScene,						_( "Save scene\tCTRL+F+S" ) );
	m_pFileMenu->Append( eLoadScene,						_( "Open scene\tCTRL+F+O" ) );
	m_pFileMenu->Append( eRender,							_( "Render scene\tCTRL+F+R" ) );
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append( eExit,								_( "Quit\tALT+F4" ) );
	m_pNewMenu = new wxMenu();
	wxMenu * l_pSubMenu = new wxMenu();
	l_pSubMenu->Append( eNewCone,							_( "Cone" ) );
	l_pSubMenu->Append( eNewCube,							_( "Cube" ) );
	l_pSubMenu->Append( eNewCylinder,						_( "Cylinder" ) );
	l_pSubMenu->Append( eNewIcosahedron,						_( "Icosahedron" ) );
	l_pSubMenu->Append( eNewPlane,							_( "Plane" ) );
	l_pSubMenu->Append( eNewSphere,							_( "Sphere" ) );
	l_pSubMenu->Append( eNewTorus,							_( "Torus" ) );
	l_pSubMenu->Append( eNewProjection,						_( "Projection" ) );
	m_pNewMenu->AppendSubMenu( l_pSubMenu,					_( "New Geometry\tCTRL+N+G" ) );
	m_pNewMenu->Append( eNewMaterial,						_( "Material\tCTRL+N+M" ) );
	m_pSettingsMenu = new wxMenu();
	m_pSettingsMenu->AppendCheckItem( eSelect,				_( "Select\tCTRL+E+S" ) );
	m_pSettingsMenu->AppendCheckItem( eModify,				_( "Modify\tCTRL+E+M" ) );
	m_pSettingsMenu->Append( eNone,							_( "Cancel\tCTRL+E+C" ) );
	m_pSettingsMenu->AppendSeparator();
	m_pSettingsMenu->AppendRadioItem( eSelectGeometries,	_( "Geometry\tCTRL+E+G" ) )->Enable( false );
	m_pSettingsMenu->AppendRadioItem( eSelectPoints,		_( "Point\tCTRL+E+P" ) )->Enable( false );
	m_pSettingsMenu->AppendSeparator();
	m_pSettingsMenu->Append( eCloneSelection,				_( "Clone\tCTRL+E+D" ) );
	l_pSubMenu = new wxMenu();
	l_pSubMenu->Append( eSubdividePNTriangles,				_( "PN Triangles\tCTRL+E+S+T" ) );
	l_pSubMenu->Append( eSubdivideLoop,						_( "Loop\tCTRL+E+S+L" ) );
	m_pSettingsMenu->AppendSubMenu( l_pSubMenu,				_( "Subdivide" ) );
	m_pSettingsMenu->Append( eSelectNone,					_( "No action\tCTRL+E+A" ) );
	l_pToolbar->AddTool( eFile,			_( "File"	),	wxImage( *wxImagesLoader::GetBitmap( eBMP_FICHIER	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ),	_( "File menu"	) );
	m_pSplashScreen->Step( 1 );
	l_pToolbar->AddTool( eNew,			_( "New"	),	wxImage( *wxImagesLoader::GetBitmap( eBMP_AJOUTER	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ),	_( "New element"	) );
	m_pSplashScreen->Step( 1 );
	l_pToolbar->AddTool( eSettings,		_( "Parameters"	),	wxImage( *wxImagesLoader::GetBitmap( eBMP_PARAMETRES	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ),	_( "Parameters menu"	) );
	m_pSplashScreen->Step( 1 );
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eGeometries,	_( "Geometries"	),	wxImage( *wxImagesLoader::GetBitmap( eBMP_GEOMETRIES	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ),	_( "Display geometries"	) );
	m_pSplashScreen->Step( 1 );
	l_pToolbar->AddTool( eMaterials,	_( "Materials"	),	wxImage( *wxImagesLoader::GetBitmap( eBMP_MATERIALS	) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ),	_( "Display materials"	) );
	m_pSplashScreen->Step( 1 );
	l_pToolbar->AddSeparator();
	l_pToolbar->Realize();
}

void MainFrame::DoCreateGeometry( eMESH_TYPE p_meshType, String p_name, String const & p_meshStrVars, wxString const & p_baseName, SceneSPtr p_scene, NewGeometryDialog * p_dialog, uint32_t i, uint32_t j, real a, real b, real c )
{
	if ( p_name.empty() || p_name == _( "Geometry Name" ) )
	{
		p_name = p_baseName;
		p_name << m_iNbGeometries;
	}

	SceneNodeSPtr l_sceneNode = p_scene->CreateSceneNode( cuT( "SN_" ) + p_name, p_scene->GetObjectRootNode() );

	if ( l_sceneNode )
	{
		UIntArray l_faces;
		RealArray l_dimensions;
		l_faces.push_back( i );
		l_faces.push_back( j );
		l_dimensions.push_back( a );
		l_dimensions.push_back( b );
		l_dimensions.push_back( c );
		GeometrySPtr l_geometry = p_scene->CreateGeometry( p_name, p_meshType, String( ( const wxChar * )p_baseName.c_str() ) + cuT( "_" ) + p_meshStrVars, l_faces, l_dimensions );

		if ( l_geometry )
		{
			l_sceneNode->AttachObject( l_geometry );
			String l_materialName = p_dialog->GetMaterialName();
			MeshSPtr l_mesh = l_geometry->GetMesh();
			std::for_each( l_mesh->Begin(), l_mesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				l_geometry->SetMaterial( p_pSubmesh, m_castor3D.GetMaterialManager().find( l_materialName ) );
			} );
			l_sceneNode->SetVisible( true );
			m_iNbGeometries++;
		}
	}
}

void MainFrame::_setSelectionType( SelectionType p_eType )
{
	m_3dFrame->SetSelectionType( p_eType );

	if ( m_bMultiFrames )
	{
		m_2dFrameHD->SetSelectionType( p_eType );
		m_2dFrameBG->SetSelectionType( p_eType );
		m_2dFrameBD->SetSelectionType( p_eType );
	}
}
void MainFrame::_setActionType( ActionType p_eType )
{
	m_3dFrame->SetActionType( p_eType );

	if ( m_bMultiFrames )
	{
		m_2dFrameHD->SetActionType( p_eType );
		m_2dFrameBG->SetActionType( p_eType );
		m_2dFrameBD->SetActionType( p_eType );
	}
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
	EVT_TIMER(	1,						MainFrame::_onTimer )
	EVT_SIZE(	MainFrame::_onSize )
	EVT_MOVE(	MainFrame::_onMove )
	EVT_CLOSE(	MainFrame::_onClose )
	EVT_ENTER_WINDOW(	MainFrame::_onEnterWindow )
	EVT_LEAVE_WINDOW(	MainFrame::_onLeaveWindow )
	EVT_ERASE_BACKGROUND(	MainFrame::_onEraseBackground )

	EVT_MENU(	wxID_EXIT,				MainFrame::_onMenuClose )
	EVT_MENU(	eExit,					MainFrame::_onMenuClose )
	EVT_MENU(	eSaveScene,				MainFrame::_onSaveScene )
	EVT_MENU(	eLoadScene,				MainFrame::_onLoadScene )
	EVT_MENU(	eNewCone,				MainFrame::_onNewCone )
	EVT_MENU(	eNewCube,				MainFrame::_onNewCube )
	EVT_MENU(	eNewCylinder,			MainFrame::_onNewCylinder )
	EVT_MENU(	eNewIcosahedron,			MainFrame::_onNewIcosahedron )
	EVT_MENU(	eNewPlane,				MainFrame::_onNewPlane )
	EVT_MENU(	eNewSphere,				MainFrame::_onNewSphere )
	EVT_MENU(	eNewTorus,				MainFrame::_onNewTorus )
	EVT_MENU(	eNewProjection,			MainFrame::_onNewProjection )
	EVT_MENU(	eNewMaterial,			MainFrame::_onNewMaterial )
	EVT_MENU(	eGeometries,			MainFrame::_onShowGeometriesList )
	EVT_MENU(	eMaterials,				MainFrame::_onShowMaterialsList )
	EVT_MENU(	eSelectGeometries,		MainFrame::_onSelectGeometries )
	EVT_MENU(	eSelectPoints,			MainFrame::_onSelectPoints )
	EVT_MENU(	eCloneSelection,		MainFrame::_onCloneSelection )
	EVT_MENU(	eSelectNone,			MainFrame::_onSelectNone )
	EVT_MENU(	eSubdividePNTriangles,	MainFrame::_onSubdivideAllPNTriangles )
	EVT_MENU(	eSubdivideLoop,			MainFrame::_onSubdivideAllLoop )
	EVT_MENU(	eSelect,				MainFrame::_onSelect )
	EVT_MENU(	eModify,				MainFrame::_onModify )
	EVT_MENU(	eNone,					MainFrame::_onNothing )
	EVT_MENU(	eRender,				MainFrame::_onRender )
	EVT_TOOL(	eFile,					MainFrame::_onFileMenu )
	EVT_TOOL(	eNew,					MainFrame::_onNewMenu )
	EVT_TOOL(	eSettings,				MainFrame::_onSettingsMenu )

	EVT_KEY_DOWN(	MainFrame::_onKeyDown )
	EVT_KEY_UP(	MainFrame::_onKeyUp )
	EVT_LEFT_DOWN(	MainFrame::_onMouseLDown )
	EVT_LEFT_UP(	MainFrame::_onMouseLUp )
	EVT_MIDDLE_DOWN(	MainFrame::_onMouseMDown )
	EVT_MIDDLE_UP(	MainFrame::_onMouseMUp )
	EVT_RIGHT_DOWN(	MainFrame::_onMouseRDown )
	EVT_RIGHT_UP(	MainFrame::_onMouseRUp )
	EVT_MOTION(	MainFrame::_onMouseMove )
	EVT_MOUSEWHEEL(	MainFrame::_onMouseWheel )

	EVT_TIMER(	eEVENT_INIT3D,			MainFrame::OnInit3D	)
END_EVENT_TABLE()

void MainFrame::_onPaint( wxPaintEvent & WXUNUSED( p_event ) )
{
	wxPaintDC l_dc( this );
	m_castor3D.RenderOneFrame();
}

void MainFrame::_onTimer( wxTimerEvent & WXUNUSED( p_event ) )
{
	m_castor3D.RenderOneFrame();
}

void MainFrame::_onSize( wxSizeEvent & WXUNUSED( p_event ) )
{
	wxClientDC l_dc( this );
	int l_width = GetClientSize().x / 2;
	int l_height = GetClientSize().y / 2;

	if ( m_3dFrame )
	{
		if ( ! m_bMultiFrames )
		{
			m_3dFrame->SetSize( GetClientSize() );
			m_3dFrame->SetPosition( wxPoint( 0, 0 ) );
		}
		else
		{
			m_3dFrame->SetSize( l_width - 1, l_height - 1 );
			m_3dFrame->SetPosition( wxPoint( 0, 0 ) );
			m_2dFrameHD->SetSize( l_width - 1, l_height - 1 );
			m_2dFrameHD->SetPosition( wxPoint( l_width + 1, 0 ) );
			m_2dFrameBG->SetSize( l_width - 1, l_height - 1 );
			m_2dFrameBG->SetPosition( wxPoint( 0, l_height + 1 ) );
			m_2dFrameBD->SetSize( l_width - 1, l_height - 1 );
			m_2dFrameBD->SetPosition( wxPoint( l_width + 1, l_height + 1 ) );
			m_separator1->SetSize( l_width, 2 );
			m_separator1->SetPosition( wxPoint( 0, l_height - 1 ) );
			m_separator2->SetSize( l_width, 2 );
			m_separator2->SetPosition( wxPoint( l_width, l_height - 1 ) );
			m_separator3->SetSize( 2, l_height );
			m_separator3->SetPosition( wxPoint( l_width - 1, 0 ) );
			m_separator4->SetSize( 2, l_height );
			m_separator4->SetPosition( wxPoint( l_width - 1, l_height ) );
		}
	}

	ShowPanels();
}

void MainFrame::_onMove( wxMoveEvent & WXUNUSED( p_event ) )
{
	wxClientDC l_dc( this );
	ShowPanels();
}

void MainFrame::_onClose( wxCloseEvent & p_event )
{
	Hide();

	if ( m_timer )
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = nullptr;
	}

	delete m_pImagesLoader;
	m_arraySubdividersDummy.clear();
	m_selectedGeometry.reset();
	m_repereX.reset();
	m_repereY.reset();
	m_repereZ.reset();
	clear_container( m_selectedGeometryMaterials );

	if ( m_mainScene )
	{
		m_mainScene.reset();
	}

	/*
		if( m_3dFrame )
		{
			m_3dFrame->SetRenderWindow( nullptr );

			if (m_bMultiFrames)
			{
				m_2dFrameHD->SetRenderWindow( nullptr );
				m_2dFrameBG->SetRenderWindow( nullptr );
				m_2dFrameBD->SetRenderWindow( nullptr );
			}
		}
	*/
	m_castor3D.Cleanup();

	if ( m_3dFrame )
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true );
		m_3dFrame = nullptr;

		if ( m_bMultiFrames )
		{
			m_2dFrameHD->UnFocus();
			m_2dFrameHD->Close( true );
			m_2dFrameHD = nullptr;
			m_2dFrameBG->UnFocus();
			m_2dFrameBG->Close( true );
			m_2dFrameBG = nullptr;
			m_2dFrameBD->UnFocus();
			m_2dFrameBD->Close( true );
			m_2dFrameBD = nullptr;
		}
	}

	DestroyChildren();
	wxImagesLoader::Cleanup();
	p_event.Skip();
}

void MainFrame::_onEnterWindow( wxMouseEvent & WXUNUSED( p_event ) )
{
	SetFocus();
}

void MainFrame::_onLeaveWindow( wxMouseEvent & WXUNUSED( p_event ) )
{
}

void MainFrame::_onEraseBackground( wxEraseEvent & WXUNUSED( p_event ) )
{
}

void MainFrame::_onKeyDown( wxKeyEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onKeyDown( event );
	}
}

void MainFrame::_onKeyUp( wxKeyEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onKeyUp( event );
	}
}

void MainFrame::_onMouseLDown( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseLDown( event );
	}
}

void MainFrame::_onMouseLUp( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseLUp( event );
	}
}

void MainFrame::_onMouseMDown( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseMDown( event );
	}
}

void MainFrame::_onMouseMUp( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseMUp( event );
	}
}

void MainFrame::_onMouseRDown( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseRDown( event );
	}
}

void MainFrame::_onMouseRUp( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseRUp( event );
	}
}

void MainFrame::_onMouseMove( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseMove( event );
	}
}

void MainFrame::_onMouseWheel( wxMouseEvent & event )
{
	if ( m_selectedFrame )
	{
		m_selectedFrame->_onMouseWheel( event );
	}
}

void MainFrame::_onMenuClose( wxCommandEvent & WXUNUSED( p_event ) )
{
	Close( true );
}

void MainFrame::_onSaveScene( wxCommandEvent & WXUNUSED( p_event ) )
{
	wxFileDialog * l_fileDialog = new wxFileDialog( this, _( "Save scene" ), wxEmptyString, wxEmptyString, _( "Castor Shape files (*.cscn)|*.cscn" ) );

	if ( l_fileDialog->ShowModal() == wxID_OK )
	{
		BinaryFile l_file( ( char const * )l_fileDialog->GetPath().char_str(), File::eOPEN_MODE_WRITE );
		Path l_filePath = ( char const * )l_fileDialog->GetPath().c_str();
		Collection<Scene, String> l_scnManager;

		if ( m_castor3D.GetMaterialManager().Save( l_file ) )
		{
			Logger::LogInfo( cuT( "Materials written" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "Can't write materials" ) );
			return;
		}

		if ( m_castor3D.SaveMeshes( l_file ) )
		{
			Logger::LogInfo( cuT( "Meshes written" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "Can't write meshes" ) );
			return;
		}

		//if (Scene::BinaryLoader()( *l_scnManager.find( cuT( "MainScene")), l_file ) )
		//{
		//	Logger::LogInfo( cuT( "Save Successfull"));
		//}
		//else
		//{
		//	Logger::LogInfo( cuT( "Save Failed"));
		//}
	}
}

void MainFrame::_onLoadScene( wxCommandEvent & WXUNUSED( p_event ) )
{
	wxString l_wildcard = _( "Castor3D scene files (*.cscn)|*.cscn" );

	for ( PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != m_castor3D.PluginsEnd( ePLUGIN_TYPE_IMPORTER ); ++l_it )
	{
		ImporterPluginSPtr l_pPlugin = std::static_pointer_cast< ImporterPlugin >( l_it->second );
		ImporterPlugin::ExtensionArray l_arrayExt = l_pPlugin->GetExtensions();

		for ( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExt.begin(); l_itExt != l_arrayExt.end(); ++l_itExt )
		{
			std::string l_strName = str_utils::to_str( l_itExt->second );
			std::string l_strExt = str_utils::to_str( str_utils::lower_case( l_itExt->first ) );
			l_wildcard.Printf( cuT( "%s|%s (*.%s)|*.%s" ), ( char const * )l_wildcard.char_str(), l_strName.c_str(), l_strExt.c_str(), l_strExt.c_str() );
		}
	}

	wxFileDialog l_fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, l_wildcard );

	if ( l_fileDialog.ShowModal() == wxID_OK )
	{
		m_strFilePath = ( wxChar const * )l_fileDialog.GetPath().c_str();
		LoadScene( m_strFilePath );
	}

	ShowPanels();
}

void MainFrame::_onNewCone( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewConeDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_radius	= l_dialog.GetConeRadius();
			real l_height	= l_dialog.GetConeHeight();
			int l_nbFaces	= l_dialog.GetFacesNumber();

			if ( l_radius != 0.0 && l_height != 0.0  && l_nbFaces >= 1 )
			{
				DoCreateGeometry( eMESH_TYPE_CONE, l_dialog.GetGeometryName(), l_dialog.GetFacesNumberStr(), _( "Cone" ), m_mainScene, &l_dialog, l_nbFaces, 0, l_height, l_radius, 0 );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewCube( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewCubeDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_width	= l_dialog.GetCubeWidth();
			real l_height	= l_dialog.GetCubeHeight();
			real l_depth	= l_dialog.GetCubeDepth();

			if ( l_width != 0.0 && l_height != 0.0 && l_depth != 0.0 )
			{
				DoCreateGeometry( eMESH_TYPE_CUBE, l_dialog.GetGeometryName(), cuEmptyString, _( "Cube" ), m_mainScene, &l_dialog, 0, 0, l_width, l_height, l_depth );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewCylinder( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewCylinderDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_radius	= l_dialog.GetCylinderRadius();
			real l_height	= l_dialog.GetCylinderHeight();
			int l_nbFaces	= l_dialog.GetFacesNumber();

			if ( l_radius != 0.0 && l_height != 0.0 && l_nbFaces >= 1 )
			{
				DoCreateGeometry( eMESH_TYPE_CYLINDER, l_dialog.GetGeometryName(), l_dialog.GetFacesNumberStr(), _( "Cylinder" ), m_mainScene, &l_dialog, l_nbFaces, 0, l_height, l_radius );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewIcosahedron( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewIcosahedronDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_radius = l_dialog.GetIcosahedronRadius();
			int l_nbFaces = l_dialog.GetNbSubdiv();

			if ( l_radius != 0.0 && l_nbFaces >= 1 )
			{
				DoCreateGeometry( eMESH_TYPE_ICOSAHEDRON, l_dialog.GetGeometryName(), l_dialog.GetNbSubdivStr(), _( "Icosahedron" ), m_mainScene, &l_dialog, l_nbFaces, 0, l_radius );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewPlane( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewPlaneDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_width		= l_dialog.GetGeometryWidth();
			real l_depth		= l_dialog.GetGeometryDepth();
			int l_nbWidthSubdiv	= l_dialog.GetNbWidthSubdiv();
			int l_nbDepthSubdiv	= l_dialog.GetNbDepthSubdiv();

			if ( l_width != 0.0 && l_depth != 0.0 && l_nbWidthSubdiv >= 0 && l_nbDepthSubdiv >= 0 )
			{
				DoCreateGeometry( eMESH_TYPE_PLANE, l_dialog.GetGeometryName(), l_dialog.GetNbDepthSubdivStr() + cuT( "x" ) + l_dialog.GetNbWidthSubdivStr(), _( "Plane" ), m_mainScene, &l_dialog, l_nbDepthSubdiv, l_nbWidthSubdiv, l_width, l_depth );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewSphere( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewSphereDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_radius	= l_dialog.GetSphereRadius();
			int l_nbFaces	= l_dialog.GetFacesNumber();

			if ( l_radius != 0.0 && l_nbFaces >= 3 )
			{
				DoCreateGeometry( eMESH_TYPE_SPHERE, l_dialog.GetGeometryName(), l_dialog.GetFacesNumberStr(), _( "Sphere" ), m_mainScene, &l_dialog, l_nbFaces, 0, l_radius );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewTorus( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewTorusDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_width			= l_dialog.GetInternalRadius();
			real l_radius			= l_dialog.GetExternalRadius();
			int l_nbRadiusSubdiv	= l_dialog.GetExternalNbFaces();
			int l_nbWidthSubdiv		= l_dialog.GetInternalNbFaces();

			if ( l_width != 0.0 && l_radius != 0.0 && l_nbRadiusSubdiv >= 1 && l_nbWidthSubdiv >= 1 )
			{
				DoCreateGeometry( eMESH_TYPE_TORUS, l_dialog.GetGeometryName(), l_dialog.GetExternalNbFacesStr() + cuT( "x" ) + l_dialog.GetInternalNbFacesStr(), _( "Torus" ), m_mainScene, &l_dialog, l_nbWidthSubdiv, l_nbRadiusSubdiv, l_width, l_radius );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewProjection( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewSphereDialog l_dialog( &m_castor3D, this, wxID_ANY );
	l_dialog.Initialise();

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		if ( m_mainScene )
		{
			real l_fDepth	= l_dialog.GetSphereRadius();
			int l_nbFaces	= l_dialog.GetFacesNumber();

			if ( l_fDepth != 0.0 && l_nbFaces >= 1 )
			{
				DoCreateGeometry( eMESH_TYPE_PROJECTION, l_dialog.GetGeometryName(), l_dialog.GetFacesNumberStr(), _( "Projection" ), m_mainScene, &l_dialog, l_nbFaces, 0, l_fDepth, 0.0 );
			}
		}

		ShowPanels();
	}
}

void MainFrame::_onNewMaterial( wxCommandEvent & WXUNUSED( p_event ) )
{
	NewMaterialDialog l_dialog( &m_castor3D, this, wxID_ANY );

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		Logger::LogInfo( cuT( "Material Created" ) );
	}
}

void MainFrame::_onShowGeometriesList( wxCommandEvent & WXUNUSED( p_event ) )
{
	wxGeometriesListFrame * l_listFrame = new wxGeometriesListFrame( &m_castor3D, this, _( "Geometries" ), m_mainScene, wxDefaultPosition, wxSize( 200, 300 ) );
	l_listFrame->Show();
}

void MainFrame::_onShowMaterialsList( wxCommandEvent & WXUNUSED( p_event ) )
{
	CastorShape::wxMaterialsFrame * l_pFrame = new CastorShape::wxMaterialsFrame( &m_castor3D, this, _( "Materials" ), wxDefaultPosition );
	l_pFrame->Initialise();
	l_pFrame->Show();
}

void MainFrame::_onSelectGeometries( wxCommandEvent & WXUNUSED( p_event ) )
{
	SelectGeometry( GeometrySPtr() );
	_setSelectionType( stGeometry );
}

void MainFrame::_onSelectPoints( wxCommandEvent & WXUNUSED( p_event ) )
{
	SelectGeometry( GeometrySPtr() );
	_setSelectionType( stVertex );
}

void MainFrame::_onCloneSelection( wxCommandEvent & WXUNUSED( p_event ) )
{
	_setSelectionType( stClone );
}

void MainFrame::_onSelectNone( wxCommandEvent & WXUNUSED( p_event ) )
{
	SelectGeometry( GeometrySPtr() );
	_setSelectionType( stNone );
}

void MainFrame::_onSelectAll( wxCommandEvent & WXUNUSED( p_event ) )
{
	_setSelectionType( stAll );
}

void MainFrame::_onSelect( wxCommandEvent & p_event )
{
	m_pSettingsMenu->FindItem( eModify )->Check( false );

	if ( p_event.IsChecked() )
	{
		m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( true );
		m_pSettingsMenu->FindItem( eSelectPoints )->Enable( true );
		_setActionType( atSelect );
		SelectGeometry( GeometrySPtr() );

		if ( m_pSettingsMenu->FindItem( eSelectGeometries )->IsChecked() )
		{
			_setSelectionType( stGeometry );
		}
		else
		{
			_setSelectionType( stVertex );
		}
	}
	else
	{
		_setActionType( atNone );
	}
}

void MainFrame::_onModify( wxCommandEvent & p_event )
{
	m_pSettingsMenu->FindItem( eSelect )->Check( false );
	m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( false );
	m_pSettingsMenu->FindItem( eSelectPoints )->Enable( false );

	if ( p_event.IsChecked() )
	{
		_setActionType( atModify );
	}
	else
	{
		_setActionType( atNone );
	}
}

void MainFrame::_onNothing( wxCommandEvent & WXUNUSED( p_event ) )
{
	m_pSettingsMenu->FindItem( eSelect )->Check( false );
	m_pSettingsMenu->FindItem( eModify )->Check( false );
	m_pSettingsMenu->FindItem( eSelectGeometries )->Enable( false );
	m_pSettingsMenu->FindItem( eSelectPoints )->Enable( false );
	SelectGeometry( GeometrySPtr() );
	_setActionType( atNone );
}

void MainFrame::_onSubdivideAllPNTriangles( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( m_selectedGeometry )
	{
		Subdivider 	*	l_pDivider = NULL;
		DividerPluginSPtr	l_pPlugin;

		for ( PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePLUGIN_TYPE_DIVIDER ); l_it != m_castor3D.PluginsEnd( ePLUGIN_TYPE_DIVIDER ) && !l_pDivider; ++l_it )
		{
			l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it->second );

			if ( str_utils::lower_case( l_pPlugin->GetDividerType() ) == cuT( "pn_tri" ) )
			{
				l_pDivider = l_pPlugin->CreateDivider();
			}
		}

		if ( l_pDivider )
		{
			std::for_each( m_selectedGeometry->GetMesh()->Begin(), m_selectedGeometry->GetMesh()->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				l_pDivider->Subdivide( p_pSubmesh, 1, true );
			} );
			l_pPlugin->DestroyDivider( l_pDivider );
		}

		//m_arraySubdividers.push_back( l_pDivider );
		//l_pDivider->SetThreadEndFunction( &_endSubdivision, this );
		//m_selectedGeometry->Subdivide( i, l_pDivider, true );

		//m_pSettingsMenu->FindItem( eSubdividePNTriangles)->Enable( false);
		//m_pSettingsMenu->FindItem( eSubdivideLoop)->Enable( false);
	}
}

void MainFrame::_onSubdivideAllLoop( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( m_selectedGeometry )
	{
		Subdivider 	*	l_pDivider = NULL;
		DividerPluginSPtr	l_pPlugin;

		for ( PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePLUGIN_TYPE_DIVIDER ); l_it != m_castor3D.PluginsEnd( ePLUGIN_TYPE_DIVIDER ) && !l_pDivider; ++l_it )
		{
			l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it->second );

			if ( str_utils::lower_case( l_pPlugin->GetDividerType() ) == cuT( "pn_tri" ) )
			{
				l_pDivider = l_pPlugin->CreateDivider();
			}
		}

		if ( l_pDivider )
		{
			std::for_each( m_selectedGeometry->GetMesh()->Begin(), m_selectedGeometry->GetMesh()->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				l_pDivider->Subdivide( p_pSubmesh, 1, true );
			} );
			l_pPlugin->DestroyDivider( l_pDivider );
		}

		//m_arraySubdividers.push_back( l_pDivider );
		//l_pDivider->SetThreadEndFunction( &_endSubdivision, this );
		//m_selectedGeometry->Subdivide( i, l_pDivider, true );

		//m_pSettingsMenu->FindItem( eSubdividePNTriangles)->Enable( false);
		//m_pSettingsMenu->FindItem( eSubdivideLoop)->Enable( false);
	}
}

void MainFrame::_endSubdivision( void * p_pThis, Subdivider * p_pDivider )
{
	MainFrame * l_pThis = static_cast<MainFrame *>( p_pThis );
	bool l_bFound = false;
	uint32_t i = 0;
	SubdividerSPtr l_pDivider;

	while ( i < l_pThis->m_arraySubdividers.size() && ! l_bFound )
	{
		if ( l_pThis->m_arraySubdividers[i].get() == p_pDivider )
		{
			l_bFound = true;
			l_pDivider = l_pThis->m_arraySubdividers[i];
			l_pThis->m_arraySubdividers.erase( l_pThis->m_arraySubdividers.begin() + i );
		}

		i++;
	}

	if ( l_pThis->m_arraySubdividers.size() == 0 )
	{
		l_pThis->m_pSettingsMenu->FindItem( eSubdividePNTriangles )->Enable( true );
		l_pThis->m_pSettingsMenu->FindItem( eSubdivideLoop )->Enable( true );
	}

	l_pThis->m_arraySubdividersDummy.push_back( l_pDivider );
	l_pDivider->Cleanup();
}

void MainFrame::_onRender( wxCommandEvent & WXUNUSED( p_event ) )
{
	RenderEngine renderEngine( cuT( "Scene.tga" ), m_mainScene );
	renderEngine.Draw();
}

void MainFrame::_onFileMenu( wxCommandEvent & p_event )
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eFile );

	if ( l_pTool )
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse );
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pFileMenu, l_ptMouse.x, 0 );
	}
	else
	{
		PopupMenu( m_pFileMenu );
	}

	p_event.Skip();
}

void MainFrame::_onNewMenu( wxCommandEvent & p_event )
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eNew );

	if ( l_pTool )
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse );
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pNewMenu, l_ptMouse.x, 0 );
	}
	else
	{
		PopupMenu( m_pNewMenu );
	}

	p_event.Skip();
}

void MainFrame::_onSettingsMenu( wxCommandEvent & p_event )
{
	wxToolBar * l_pToolbar = GetToolBar();
	wxToolBarToolBase * l_pTool = l_pToolbar->FindById( eSettings );

	if ( l_pTool )
	{
		wxPoint l_ptMouse = wxGetMousePosition();
		l_ptMouse = ScreenToClient( l_ptMouse );
		wxRect l_rcBar = l_pToolbar->GetRect();
		PopupMenu( m_pSettingsMenu, l_ptMouse.x, 0 );
	}
	else
	{
		PopupMenu( m_pSettingsMenu );
	}

	p_event.Skip();
}

void MainFrame::OnInit3D( wxTimerEvent & WXUNUSED( p_event ) )
{
	delete m_pTimerInit;
	m_pTimerInit = NULL;
	m_3dFrame->Show();

	if ( m_bMultiFrames )
	{
		m_2dFrameHD->Show();
		m_2dFrameBG->Show();
		m_2dFrameBD->Show();
	}

	m_3dFrame->InitialiseRenderWindow();

	if ( m_bMultiFrames )
	{
		m_2dFrameHD->InitialiseRenderWindow();
		m_2dFrameBG->InitialiseRenderWindow();
		m_2dFrameBD->InitialiseRenderWindow();
	}

	ShowPanels();
	LightSPtr l_light1 = m_mainScene->CreateLight( cuT( "Light1" ), m_mainScene->CreateSceneNode( cuT( "Light1Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

	if ( l_light1 )
	{
		l_light1->GetDirectionalLight()->SetDirection( Point3r( 0.0f, 0.0f, 1.0f ) );
		l_light1->SetDiffuse( 1.0f, 1.0f, 1.0f );
		l_light1->SetSpecular( 1.0f, 1.0f, 1.0f );
		l_light1->SetEnabled( true );
	}

	LightSPtr l_light2 = m_mainScene->CreateLight( cuT( "Light2" ), m_mainScene->CreateSceneNode( cuT( "Light2Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

	if ( l_light2 )
	{
		l_light2->GetDirectionalLight()->SetDirection( Point3r( 0.0f, -1.0f, 1.0f ) );
		l_light2->SetDiffuse( 1.0f, 1.0f, 1.0f );
		l_light2->SetSpecular( 1.0f, 1.0f, 1.0f );
		l_light2->SetEnabled( true );
	}

	LightSPtr l_light3 = m_mainScene->CreateLight( cuT( "Light3" ), m_mainScene->CreateSceneNode( cuT( "Light3Node" ) ), eLIGHT_TYPE_DIRECTIONAL );

	if ( l_light3 )
	{
		l_light3->GetDirectionalLight()->SetDirection( Point3r( -1.0f, -1.0f, -1.0f ) );
		l_light3->SetDiffuse( 1.0f, 1.0f, 1.0f );
		l_light3->SetSpecular( 1.0f, 1.0f, 1.0f );
		l_light3->SetEnabled( true );
	}

	if ( m_timer == NULL )
	{
		m_timer = new wxTimer( this, 1 );
		m_timer->Start( 40 );
	}

	/*
		OverlayPtr l_pOverlay1 = Factory<Overlay>::Create( m_mainScene, "Overlay1", eOverlayPanel, nullptr, Point2r( 0.25f, 0.25f), Point2r( 0.5f, 0.5f));
		l_pOverlay1->SetMaterial( Factory<Material>::Create( "Overlay1", 1));
		OverlayPtr l_pOverlay2 = Factory<Overlay>::Create( m_mainScene, "Overlay2", eOverlayPanel, l_pOverlay, Point2r( 0.25f, 0.25f), Point2r( 0.5f, 0.5f));
		l_pOverlay2->SetMaterial( Factory<Material>::Create( "Overlay2", 1));
	*/
}
