#include "CastorViewer/PrecompiledHeader.h"

#include "CastorViewer/RenderPanel.h"

#include "CastorViewer/MainFrame.h"

using namespace CastorViewer;
using namespace Castor3D;

MainFrame :: MainFrame( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 600)),
		m_3dFrame( NULL),
		m_castor3D( NULL)
{
	CreateStatusBar();
	_populateToolbar();
	_initialise3D();
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
		m_mainScene->ClearScene();
		Logger::LogMessage( CU_T( "Scene cleared"));
		m_castor3D->GetSceneManager()->GetMeshManager()->Clear();
		Logger::LogMessage( CU_T( "Mesh manager cleared"));
		m_castor3D->GetSceneManager()->GetMaterialManager()->Clear();
		Logger::LogMessage( CU_T( "Material manager cleared"));
		Logger::LogMessage( m_strFilePath.c_str());

		if (m_castor3D->GetSceneManager()->GetMaterialManager()->Read( m_strFilePath))
		{
			Logger::LogMessage( CU_T( "Materials read"));
		}
		else
		{
			Logger::LogMessage( CU_T( "Can't read materials"));
			return;
		}

		SceneFileParser l_parser( m_castor3D->GetSceneManager());
		l_parser.ParseFile( m_strFilePath.c_str());
	}
}

void MainFrame :: _initialise3D()
{
	Logger::LogMessage( CU_T( "Initialising Castor3D"));

	m_castor3D = new Root( 25);

	try
	{
		wxStandardPaths * l_paths = (wxStandardPaths *) & wxStandardPaths::Get();
		wxString l_dataDir = l_paths->GetDataDir();
		bool l_bRendererInit = m_castor3D->LoadRenderer( RendererDriver::eOpenGL3, l_dataDir.c_str());

		if ( ! l_bRendererInit)
		{
			l_bRendererInit = m_castor3D->LoadRenderer( RendererDriver::eOpenGL2, l_dataDir.c_str());
		}

		if (l_bRendererInit)
		{
			m_mainScene = m_castor3D->GetSceneManager()->CreateElement( "MainScene");

			Logger::LogMessage( CU_T( "Castor3D Initialised"));
			int l_width = GetClientSize().x;
			int l_height = GetClientSize().y;
			m_3dFrame = new RenderPanel( this, wxID_ANY, Viewport::e3DView, m_mainScene, wxPoint( 0, 0), wxSize( l_width, l_height));
			m_3dFrame->Show();
		}
	}
	catch ( ... )
	{
		wxMessageBox( CU_T( "Problème survenu lors de l'initialisation de Castor3D"));
		Close( true);
	}
}

void MainFrame :: _populateToolbar()
{
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL);

	l_pToolbar->SetBackgroundColour( * wxWHITE);
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32));

	l_pToolbar->AddTool( eLoadScene,	wxBITMAP( Scene).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Scène"), wxT( "Ouvrir une nouvelle scène"));
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eGeometries,	wxBITMAP( Geometry).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Géométries"), wxT( "Afficher les géométries"));
	l_pToolbar->AddTool( eMaterials,	wxBITMAP( Material).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Matériaux"), wxT( "Afficher les matériaux"));
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

	if (m_3dFrame != NULL)
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

	if (m_3dFrame != NULL)
	{
		m_3dFrame->UnFocus();
		m_3dFrame->Close( true);
		m_3dFrame = NULL;
	}

	DestroyChildren();

	if ( ! m_mainScene == NULL)
	{
		m_mainScene.reset();
	}

	if (m_castor3D != NULL)
	{
		m_castor3D->EndRendering();
	}

	if (m_castor3D != NULL)
	{
		delete m_castor3D;
	}

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
	wxString l_wildcard = CU_T( "Castor3D scene files (*.cscn)|*.cscn");
	wxFileDialog l_fileDialog( this, wxT( "Ouvrir une scene"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog.ShowModal() == wxID_OK)
	{
		m_strFilePath = l_fileDialog.GetPath().c_str();
		m_strFilePath.Replace( wxT( "\\"), wxT( "/"));

		LoadScene();
	}
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	GeometriesListFrame * l_pFrame = new GeometriesListFrame( m_castor3D->GetSceneManager()->GetMaterialManager(), this, wxT( "Géometries"), m_mainScene, wxDefaultPosition, wxSize( 200, 300));
	l_pFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	MaterialsFrame * l_pFrame = new MaterialsFrame( m_castor3D->GetSceneManager()->GetMaterialManager(), this, wxT( "Matériaux"), wxDefaultPosition);
	l_pFrame->Show();
}
