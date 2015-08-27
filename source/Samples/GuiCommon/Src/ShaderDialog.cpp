#include "ShaderDialog.hpp"
#include "StcTextEditor.hpp"
#include "FrameVariableDialog.hpp"

#include <Engine.hpp>
#include <RenderSystem.hpp>
#include <Pass.hpp>
#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

typedef enum eID
{
	eID_NOTEBOOK_EDITORS,
	eID_GRID_VERTEX,
	eID_GRID_FRAGMENT,
	eID_GRID_GEOMETRY,
	eID_GRID_HULL,
	eID_GRID_DOMAIN,
	eID_MENU_OPEN,
	eID_MENU_SAVE_ONE,
	eID_MENU_SAVE_ALL,
	eID_MENU_QUIT,
	eID_MENU_PREFS,
}	eID;

wxShaderDialog::wxShaderDialog( Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, PassSPtr p_pPass, wxPoint const & p_ptPosition, const wxSize p_ptSize )
	: wxFrame( p_pParent, wxID_ANY, _( "Shaders" ), p_ptPosition, p_ptSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
	, m_pPass( p_pPass )
	, m_bCompiled( true )
	, m_bOwnShader( true )
	, m_pStcContext( new StcContext )
#if defined( NDEBUG )
	, m_bCanEdit( p_bCanEdit )
#else
	, m_bCanEdit( p_bCanEdit || true )
#endif
	, m_pEngine( p_pEngine )
{
	wxPanel * l_pPanelBackGround = new wxPanel( this, wxID_ANY, wxPoint( 0, 0 ), GetClientSize() );
	l_pPanelBackGround->Show();
	wxArrayString l_arrayChoices;
	wxArrayString l_arrayTexts;
	PathArray l_arrayFiles;
	wxPoint l_ptButtonPosition = wxPoint( 10, 10 );
	wxSize l_sizeButton = wxSize( 65, 20 );
	wxSize l_size = GetClientSize();
	Path l_pathCurrent = Engine::GetDataDirectory() / cuT( "Castor3D" );
	RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();
	eSHADER_MODEL l_eMaxShaderModel = eSHADER_MODEL_COUNT;
	eSHADER_MODEL l_eShaderModel = eSHADER_MODEL_COUNT;
	wxString l_strExtension;

	if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_5 ) )
	{
		l_eMaxShaderModel = eSHADER_MODEL_5;
	}
	else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_4 ) )
	{
		l_eMaxShaderModel = eSHADER_MODEL_4;
	}
	else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_3 ) )
	{
		l_eMaxShaderModel = eSHADER_MODEL_3;
	}
	else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_2 ) )
	{
		l_eMaxShaderModel = eSHADER_MODEL_2;
	}
	else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_1 ) )
	{
		l_eMaxShaderModel = eSHADER_MODEL_1;
	}

	if ( l_eMaxShaderModel != eSHADER_MODEL_COUNT )
	{
		m_pShaderProgram = m_pPass.lock()->GetShader< ShaderProgramBase >();

		if ( m_pShaderProgram.lock() )
		{
			m_bOwnShader = false;
		}
		else
		{
			switch ( l_pRenderSystem->GetRendererType() )
			{
			case eRENDERER_TYPE_OPENGL:
				m_pShaderProgram = l_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
				break;

			case eRENDERER_TYPE_DIRECT3D9:
			case eRENDERER_TYPE_DIRECT3D10:
			case eRENDERER_TYPE_DIRECT3D11:
				m_pShaderProgram = l_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_HLSL );
				break;
			}
		}

		switch ( m_pShaderProgram.lock()->GetLanguage() )
		{
		case Castor3D::eSHADER_LANGUAGE_GLSL:
			l_strExtension = wxT( ".glsl" );
			break;

		case Castor3D::eSHADER_LANGUAGE_HLSL:
			l_strExtension = wxT( ".hlsl" );
			break;
		}

		l_arrayTexts.push_back( _( "Vertex" ) );
		l_arrayTexts.push_back( _( "Hull" ) );
		l_arrayTexts.push_back( _( "Domain" ) );
		l_arrayTexts.push_back( _( "Geometry" ) );
		l_arrayTexts.push_back( _( "Pixel" ) );
		l_arrayTexts.push_back( _( "Compute" ) );
		File::ListDirectoryFiles( l_pathCurrent, l_arrayFiles, true );
		Path l_pathFile;
		std::for_each( l_arrayFiles.begin(), l_arrayFiles.end(), [&]( String const & p_strFile )
		{
			l_pathFile = p_strFile;

			if ( l_pathFile.GetFileName()[0] != cuT( '.' ) && l_pathFile.GetExtension() == cuT( "lang" ) )
			{
				m_pStcContext->ParseFile( p_strFile );
			}
		} );
		m_pNotebookEditors = new wxNotebook( l_pPanelBackGround, eID_NOTEBOOK_EDITORS, wxPoint( 0, 0 ), wxSize( l_size.x, l_size.y ), wxNB_FIXEDWIDTH );
		ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
		wxPanel * l_pTmpPanels[eSHADER_TYPE_COUNT] = { NULL };
		wxStaticText * l_pTmpStatics[eSHADER_TYPE_COUNT] = { NULL };
		int l_iListWidth = 200;

		for ( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
		{
			l_arrayChoices.clear();
			l_arrayChoices.push_back( wxCOMBO_NEW );

			if ( l_pProgram->HasProgram( eSHADER_TYPE( i ) ) )
			{
// 				int l_iCount = 0;
//
// 				for( FrameVariablePtrList::const_iterator l_it = l_pProgram->GetFrameVariablesBegin( eSHADER_TYPE( i ) ); l_it != l_pProgram->GetFrameVariablesEnd( eSHADER_TYPE( i ) ); ++l_it )
// 				{
// 					l_arrayChoices.push_back( (*l_it)->GetName() );
// 					m_mapFrameVariables[i].insert( std::make_pair( l_iCount++, (*l_it) ) );
// 				}
				l_eShaderModel = l_eMaxShaderModel;

				while ( m_strShaderSources[i].empty() && m_strShaderFiles[i].empty() && l_eShaderModel >= eSHADER_MODEL_1 )
				{
					m_strShaderSources[i]	= l_pProgram->GetSource(	eSHADER_TYPE( i ), l_eShaderModel );
					m_strShaderFiles[i]		= l_pProgram->GetFile(	eSHADER_TYPE( i ), l_eShaderModel );

					if ( m_strShaderSources[i].empty() || m_strShaderFiles[i].empty() )
					{
						l_eShaderModel = eSHADER_MODEL( int( l_eShaderModel ) - 1 );
					}
				}
			}

			l_pTmpPanels[i] = new wxPanel( m_pNotebookEditors, wxID_ANY, wxPoint( 0, 0 ) );
			m_pNotebookEditors->AddPage( l_pTmpPanels[i], l_arrayTexts[i], true );
			l_pTmpPanels[i]->SetBackgroundColour( * wxWHITE );
			l_pTmpPanels[i]->SetSize( 0, 22, m_pNotebookEditors->GetClientSize().x, m_pNotebookEditors->GetClientSize().y - 22 );
			m_pStcEditors[i] = new wxStcTextEditor( m_pStcContext, l_pTmpPanels[i], wxID_ANY, wxPoint( l_iListWidth, 0 ), l_pTmpPanels[i]->GetClientSize() - wxSize( l_iListWidth, 0 ) );
			m_pStcEditors[i]->Show();
			l_pTmpStatics[i] = new wxStaticText( l_pTmpPanels[i], wxID_ANY, _( "Frame variables" ), wxPoint( 0, 0 ), wxSize( l_iListWidth, 25 ), wxALIGN_CENTRE );
			m_pListFrameVariables[i] = new wxListBox( l_pTmpPanels[i], eID_GRID_VERTEX + i, wxPoint( 0, 25 ), wxSize( l_iListWidth, 0 ), l_arrayChoices, wxBORDER_SIMPLE | wxWANTS_CHARS );
			m_pListFrameVariables[i]->Enable( m_bCanEdit );
			l_ptButtonPosition.y += 20;

			if ( !m_strShaderFiles[i].empty() )
			{
				m_pStcEditors[i]->LoadFile( m_strShaderFiles[i] );
				m_pStcEditors[i]->SetReadOnly( !m_bCanEdit );
			}
			else if ( !m_strShaderSources[i].empty() )
			{
				m_pStcEditors[i]->SetText( m_strShaderSources[i] );
				m_pStcEditors[i]->SetReadOnly( true );
			}

			m_pStcEditors[i]->InitializePrefs( m_pStcEditors[i]->DeterminePrefs( l_strExtension ) );
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
			m_pStcEditors[i]->AlwaysShowScrollbars( true, true );
#endif
		}

		for ( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
		{
			wxBoxSizer * l_pSizerTabList = new wxBoxSizer( wxVERTICAL );
			l_pSizerTabList->Add( l_pTmpStatics[i], wxSizerFlags( 0 ) );
			l_pSizerTabList->Add( m_pListFrameVariables[i], wxSizerFlags( 1 ) );
			wxBoxSizer * l_pSizerTab = new wxBoxSizer( wxHORIZONTAL );
			l_pSizerTab->Add( m_pStcEditors[i],	wxSizerFlags( 1 ).Expand() );
			l_pSizerTab->Add( l_pSizerTabList,	wxSizerFlags( 0 ).Expand() );
			l_pTmpPanels[i]->SetSizer( l_pSizerTab );
			l_pSizerTab->SetSizeHints( l_pTmpPanels[i] );
		}

		wxBoxSizer * l_pSizerPanel = new wxBoxSizer( wxHORIZONTAL	);
		l_pSizerPanel->Add( m_pNotebookEditors, wxSizerFlags( 1 ).Border( wxALL, 10 ).Expand() );
		l_pPanelBackGround->SetSizer( l_pSizerPanel );
		l_pSizerPanel->SetSizeHints( l_pPanelBackGround );
		wxBoxSizer * l_pSizerAll = new wxBoxSizer( wxHORIZONTAL	);
		l_pSizerAll->Add( l_pPanelBackGround, wxSizerFlags( 1 ).Expand() );
		this->SetSizer( l_pSizerAll );
		l_pSizerAll->SetSizeHints( this );
		DoPopulateMenu();
		this->Maximize();
	}
}

wxShaderDialog::~wxShaderDialog()
{
	delete m_pStcContext;
}

void wxShaderDialog::DoPopulateMenu()
{
	wxMenuBar * l_pMenuBar = new wxMenuBar;
	wxMenu * l_pMenu = new wxMenu;

	if ( m_bCanEdit )
	{
		l_pMenu->Append(	eID_MENU_OPEN,		_( "&Open file\tCTRL+O"	) );
		l_pMenu->Append(	eID_MENU_SAVE_ONE,	_( "Save &current file\tCTRL+S"	) );
		l_pMenu->Append(	eID_MENU_SAVE_ALL,	_( "Save &all files\tCTRL+SHIFT+S"	) );
		l_pMenu->AppendSeparator();
	}

	l_pMenu->Append(	eID_MENU_QUIT,		_( "&Quit\tCTRL+Q"	) );
	l_pMenuBar->Append(	l_pMenu,			_T( "&File"	) );
	l_pMenu = new wxMenu;
	l_pMenu->Append(	eID_MENU_PREFS,		_( "&Edit preferences ...\tCTRL+E"	) );
	l_pMenuBar->Append(	l_pMenu,			_T( "O&ptions"	) );
	SetMenuBar( l_pMenuBar );
}

void wxShaderDialog::DoCleanup()
{
	if ( m_bOwnShader && !m_pShaderProgram.expired() )
	{
		m_pPass.lock()->SetShader( nullptr );
//		m_pEngine->GetShaderManager().RemoveProgram( m_pShaderProgram.lock() );
		m_pShaderProgram.reset();
	}
}

void wxShaderDialog::DoLoadShader()
{
	if ( m_strShaderFiles[eSHADER_TYPE_VERTEX].empty() )
	{
		wxMessageBox( _( "Fill the vertex shader file name" ), _( "ERROR" ) );
	}
	else
	{
		if ( m_strShaderFiles[eSHADER_TYPE_PIXEL].empty() )
		{
			wxMessageBox( _( "Fill the fragment file name" ), _( "ERROR" ) );
		}
		else
		{
			if ( m_pShaderProgram.expired() )
			{
				m_pShaderProgram = m_pEngine->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
			}

			for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
			{
				if ( !m_strShaderFiles[i].empty() )
				{
					m_pStcEditors[i]->SaveFile( m_strShaderFiles[i] );
					m_pShaderProgram.lock()->CreateObject( eSHADER_TYPE( i ) );
					m_pShaderProgram.lock()->SetFile( eSHADER_TYPE( i ), eSHADER_MODEL_3, ( wxChar const * )m_strShaderFiles[i].c_str() );
				}
			}

			m_pPass.lock()->SetShader( m_pShaderProgram.lock() );
			m_bCompiled = true;
		}
	}
}

void wxShaderDialog::DoFolder( eSHADER_TYPE p_eType )
{
	wxFileDialog l_dialog( this, _( "Select a shader program file" ), wxEmptyString, wxEmptyString, wxEmptyString );

	if ( l_dialog.ShowModal() == wxID_OK )
	{
		m_strShaderFiles[p_eType] = l_dialog.GetPath();
		m_pStcEditors[p_eType]->LoadFile( m_strShaderFiles[p_eType] );
		m_bCompiled = false;
	}
}

void wxShaderDialog::DoSave( Castor3D::eSHADER_TYPE p_eType, bool p_bTell )
{
	if ( m_strShaderFiles[p_eType].empty() && p_bTell )
	{
		wxString l_wildcard;

		switch ( m_pShaderProgram.lock()->GetLanguage() )
		{
		case Castor3D::eSHADER_LANGUAGE_GLSL:
			l_wildcard = _( "GLSL Files" );
			l_wildcard += wxT( " (*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval)|*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval" );
			break;

		case Castor3D::eSHADER_LANGUAGE_HLSL:
			l_wildcard = _( "HLSL Files" );
			l_wildcard += wxT( " (*.hlsl)|*.hlsl" );
			break;
		}

		wxFileDialog l_dialog( this, _( "Save Shader file " ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			m_strShaderFiles[p_eType] = l_dialog.GetPath();
		}
	}

	if ( !m_strShaderFiles[p_eType].empty() )
	{
		m_pStcEditors[p_eType]->SaveFile( m_strShaderFiles[p_eType] );
		m_pStcEditors[p_eType]->SetReadOnly( !m_bCanEdit );
	}
}

void wxShaderDialog::DoGridCellChange( eSHADER_TYPE p_eType, int p_iRow )
{
	int l_iRow = p_iRow;
	FrameVariableSPtr l_pFrameVariable;

	if ( m_mapFrameVariables[p_eType].find( l_iRow ) == m_mapFrameVariables[p_eType].end() )
	{
		wxFrameVariableDialog l_dialog( this, m_pShaderProgram.lock(), p_eType );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			l_pFrameVariable = l_dialog.GetFrameVariable();

			if ( l_pFrameVariable )
			{
				m_mapFrameVariables[p_eType].insert( std::make_pair( l_iRow, l_pFrameVariable ) );
				wxArrayString l_arrayString;
				l_arrayString.push_back( l_pFrameVariable->GetName() + wxT( "=" ) + l_pFrameVariable->GetStrValue() );
				m_pListFrameVariables[p_eType]->InsertItems( l_arrayString, l_iRow );
			}
		}
	}
	else
	{
		l_pFrameVariable = m_mapFrameVariables[p_eType].find( l_iRow )->second.lock();
		wxFrameVariableDialog l_dialog( this, m_pShaderProgram.lock(), p_eType, l_pFrameVariable );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			m_pListFrameVariables[p_eType]->SetString( l_iRow, l_pFrameVariable->GetName() + wxT( "=" ) + l_pFrameVariable->GetStrValue() );
		}
	}
}

BEGIN_EVENT_TABLE( wxShaderDialog, wxFrame )
	EVT_CLOSE(	wxShaderDialog::OnClose	)
	EVT_LISTBOX_DCLICK(	eID_GRID_VERTEX,		wxShaderDialog::OnGridVertexCellChange	)
	EVT_LISTBOX_DCLICK(	eID_GRID_FRAGMENT,		wxShaderDialog::OnGridFragmentCellChange	)
	EVT_LISTBOX_DCLICK(	eID_GRID_GEOMETRY,		wxShaderDialog::OnGridGeometryCellChange	)
	EVT_LISTBOX_DCLICK(	eID_GRID_HULL,			wxShaderDialog::OnGridHullCellChange	)
	EVT_LISTBOX_DCLICK(	eID_GRID_DOMAIN,		wxShaderDialog::OnGridDomainCellChange	)
	EVT_MENU(	eID_MENU_OPEN,			wxShaderDialog::OnOpenFile	)
	EVT_MENU(	eID_MENU_SAVE_ONE,		wxShaderDialog::OnSaveFile	)
	EVT_MENU(	eID_MENU_SAVE_ALL,		wxShaderDialog::OnSaveAll	)
	EVT_MENU(	eID_MENU_QUIT,			wxShaderDialog::OnMenuClose	)
END_EVENT_TABLE()

void wxShaderDialog::OnOpenFile( wxCommandEvent & p_event )
{
	eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

	for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
	{
		if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
		{
			l_eType = eSHADER_TYPE( i );
		}
	}

	if ( l_eType != eSHADER_TYPE_COUNT )
	{
		DoFolder( l_eType );
	}

	p_event.Skip();
}

void wxShaderDialog::OnLoadShader( wxCommandEvent & p_event )
{
	DoLoadShader();
	p_event.Skip();
}

void wxShaderDialog::OnClose( wxCloseEvent & p_event )
{
	DoCleanup();
	p_event.Skip();
}

void wxShaderDialog::OnSaveFile( wxCommandEvent & p_event )
{
	eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

	for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
	{
		if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
		{
			l_eType = eSHADER_TYPE( i );
		}
	}

	if ( l_eType != eSHADER_TYPE_COUNT )
	{
		DoSave( l_eType, true );
	}

	p_event.Skip();
}

void wxShaderDialog::OnSaveAll( wxCommandEvent & p_event )
{
	eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

	for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
	{
		DoSave( eSHADER_TYPE( i ), false );
	}

	p_event.Skip();
}

void wxShaderDialog::OnMenuClose( wxCommandEvent & p_event )
{
	Close();
	p_event.Skip();
}

void wxShaderDialog::OnGridVertexCellChange( wxCommandEvent & p_event )
{
	DoGridCellChange( eSHADER_TYPE_VERTEX, p_event.GetInt() );
	p_event.Skip();
}

void wxShaderDialog::OnGridFragmentCellChange( wxCommandEvent & p_event )
{
	DoGridCellChange( eSHADER_TYPE_PIXEL, p_event.GetInt() );
	p_event.Skip();
}

void wxShaderDialog::OnGridGeometryCellChange( wxCommandEvent & p_event )
{
	DoGridCellChange( eSHADER_TYPE_GEOMETRY, p_event.GetInt() );
	p_event.Skip();
}

void wxShaderDialog::OnGridHullCellChange( wxCommandEvent & p_event )
{
	DoGridCellChange( eSHADER_TYPE_HULL, p_event.GetInt() );
	p_event.Skip();
}

void wxShaderDialog::OnGridDomainCellChange( wxCommandEvent & p_event )
{
	DoGridCellChange( eSHADER_TYPE_DOMAIN, p_event.GetInt() );
	p_event.Skip();
}
