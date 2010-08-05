#include "PrecompiledHeader.h"

#include "RenderPanel.h"

#include "MainFrame.h"
#include "GeometriesListFrame.h"
#include "MaterialsFrame.h"

using namespace CastorViewer;
using namespace Castor3D;

MainFrame :: MainFrame( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 600)),
		m_3dFrame( NULL),
		m_castor3D( NULL)
{
	m_log = new Log( C3D_T( "Castor.log"));

	_buildMenuBar();
	_initialise3D();
	 wxInitAllImageHandlers();
}

MainFrame :: ~MainFrame()
{
	delete m_log;
}

void MainFrame :: ShowPanels()
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
		m_mainScene->ClearScene();
		Log::LogMessage( C3D_T( "Scene cleared"));
		MeshManager::GetSingletonPtr()->Clear();
		Log::LogMessage( C3D_T( "Mesh manager cleared"));
		MaterialManager::GetSingletonPtr()->Clear();
		Log::LogMessage( C3D_T( "Material manager cleared"));
		Log::LogMessage( m_strFilePath.c_str());

		SceneFileParser l_parser;
		l_parser.ParseFile( m_strFilePath.c_str());
	}
}

void MainFrame :: _initialise3D()
{
	Log::LogMessage( C3D_T( "Initialising Castor3D"));

	m_castor3D = new Root( 25);

	try
	{
#ifdef _DEBUG
#	if C3D_UNICODE
			m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemdu.dll"));
#	else
			m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemd.dll"));
#	endif
#else
#	if C3D_UNICODE
			m_castor3D->LoadPlugin( C3D_T( "GLRenderSystemu.dll"));
#	else
			m_castor3D->LoadPlugin( C3D_T( "GLRenderSystem.dll"));
#	endif
#endif

		RendererDriver * l_driver = m_castor3D->GetRendererServer().GetDriver( 0);

		if (l_driver != NULL)
		{
			l_driver->CreateRenderSystem();
		}
		else
		{
			return;
		}

		m_mainScene = SceneManager::GetSingletonPtr()->CreateElement( C3D_T( "MainScene"));

		Log::LogMessage( C3D_T( "Castor3D Initialised"));
		int l_width = GetClientSize().x;
		int l_height = GetClientSize().y;
		m_3dFrame = new RenderPanel( this, wxID_ANY, pt3DView, m_mainScene, wxPoint( 0, 0), wxSize( l_width, l_height));

		m_castor3D->StartRendering();
		m_3dFrame->Show();

		ShowPanels();
	}
	catch ( ... )
	{
	}
}

void MainFrame :: _buildMenuBar()
{
	wxMenu * l_menu;
	wxMenuBar * l_menuBar = new wxMenuBar();

	l_menu = new wxMenu();
	l_menu->Append( eLoadScene, C3D_T( "&Ouvrir une Scene\tCTRL+F+O"));
	l_menu->AppendSeparator();
	l_menu->Append( eExit, C3D_T( "&Quitter\tALT+F4"));
	l_menuBar->Append( l_menu, C3D_T( "&Fichier"));

	l_menu = new wxMenu();
	l_menu->Append( eGeometries, C3D_T( "&Geometries\tCTRL+A+G"));
	l_menu->Append( eMaterials, C3D_T( "&Materiaux\tCTRL+A+M"));
	l_menuBar->Append( l_menu, C3D_T( "&Affichage"));

	SetMenuBar( l_menuBar);
}


BEGIN_EVENT_TABLE( MainFrame, wxFrame)
	EVT_SIZE(				MainFrame::_onSize)
	EVT_MOVE(				MainFrame::_onMove)
	EVT_CLOSE(				MainFrame::_onClose)
	EVT_ENTER_WINDOW(		MainFrame::_onEnterWindow)
	EVT_LEAVE_WINDOW(		MainFrame::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	MainFrame::_onEraseBackground)
	EVT_KEY_UP(				MainFrame::_onKeyUp)

    EVT_MENU( wxID_EXIT,	MainFrame::_onMenuClose)
	EVT_MENU( eExit,		MainFrame::_onMenuClose)
	EVT_MENU( eLoadScene,	MainFrame::_onLoadScene)
	EVT_MENU( eGeometries,	MainFrame::_onShowGeometriesList)
	EVT_MENU( eMaterials,	MainFrame::_onShowMaterialsList)
END_EVENT_TABLE()

void MainFrame :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);

	int l_width = GetClientSize().x;
	int l_height = GetClientSize().y;

	if (m_3dFrame != NULL)
	{
		m_3dFrame->SetSize( l_width, l_height);
		m_3dFrame->SetPosition( wxPoint( 0, 0));
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
	if (m_castor3D != NULL)
	{
		m_castor3D->EndRendering();
	}

	if (m_3dFrame != NULL)
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true);
		m_3dFrame = NULL;
	}

	if (m_castor3D != NULL)
	{
		delete m_castor3D;
	}

	delete m_log;

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
	wxString l_wildcard = C3D_T( "Castor3D scene files (*.escn)|*.escn");
	wxFileDialog * l_fileDialog = new wxFileDialog( this, C3D_T( "Ouvrir une scene"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		m_strFilePath = makeString( l_fileDialog->GetPath());
		m_strFilePath.Replace( C3D_T( "\\"), C3D_T( "/"));

		LoadScene();
	}

	ShowPanels();
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	GeometriesListFrame * l_listFrame = new GeometriesListFrame( this, "Geometries", m_mainScene, wxDefaultPosition, wxSize( 200, 300));
	l_listFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	MaterialsFrame * l_listFrame = new MaterialsFrame( this, "Materiaux", wxDefaultPosition);
	l_listFrame->Show();
}
