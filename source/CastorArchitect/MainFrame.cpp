#include "CastorArchitect/PrecompiledHeader.h"

#include "CastorArchitect/RenderPanel3D.h"

#include "CastorArchitect/MainFrame.h"

using namespace CastorArchitect;
using namespace Castor3D;

MainFrame :: MainFrame( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 600)),
		m_p3dFrame( NULL),
		m_pCastor3D( NULL)
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
		m_pMainScene->ClearScene();
		Logger::LogMessage( CU_T( "Scene cleared"));
		m_pCastor3D->GetSceneManager()->GetMeshManager()->Clear();
		Logger::LogMessage( CU_T( "Mesh manager cleared"));
		m_pCastor3D->GetSceneManager()->GetMaterialManager()->Clear();
		Logger::LogMessage( CU_T( "Material manager cleared"));
		Logger::LogMessage( m_strFilePath.c_str());

		if (m_pCastor3D->GetSceneManager()->GetMaterialManager()->Read( m_strFilePath.c_str()))
		{
			Logger::LogMessage( CU_T( "Materials read"));
		}
		else
		{
			Logger::LogMessage( CU_T( "Can't read materials"));
			return;
		}

		SceneFileParser l_parser( m_pCastor3D->GetSceneManager());
		l_parser.ParseFile( m_strFilePath.c_str());
	}
}

void MainFrame :: _initialise3D()
{
	Logger::LogMessage( CU_T( "Initialising Castor3D"));

	m_pCastor3D = new Root( 25);

	try
	{
		m_pCastor3D->LoadRenderer( RendererDriver::eOpenGL2);
		m_pMainScene = m_pCastor3D->GetSceneManager()->CreateElement( CU_T( "MainScene"));

		Logger::LogMessage( CU_T( "Castor3D Initialised"));
		int l_width = GetClientSize().x;
		int l_height = GetClientSize().y;
		m_p3dFrame = new RenderPanel( this, wxID_ANY, Viewport::e3DView, m_pMainScene, wxPoint( 0, 0), wxSize( l_width, l_height));
		m_p3dFrame->Show();
	}
	catch ( ... )
	{
	}
}

void MainFrame :: _populateToolbar()
{
	wxToolBar * l_pToolbar = CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL);

	l_pToolbar->SetBackgroundColour( * wxWHITE);
	l_pToolbar->SetToolBitmapSize( wxSize( 32, 32));

	l_pToolbar->AddTool( eLoadScene,	wxBITMAP( Scene).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),		wxT( "Sc�ne"), wxT( "Ouvrir une nouvelle sc�ne"));
	l_pToolbar->AddSeparator();
	l_pToolbar->AddTool( eGeometries,	wxBITMAP( Geometry).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "G�om�tries"), wxT( "Afficher les g�om�tries"));
	l_pToolbar->AddTool( eMaterials,	wxBITMAP( Material).ConvertToImage().Rescale( 32, 32, wxIMAGE_QUALITY_HIGH),	wxT( "Mat�riaux"), wxT( "Afficher les mat�riaux"));
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

	if (m_p3dFrame != NULL)
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

	if (m_p3dFrame != NULL)
	{
		m_p3dFrame->UnFocus();
		m_p3dFrame->Close( true);
		m_p3dFrame = NULL;
	}

	DestroyChildren();

	if ( ! m_pMainScene == NULL)
	{
		m_pMainScene.reset();
	}

	if (m_pCastor3D != NULL)
	{
		m_pCastor3D->EndRendering();
	}

	if (m_pCastor3D != NULL)
	{
		delete m_pCastor3D;
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

void MainFrame :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}

void MainFrame :: _onLoadScene( wxCommandEvent & event)
{
	wxString l_wildcard = CU_T( "Castor3D scene files (*.cscn)|*.cscn");
	wxFileDialog * l_fileDialog = new wxFileDialog( this, CU_T( "Ouvrir une sc�ne"), wxEmptyString, wxEmptyString, l_wildcard);

	if (l_fileDialog->ShowModal() == wxID_OK)
	{
		m_strFilePath = l_fileDialog->GetPath().c_str();
		m_strFilePath.Replace( CU_T( "\\"), CU_T( "/"));

		LoadScene();
	}
}

void MainFrame :: _onShowGeometriesList( wxCommandEvent & event)
{
	GeometriesListFrame * l_listFrame = new GeometriesListFrame( m_pCastor3D->GetSceneManager()->GetMaterialManager(), this, CU_T( "G�ometries"), m_pMainScene, wxDefaultPosition, wxSize( 200, 300));
	l_listFrame->Show();
}

void MainFrame :: _onShowMaterialsList( wxCommandEvent & event)
{
	MaterialsFrame * l_listFrame = new MaterialsFrame( m_pCastor3D->GetSceneManager()->GetMaterialManager(), this, CU_T( "Materiaux"), wxDefaultPosition);
	l_listFrame->Show();
}