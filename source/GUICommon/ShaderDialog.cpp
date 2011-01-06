#include "GUICommon/PrecompiledHeader.h"

#include "GUICommon/ShaderDialog.h"
#include "GUICommon/EnvironmentFrame.h"
#include "GUICommon/TextPanel.h"

using namespace Castor3D;
using namespace GUICommon;

ShaderDialog :: ShaderDialog( wxWindow * p_pParent, ShaderManager * p_pManager, PassPtr p_pPass, const wxPoint & p_ptPosition, const wxSize p_ptSize)
	:	wxDialog( p_pParent, wxID_ANY, CU_T( "Shaders"), p_ptPosition, p_ptSize)
	,	m_pPass( p_pPass)
	,	m_bCompiled( true)
	,	m_bOwnShader( true)
	,	m_pManager( p_pManager)
{
	wxSize l_size = GetClientSize();
	wxArrayString l_arrayChoices;
	wxArrayString l_arrayTexts;
	l_arrayTexts.push_back( wxT( "Vertex"));
	l_arrayTexts.push_back( wxT( "Pixel"));
	l_arrayTexts.push_back( wxT( "Geometry"));

	m_pEditorsBook = new wxNotebook( this, eEditors, wxPoint( 85, 0), wxSize( l_size.x - 95, l_size.y), wxNB_FIXEDWIDTH);

	wxPoint l_ptButtonPosition( 10, 10);

	for (int i = 0 ; i < eNbShaderTypes ; i++)
	{
		m_pFolders[i] = new wxButton( this, eVertex + i, l_arrayTexts[i], l_ptButtonPosition, wxSize( 65, 20), wxBORDER_SIMPLE);

		wxPanel * l_pTmpPanel = new wxPanel( m_pEditorsBook, wxID_ANY, wxPoint( 0, 0));
		m_pEditorsBook->AddPage( l_pTmpPanel, l_arrayTexts[i], true);
		l_pTmpPanel->SetBackgroundColour( * wxWHITE);
		l_pTmpPanel->SetSize( 0, 22, m_pEditorsBook->GetClientSize().x, m_pEditorsBook->GetClientSize().y - 22);
		m_pEditors[i] = new TextPanel( l_pTmpPanel, wxID_ANY, wxPoint( 0, 0), l_pTmpPanel->GetClientSize() - wxSize( 0, 60),
									   wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB, true);
		if (m_shaderProgram->GetProgram( eSHADER_PROGRAM_TYPE( i)).use_count() > 0)
		{
			const FrameVariablePtrStrMap & l_mapVariables = m_shaderProgram->GetProgram( eSHADER_PROGRAM_TYPE( i))->GetFrameVariables();
			int l_iCount = 0;

			for (FrameVariablePtrStrMap::const_iterator l_it = l_mapVariables.begin() ; l_it != l_mapVariables.end() ; ++l_it)
			{
				l_arrayChoices.push_back( l_it->first);
				m_mapFrameVariables[i].insert( KeyedContainer< int, Castor3D::FrameVariablePtr>::Map::value_type( l_iCount++, l_it->second));
			}
		}

		l_arrayChoices.push_back( CU_T( "New..."));
		m_pListsFrameVariables[i] = new wxListBox( l_pTmpPanel, eGridVertex + i, wxPoint( 0, l_pTmpPanel->GetClientSize().y - 60), wxSize( l_pTmpPanel->GetClientSize().x, 60), l_arrayChoices, wxBORDER_SIMPLE | wxWANTS_CHARS);

		l_ptButtonPosition.y += 20;
	}

	if ( ! m_pPass->GetShader<ShaderProgram>() == NULL)
	{
		m_bOwnShader = false;
		m_shaderProgram = m_pPass->GetShader<ShaderProgram>();
	}

	if (m_shaderProgram == NULL)
	{
		m_shaderProgram = m_pManager->CreateShaderProgramFromFiles( C3DEmptyString, C3DEmptyString, C3DEmptyString);
	}
	else
	{
		m_strShaders[eVertexShader] = m_shaderProgram->GetProgram( eVertexShader)->GetFile();
		m_pEditors[eVertexShader]->LoadFile( m_shaderProgram->GetProgram( eVertexShader)->GetFile(), NULL);

		m_strShaders[ePixelShader] = m_shaderProgram->GetProgram( ePixelShader)->GetFile();
		m_pEditors[ePixelShader]->LoadFile( m_shaderProgram->GetProgram( ePixelShader)->GetFile(), NULL);

		if (m_shaderProgram->UsesGeometryShader())
		{
			m_strShaders[eGeometryShader] = m_shaderProgram->GetProgram( eGeometryShader)->GetFile();
			m_pEditors[eGeometryShader]->LoadFile( m_shaderProgram->GetProgram( eGeometryShader)->GetFile(), NULL);
		}
	}


	if ( ! m_shaderProgram == NULL)
	{
		for (int i = 0 ; i < eNbShaderTypes ; i++)
		{
		}
	}

	m_loadShader = new wxButton( this, eLoadShader, CU_T( "Appliquer"), wxPoint( 10, l_size.y - 75), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonOk = new wxButton( this, eOK, CU_T( "OK"), wxPoint( 10, l_size.y - 50), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonCancel = new wxButton( this, eCancel, CU_T( "Annuler"), wxPoint( 10, l_size.y - 25), wxSize( 65, 20), wxBORDER_SIMPLE);
}

ShaderDialog :: ~ShaderDialog()
{
}

void ShaderDialog :: _cleanup()
{
	if (m_bOwnShader && ! m_shaderProgram == NULL)
	{
		m_pPass->SetShader( ShaderProgramPtr());
		m_pManager->RemoveProgram( m_shaderProgram);
		m_shaderProgram.reset();
	}
}

void ShaderDialog :: _loadShader()
{
	if (m_strShaders[eVertexShader].empty())
	{
		wxMessageBox( CU_T( "Remplissez le nom du vertex shader"), CU_T( "ERROR"));
		return;
	}

	m_pEditors[eVertexShader]->SaveFile( m_strShaders[eVertexShader], NULL);

	if (m_strShaders[ePixelShader].empty())
	{
		wxMessageBox( CU_T( "Remplissez le nom du fragment shader"), CU_T( "ERROR"));
		return;
	}

	m_pEditors[ePixelShader]->SaveFile( m_strShaders[ePixelShader], NULL);

	if ( ! m_strShaders[eGeometryShader].empty())
	{
		m_pEditors[eGeometryShader]->SaveFile( m_strShaders[eGeometryShader], NULL);
	}

	if (m_shaderProgram == NULL)
	{
		m_shaderProgram = m_pManager->CreateShaderProgramFromFiles( m_strShaders[eVertexShader].c_str(), m_strShaders[ePixelShader].c_str(), m_strShaders[eGeometryShader].c_str());
	}
	else
	{
		m_shaderProgram->SetVertexFile( m_strShaders[eVertexShader].c_str());
		m_shaderProgram->SetFragmentFile( m_strShaders[ePixelShader].c_str());

		if ( ! m_strShaders[eGeometryShader].empty())
		{
			m_shaderProgram->UsesGeometryShader( RenderSystem::HasGeometryShader());
			m_shaderProgram->SetGeometryFile( m_strShaders[eGeometryShader].c_str());
		}
		else
		{
			m_shaderProgram->UsesGeometryShader( false);
		}
	}

	for (int i = 0 ; i < eNbShaderTypes ; i++)
	{
		for (KeyedContainer< int, Castor3D::FrameVariablePtr>::Map::iterator l_it = m_mapFrameVariables[i].begin() ; l_it != m_mapFrameVariables[i].end() ; ++l_it)
		{
			m_shaderProgram->AddFrameVariable( l_it->second, eSHADER_PROGRAM_TYPE( i));
		}
	}

	m_pPass->SetShader( m_shaderProgram);

	m_bCompiled = true;
}

void ShaderDialog :: _onFolder( eSHADER_PROGRAM_TYPE p_eType)
{
	wxFileDialog l_dialog( this, CU_T( "Select a glsl program shader file"), wxEmptyString, wxEmptyString, C3DEmptyString);

	if (l_dialog.ShowModal() == wxID_OK)
	{
		m_strShaders[p_eType] = l_dialog.GetPath();
		m_pEditors[p_eType]->LoadFile( m_strShaders[p_eType], NULL);
		m_bCompiled = false;
	}
}

void ShaderDialog :: _onGridCellChange( eSHADER_PROGRAM_TYPE p_eType, int p_iRow)
{
	int l_iRow = p_iRow;
	FrameVariablePtr l_pFrameVariable;

	if (m_mapFrameVariables[p_eType].find( l_iRow) == m_mapFrameVariables[p_eType].end())
	{
		FrameVariableDialog l_dialog( this);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			l_pFrameVariable = l_dialog.GetFrameVariable();

			if ( ! l_pFrameVariable == NULL)
			{
				m_mapFrameVariables[p_eType].insert( KeyedContainer< int, Castor3D::FrameVariablePtr>::Map::value_type( l_iRow, l_pFrameVariable));
				wxArrayString l_arrayString;
				l_arrayString.push_back( l_pFrameVariable->GetName() + "=" + l_pFrameVariable->GetStrValue());
				m_pListsFrameVariables[p_eType]->InsertItems( l_arrayString, l_iRow);
			}
		}
	}
	else
	{
		l_pFrameVariable = m_mapFrameVariables[p_eType].find( l_iRow)->second;
		FrameVariableDialog l_dialog( this, l_pFrameVariable);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			m_pListsFrameVariables[p_eType]->SetString( l_iRow, l_pFrameVariable->GetName() + "=" + l_pFrameVariable->GetStrValue());
		}
	}
}

BEGIN_EVENT_TABLE( ShaderDialog, wxDialog)
	EVT_BUTTON(					eVertex,		ShaderDialog::_onVertexFolder)
	EVT_BUTTON(					eFragment,		ShaderDialog::_onFragmentFolder)
	EVT_BUTTON(					eGeometry,		ShaderDialog::_onGeometryFolder)
	EVT_BUTTON(					eLoadShader,	ShaderDialog::_onLoadShader)
	EVT_CLOSE(									ShaderDialog::_onClose)
	EVT_BUTTON(					eOK,			ShaderDialog::_onOk)
	EVT_BUTTON(					eCancel,		ShaderDialog::_onCancel)
	EVT_LISTBOX_DCLICK(			eGridVertex,	ShaderDialog::_onGridVertexCellChange)
	EVT_LISTBOX_DCLICK(			eGridFragment,	ShaderDialog::_onGridFragmentCellChange)
	EVT_LISTBOX_DCLICK(			eGridGeometry,	ShaderDialog::_onGridGeometryCellChange)
END_EVENT_TABLE()

void ShaderDialog :: _onVertexFolder( wxCommandEvent & event)
{
	_onFolder( eVertexShader);
}

void ShaderDialog :: _onFragmentFolder( wxCommandEvent & event)
{
	_onFolder( ePixelShader);
}

void ShaderDialog :: _onGeometryFolder( wxCommandEvent & event)
{
	_onFolder( eGeometryShader);
}

void ShaderDialog :: _onLoadShader( wxCommandEvent & event)
{
	_loadShader();
}

void ShaderDialog :: _onClose( wxCloseEvent & event)
{
	_cleanup();
	EndModal( wxID_CANCEL);
}

void ShaderDialog :: _onOk( wxCommandEvent& event)
{
	_loadShader();
	EndModal( wxID_OK);
}

void ShaderDialog :: _onCancel( wxCommandEvent& event)
{
	_cleanup();
	EndModal( wxID_CANCEL);
}

void ShaderDialog :: _onGridVertexCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eVertexShader, event.GetInt());
}

void ShaderDialog :: _onGridFragmentCellChange( wxCommandEvent & event)
{
	_onGridCellChange( ePixelShader, event.GetInt());
}

void ShaderDialog :: _onGridGeometryCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eGeometryShader, event.GetInt());
}