#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/RenderPanel.hpp"

#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/CastorViewer.hpp"

#include "xpms/geo_blanc.xpm"
#include "xpms/scene_blanc.xpm"
#include "xpms/mat_blanc.xpm"
#include "xpms/castor_transparent.xpm"
#include "xpms/castor.xpm"
#include "xpms/geo_visible.xpm"
#include "xpms/geo_visible_sel.xpm"
#include "xpms/geo_cachee.xpm"
#include "xpms/geo_cachee_sel.xpm"
#include "xpms/dossier.xpm"
#include "xpms/dossier_sel.xpm"
#include "xpms/dossier_ouv.xpm"
#include "xpms/dossier_ouv_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"

using namespace CastorViewer;
using namespace Castor3D;

DECLARE_APP( CastorViewerApp)

MainFrame :: MainFrame( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxFrame( nullptr, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 600))
	,	m_3dFrame( nullptr)
	,	m_pImagesLoader( new ImagesLoader)
	,	m_timer( nullptr)
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	m_pSplashScreen = new wxSplashScreen( this, wxT( "Castor Viewer"), wxT( "2010 DragonJoker, All rights shared"), wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2), 21);
	_initialiseImages();
	CreateStatusBar();
	_populateToolbar();
	wxIcon l_icon = wxIcon( castor_xpm);
	SetIcon( l_icon);
	_initialise3D();
	delete m_pSplashScreen;
	Show();
}

MainFrame :: ~MainFrame()
{
	if (m_timer)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = nullptr;
	}
}

void MainFrame :: LoadScene( const wxString & p_strFileName)
{
	if ( ! p_strFileName.empty())
	{
		m_strFilePath = (const wxChar *)p_strFileName.c_str();
	}

	if ( ! m_strFilePath.empty())
	{
		Root::MaterialManager * l_pMtlManager = Root::GetSingleton()->GetMaterialManager();
		Collection<Mesh, String> l_mshCollection;
		Collection<Scene, String> l_scnCollection;
		m_strFilePath.to_lower_case();
		m_mainScene->ClearScene();
		Logger::LogMessage( cuT( "Scene cleared"));
		l_mshCollection.Clear();
		Logger::LogMessage( cuT( "Mesh manager cleared"));
		l_pMtlManager->Clear();
		Logger::LogMessage( cuT( "Material manager cleared"));
		Logger::LogMessage( cuT( "Loading scene file : ") + m_strFilePath);
		ImporterPluginPtr l_pPlugin;
		Importer * l_pImporter;

		if (m_strFilePath.GetExtension().lower_case() == cuT( "cscn"))
		{
			Path l_matFilePath = m_strFilePath;
			l_matFilePath.replace( cuT( "cscn"), cuT( "cmtl"));

			if (File::FileExists( l_matFilePath))
			{
				File l_fileMat( l_matFilePath, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII);
				Logger::LogMessage( cuT( "Loading materials file : ") + l_matFilePath);

				if (l_pMtlManager->Read( l_fileMat))
				{
					Logger::LogMessage( cuT( "Materials read"));
				}
				else
				{
					Logger::LogMessage( cuT( "Can't read materials"));
					return;
				}
			}

			Path l_meshFilePath = m_strFilePath;
			l_meshFilePath.replace( cuT( "cscn"), cuT( "cmsh"));

			if (File::FileExists( l_meshFilePath))
			{
				File l_fileMesh( l_meshFilePath, File::eOPEN_MODE_READ);
				Logger::LogMessage( cuT( "Loading meshes file : ") + l_meshFilePath);

				if (Root::GetSingleton()->LoadMeshes( l_fileMesh))
				{
					Logger::LogMessage( cuT( "Meshes read"));
				}
				else
				{
					Logger::LogMessage( cuT( "Can't read meshes"));
					return;
				}
			}

			SceneFileParser l_parser;
			l_parser.ParseFile( m_strFilePath, m_pSceneNode);
		}
		else
		{
			for (PluginStrMap::iterator l_it = Root::GetSingleton()->PluginsBegin( ePLUGIN_TYPE_IMPORTER) ; l_it != Root::GetSingleton()->PluginsEnd( ePLUGIN_TYPE_IMPORTER) ; ++l_it)
			{
				l_pPlugin = static_pointer_cast<ImporterPlugin, PluginBase>( l_it->second);

				if (m_strFilePath.GetExtension().lower_case() == l_pPlugin->GetExtension().lower_case())
				{
					l_pImporter = l_pPlugin->CreateImporter();
					m_mainScene->ImportExternal( m_strFilePath, l_pImporter);
				}
			}
		}
	}
}

void MainFrame :: _initialise3D()
{
	m_pSplashScreen->Step( wxT( "Loading plugins"), 1);
	Logger::LogMessage( cuT( "Initialising Castor3D"));

	Root::GetSingleton()->Initialise( 25);

	StringArray l_arrayFiles;
	File::ListDirectoryFiles( File::DirectoryGetCurrent() / cuT( "Plugins"), l_arrayFiles);

	if (l_arrayFiles.size() > 0)
	{
		for (size_t i = 0 ; i < l_arrayFiles.size() ; i++)
		{
			Path l_file = l_arrayFiles[i];

			if (l_file.GetExtension() == CASTOR_DLL_EXT)
			{
				m_pSplashScreen->SubStatus( l_file.GetFileName());
				Root::GetSingleton()->LoadPlugin( l_file);
			}
		}
	}

	Logger::LogMessage( cuT( "Plugins loaded"));

	try
	{
		bool l_bRendererInit = false;
		wxRendererSelector m_dialog( this, wxT( "Castor Viewer"));
		m_pSplashScreen->Step( wxT( "Initialising main window"), 1);

		int l_iReturn = m_dialog.ShowModal();

		if (l_iReturn != wxID_CANCEL)
		{
			l_bRendererInit = Root::GetSingleton()->LoadRenderer( eRENDERER_TYPE( l_iReturn));

			if (l_bRendererInit)
			{
				m_mainScene = Factory<Scene>::Create( cuT( "MainScene"));

				Logger::LogMessage( cuT( "Castor3D Initialised"));
				int l_width = GetClientSize().x;
				int l_height = GetClientSize().y;
				m_3dFrame = new RenderPanel( this, wxID_ANY, eVIEWPORT_TYPE_3D, m_mainScene, wxPoint( 0, 0), wxSize( l_width, l_height));
				m_3dFrame->Show();

				if (m_timer == NULL)
				{
					m_timer = new wxTimer( this, 1);
					m_timer->Start( 40);
				}
			}
		}
		else
		{
			GetToolBar()->Hide();
		}
	}
	catch ( ... )
	{
		wxMessageBox( wxT( "Problem occured while initialising Castor3D.\nLook at CastorViewer.log for more details"), wxT( "Exception"), wxOK | wxCENTRE | wxICON_ERROR);
		Close( true);
	}
}

void MainFrame :: _initialiseImages()
{
	m_pSplashScreen->Step( wxT( "Loading images"), 1);
	m_pSplashScreen->SubStatus( wxT( "castor_transparent_xpm"));
	ImagesLoader::AddBitmap( CV_IMG_CASTOR,									castor_transparent_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "geo_visible_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE,			geo_visible_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "geo_visible_sel_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_VISIBLE_SEL,		geo_visible_sel_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "geo_cachee_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN,			geo_cachee_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "geo_cachee_sel_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_HIDDEN_SEL,		geo_cachee_sel_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "dossier_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY,			dossier_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "dossier_sel_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_SEL,		dossier_sel_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "dossier_ouv_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN,		dossier_ouv_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "dossier_ouv_sel_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_GEOMETRY_OPEN_SEL,	dossier_ouv_sel_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "submesh_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH,			submesh_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "submesh_sel_xpm"));
	ImagesLoader::AddBitmap( wxGeometriesListFrame::eBMP_SUBMESH_SEL,		submesh_sel_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "scene_blanc_xpm"));
	ImagesLoader::AddBitmap( eBMP_SCENE,									scene_blanc_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "geo_blanc_xpm"));
	ImagesLoader::AddBitmap( eBMP_GEOMETRIES,								geo_blanc_xpm);
	m_pSplashScreen->Step( 1);
	m_pSplashScreen->SubStatus( wxT( "mat_blanc_xpm"));
	ImagesLoader::AddBitmap( eBMP_MATERIALS,								mat_blanc_xpm);
	m_pSplashScreen->Step( 1);
}

void MainFrame :: _populateToolbar()
{
	m_pSplashScreen->Step( wxT( "Loading toolbar"), 1);
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL);

	l_pToolbar->SetBackgroundColour( * wxWHITE);
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32));

	l_pToolbar->AddTool( eID_TOOL_LOAD_SCENE,	wxT( "Scene"),		ImagesLoader::AddBitmap( eBMP_SCENE, scene_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Open a new scene"));
	m_pSplashScreen->Step( 1);
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eID_TOOL_GEOMETRIES,	wxT( "Geometries"),	ImagesLoader::AddBitmap( eBMP_GEOMETRIES, geo_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Display geometries"));
	m_pSplashScreen->Step( 1);
	l_pToolbar->AddTool( eID_TOOL_MATERIALS,	wxT( "Materials"),	ImagesLoader::AddBitmap( eBMP_MATERIALS, mat_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Display materials"));
	m_pSplashScreen->Step( 1);
	l_pToolbar->AddSeparator();

	l_pToolbar->Realize();
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_TIMER(	1,					MainFrame::_onTimer)
	EVT_PAINT(						MainFrame::_onPaint)
	EVT_SIZE(						MainFrame::_onSize)
	EVT_MOVE(						MainFrame::_onMove)
	EVT_CLOSE(						MainFrame::_onClose)
	EVT_ENTER_WINDOW(				MainFrame::_onEnterWindow)
	EVT_LEAVE_WINDOW(				MainFrame::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(			MainFrame::_onEraseBackground)
	EVT_KEY_UP(						MainFrame::_onKeyUp)
	EVT_TOOL( eID_TOOL_LOAD_SCENE,	MainFrame::_onLoadScene)
	EVT_TOOL( eID_TOOL_GEOMETRIES,	MainFrame::_onShowGeometriesList)
	EVT_TOOL( eID_TOOL_MATERIALS,	MainFrame::_onShowMaterialsList)
END_EVENT_TABLE()

void MainFrame :: _onPaint( wxPaintEvent & event)
{
	wxPaintDC l_paintDC( this);
	event.Skip();
}

void MainFrame :: _onTimer( wxTimerEvent & WXUNUSED(event))
{
	Root::GetSingleton()->RenderOneFrame();
}

void MainFrame :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);

	int l_width = GetClientSize().x;
	int l_height = GetClientSize().y;

	if (m_3dFrame)
	{
		m_3dFrame->SetSize( l_width, l_height);
		m_3dFrame->SetPosition( wxPoint( 0, 0));
	}
}

void MainFrame :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
}

void MainFrame :: _onClose( wxCloseEvent & event)
{
	Hide();

	if (m_timer)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = nullptr;
	}

	delete m_pImagesLoader;

	if (m_mainScene)
	{
		m_mainScene.reset();
	}

	Root::GetSingleton()->EndRendering();

	if (m_3dFrame)
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true);
		m_3dFrame = nullptr;
	}

	DestroyChildren();
	Root::GetSingleton()->Clear();
	Destroy();
}

void MainFrame :: _onEnterWindow( wxMouseEvent & event)
{
    SetFocus();
}

void MainFrame :: _onLeaveWindow( wxMouseEvent & event)
{
}

void MainFrame :: _onEraseBackground( wxEraseEvent & event)
{
}

void MainFrame :: _onKeyUp( wxKeyEvent & event)
{
	switch (event.GetKeyCode())
	{
	case WXK_F5:
		LoadScene();
		break;
	}
}

void MainFrame :: _onLoadScene( wxCommandEvent & event)
{
	wxString l_wildcard = wxT( "Castor3D scene files (*.cscn)|*.cscn");

	for (PluginStrMap::iterator l_it = Root::GetSingleton()->PluginsBegin( ePLUGIN_TYPE_IMPORTER) ; l_it != Root::GetSingleton()->PluginsEnd( ePLUGIN_TYPE_IMPORTER) ; ++l_it)
	{
		ImporterPluginPtr l_pPlugin = static_pointer_cast< ImporterPlugin >( l_it->second);
		l_wildcard += wxT( "|") + l_pPlugin->GetExtension().upper_case() + wxT( " files (*.") + l_pPlugin->GetExtension().lower_case() + wxT( ")|*.") + l_pPlugin->GetExtension().lower_case();
	}

	wxFileDialog l_fileDialog( this, wxT( "Open a scene"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog.ShowModal() == wxID_OK)
	{
		m_strFilePath = (const wxChar *)l_fileDialog.GetPath().c_str();

		LoadScene();
	}
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	wxGeometriesListFrame * l_pFrame = new wxGeometriesListFrame( this, wxT( "Geometries"), m_mainScene, wxDefaultPosition, wxSize( 200, 300));
	l_pFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	wxMaterialsFrame * l_pFrame = new wxMaterialsFrame( this, wxT( "Materials"), wxDefaultPosition);
	l_pFrame->Show();
}
