#include "CastorArchitect/PrecompiledHeader.hpp"

#include "CastorArchitect/RenderPanel.hpp"

#include "CastorArchitect/MainFrame.hpp"
#include "CastorArchitect/CastorArchitect.hpp"

#include "xpms/geo_blanc.xpm"
#include "xpms/scene_blanc.xpm"
#include "xpms/mat_blanc.xpm"
#include "xpms/castor_transparent.xpm"

using namespace CastorArchitect;
using namespace Castor3D;

DECLARE_APP( CastorArchitectApp)

MainFrame :: MainFrame( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 600))
	,	m_p3dFrame( nullptr)
	,	m_pImagesLoader( new ImagesLoader)
{
	wxGetApp().GetSplashScreen()->Step( "Initialisation des images", 1);
	ImagesLoader::AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm);
	CreateStatusBar();
	_populateToolbar();
	wxGetApp().GetSplashScreen()->Step( "Chargement des plugins", 1);
	_initialise3D();
	wxGetApp().DestroySplashScreen();
}

MainFrame :: ~MainFrame()
{
}

void MainFrame :: LoadScene( const String & p_strFileName)
{
	if (m_strFilePath.empty())
	{
		m_strFilePath = p_strFileName;
	}

	if ( ! m_strFilePath.empty())
	{
		Collection<Material, String> l_mtlCollection;
		Collection<Mesh, String> l_mshCollection;
		m_pMainScene->ClearScene();
		Logger::LogMessage( cuT( "Scene cleared"));
		l_mshCollection.Clear();
		Logger::LogMessage( cuT( "Mesh manager cleared"));
		l_mtlCollection.Clear();
		Logger::LogMessage( cuT( "Material manager cleared"));
		Logger::LogMessage( m_strFilePath.c_str());

		Path l_matFilePath = m_strFilePath;
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
		l_parser.ParseFile( m_strFilePath.c_str());
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
		wxRendererSelector m_dialog( this, wxT( "Castor Architect"));
		wxGetApp().GetSplashScreen()->Step( "Initialisation de la vue", 1);

		int l_iReturn = m_dialog.ShowModal();

		if (l_iReturn != wxID_CANCEL)
		{
			l_bRendererInit = m_castor3D.LoadRenderer( eRENDERER_TYPE( l_iReturn));

			if (l_bRendererInit)
			{
				m_pMainScene = Factory<Scene>::Create( "MainScene");

				Logger::LogMessage( cuT( "Castor3D Initialised"));
				int l_width = GetClientSize().x;
				int l_height = GetClientSize().y;
				m_p3dFrame = new RenderPanel( this, wxID_ANY, Viewport::e3DView, m_pMainScene, wxPoint( 0, 0), wxSize( l_width, l_height));
				m_p3dFrame->Show();
			}
		}
	}
	catch ( ... )
	{
		wxMessageBox( cuT( "Problème survenu lors de l'initialisation de Castor3D"), cuT( "Exception"), wxOK | wxCENTRE | wxICON_ERROR);
		Close( true);
	}
}

void MainFrame :: _populateToolbar()
{
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL);

	l_pToolbar->SetBackgroundColour( * wxWHITE);
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32));

	l_pToolbar->AddTool( eLoadScene,	wxT( "Scène"),		ImagesLoader::AddBitmap( eBmpScene, scene_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Ouvrir une nouvelle scène"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eGeometries,	wxT( "Géométries"),	ImagesLoader::AddBitmap( eBmpGeometries, geo_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Afficher les géométries"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddTool( eMaterials,	wxT( "Matériaux"),	ImagesLoader::AddBitmap( eBmpMaterials, mat_blanc_xpm)->ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Afficher les matériaux"));
	wxGetApp().GetSplashScreen()->Step( 1);
	l_pToolbar->AddSeparator();

	l_pToolbar->Realize();
}

BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_SIZE(				MainFrame::_onSize)
	EVT_MOVE(				MainFrame::_onMove)
	EVT_CLOSE(				MainFrame::_onClose)
	EVT_ENTER_WINDOW(		MainFrame::_onEnterWindow)
	EVT_LEAVE_WINDOW(		MainFrame::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	MainFrame::_onEraseBackground)
	EVT_KEY_UP(				MainFrame::_onKeyUp)
	EVT_TOOL( eLoadScene,	MainFrame::_onLoadScene)
	EVT_TOOL( eGeometries,	MainFrame::_onShowGeometriesList)
	EVT_TOOL( eMaterials,	MainFrame::_onShowMaterialsList)
END_EVENT_TABLE()

void MainFrame :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);

	int l_width = GetClientSize().x;
	int l_height = GetClientSize().y;

	if (m_p3dFrame)
	{
		m_p3dFrame->SetSize( l_width, l_height);
		m_p3dFrame->SetPosition( wxPoint( 0, 0));
	}
}

void MainFrame :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
}

void MainFrame :: _onClose( wxCloseEvent & event)
{
	Hide();

	if (m_pMainScene)
	{
		m_pMainScene.reset();
	}

	m_castor3D.EndRendering();

	if (m_p3dFrame)
	{
		m_p3dFrame->UnFocus();
		m_p3dFrame->Close( true);
		m_p3dFrame = nullptr;
	}

	DestroyChildren();
	m_castor3D.Clear();
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

void MainFrame :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}

void MainFrame :: _onLoadScene( wxCommandEvent & event)
{
	wxString l_wildcard = wxT( "Castor3D scene files (*.cscn)|*.cscn");

	for (PluginStrMap::iterator l_it = m_castor3D.PluginsBegin( ePluginImporter) ; l_it != m_castor3D.PluginsEnd( ePluginImporter) ; ++l_it)
	{
		ImporterPluginPtr l_pPlugin = static_pointer_cast< ImporterPlugin >( l_it->second);
		l_wildcard += wxT( "|") + l_pPlugin->GetExtension().upper_case() + wxT( " files (*.") + l_pPlugin->GetExtension().lower_case() + wxT( ")|*.") + l_pPlugin->GetExtension().lower_case();
	}

	wxFileDialog * l_fileDialog = new wxFileDialog( this, cuT( "Ouvrir une scène"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		m_strFilePath = (const char *)l_fileDialog->GetPath().c_str();
		m_strFilePath.replace( cuT( "\\"), cuT( "/"));

		LoadScene();
	}
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	wxGeometriesListFrame * l_listFrame = new wxGeometriesListFrame( this, cuT( "Géometries"), m_pMainScene, wxDefaultPosition, wxSize( 200, 300));
	l_listFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	wxMaterialsFrame * l_listFrame = new wxMaterialsFrame( this, cuT( "Materiaux"), wxDefaultPosition);
	l_listFrame->Show();
}
