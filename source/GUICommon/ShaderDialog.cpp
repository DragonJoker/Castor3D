#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/ShaderDialog.hpp"
#include "GuiCommon/TextPanel.hpp"

using namespace Castor3D;
using namespace GuiCommon;

wxShaderDialog :: wxShaderDialog( wxWindow * p_pParent, PassPtr p_pPass, const wxPoint & p_ptPosition, const wxSize p_ptSize)
	:	wxDialog( p_pParent, wxID_ANY, wxT( "Shaders"), p_ptPosition, p_ptSize)
	,	m_pPass( p_pPass)
	,	m_bCompiled( true)
	,	m_bOwnShader( true)
{
	if (m_pPass->GetShader<GlslShaderProgram>())
	{
		m_bOwnShader = false;
		m_shaderProgram = m_pPass->GetShader<GlslShaderProgram>();
	}

	if ( ! m_shaderProgram)
	{
		m_shaderProgram = RenderSystem::CreateShaderProgram<GlslShaderProgram>();
	}

	wxSize l_size = GetClientSize();
	wxArrayString l_arrayChoices;
	wxArrayString l_arrayTexts;
	l_arrayTexts.push_back( wxT( "Vertex"));
	l_arrayTexts.push_back( wxT( "Pixel"));
	l_arrayTexts.push_back( wxT( "Geometry"));
	l_arrayTexts.push_back( wxT( "Hull"));
	l_arrayTexts.push_back( wxT( "Domain"));

	m_pEditorsBook = new wxNotebook( this, eID_NOTEBOOK_EDITORS, wxPoint( 85, 0), wxSize( l_size.x - 95, l_size.y), wxNB_FIXEDWIDTH);

	wxPoint l_ptButtonPosition( 10, 10);

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		m_pFolders[i] = new wxButton( this, eID_BUTTON_VERTEX + i, l_arrayTexts[i], l_ptButtonPosition, wxSize( 65, 20), wxBORDER_SIMPLE);

		wxPanel * l_pTmpPanel = new wxPanel( m_pEditorsBook, wxID_ANY, wxPoint( 0, 0));
		m_pEditorsBook->AddPage( l_pTmpPanel, l_arrayTexts[i], true);
		l_pTmpPanel->SetBackgroundColour( * wxWHITE);
		l_pTmpPanel->SetSize( 0, 22, m_pEditorsBook->GetClientSize().x, m_pEditorsBook->GetClientSize().y - 22);
		m_pEditors[i] = new wxTextPanel( l_pTmpPanel, wxID_ANY, wxPoint( 0, 0), l_pTmpPanel->GetClientSize() - wxSize( 0, 60), wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB, true);
		ShaderObjectPtr l_pObject = m_shaderProgram->GetProgram( eSHADER_TYPE( i));

		if (l_pObject)
		{
			int l_iCount = 0;

			for (FrameVariablePtrList::const_iterator l_it = l_pObject->GetFrameVariablesBegin() ; l_it != l_pObject->GetFrameVariablesEnd() ; ++l_it)
			{
				l_arrayChoices.push_back( (* l_it)->GetName());
				m_mapFrameVariables[i].insert( KeyedContainer< int, Castor3D::FrameVariablePtr>::Map::value_type( l_iCount++, ( * l_it)));
			}

			m_strShaders[i] = l_pObject->GetFile();
			m_pEditors[i]->LoadFile( m_strShaders[i], nullptr);
		}

		l_arrayChoices.push_back( wxT( "New..."));
		m_pListsFrameVariables[i] = new wxListBox( l_pTmpPanel, eID_GRID_VERTEX + i, wxPoint( 0, l_pTmpPanel->GetClientSize().y - 60), wxSize( l_pTmpPanel->GetClientSize().x, 60), l_arrayChoices, wxBORDER_SIMPLE | wxWANTS_CHARS);

		l_ptButtonPosition.y += 20;
	}

	m_loadShader = new wxButton( this, eID_BUTTON_LOAD, cuT( "Load"), wxPoint( 10, l_size.y - 75), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonOk = new wxButton( this, eID_BUTTON_OK, cuT( "OK"), wxPoint( 10, l_size.y - 50), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonCancel = new wxButton( this, eID_BUTTON_CANCEL, cuT( "Cancel"), wxPoint( 10, l_size.y - 25), wxSize( 65, 20), wxBORDER_SIMPLE);
}

wxShaderDialog :: ~wxShaderDialog()
{
}

void wxShaderDialog :: _cleanup()
{
	if (m_bOwnShader && m_shaderProgram)
	{
		m_pPass->SetShader( ShaderProgramPtr());
		ShaderManager l_shaderManager;
		l_shaderManager.RemoveProgram( m_shaderProgram);
		m_shaderProgram.reset();
	}
}

void wxShaderDialog :: _loadShader()
{
	if (m_strShaders[eSHADER_TYPE_VERTEX].empty())
	{
		wxMessageBox( wxT( "Remplissez le nom du vertex shader"), wxT( "ERROR"));
		return;
	}

	if (m_strShaders[eSHADER_TYPE_PIXEL].empty())
	{
		wxMessageBox( wxT( "Remplissez le nom du fragment shader"), wxT( "ERROR"));
		return;
	}

	if ( ! m_shaderProgram)
	{
		m_shaderProgram = RenderSystem::CreateShaderProgram<GlslShaderProgram>();
	}

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		if ( ! m_strShaders[i].empty())
		{
			m_pEditors[i]->SaveFile( m_strShaders[i], nullptr);
			m_shaderProgram->CreateObject( eSHADER_TYPE( i));
			m_shaderProgram->SetProgramFile( (const wxChar *)m_strShaders[i].c_str(), eSHADER_TYPE( i));
		}

		for (KeyedContainer< int, Castor3D::FrameVariablePtr>::Map::iterator l_it = m_mapFrameVariables[i].begin() ; l_it != m_mapFrameVariables[i].end() ; ++l_it)
		{
//			m_shaderProgram->AddFrameVariable( l_it->second, l_it->second->GetName(), eSHADER_TYPE( i));
		}
	}

	m_pPass->SetShader( m_shaderProgram);

	m_bCompiled = true;
}

void wxShaderDialog :: _onFolder( eSHADER_TYPE p_eType)
{
	wxFileDialog l_dialog( this, cuT( "Select a program shader file"), wxEmptyString, wxEmptyString, cuEmptyString);

	if (l_dialog.ShowModal() == wxID_OK)
	{
		m_strShaders[p_eType] = l_dialog.GetPath();
		m_pEditors[p_eType]->LoadFile( m_strShaders[p_eType], nullptr);
		m_bCompiled = false;
	}
}

void wxShaderDialog :: _onGridCellChange( eSHADER_TYPE p_eType, int p_iRow)
{
	int l_iRow = p_iRow;
	FrameVariablePtr l_pFrameVariable;

	if (m_mapFrameVariables[p_eType].find( l_iRow) == m_mapFrameVariables[p_eType].end())
	{
		wxFrameVariableDialog l_dialog( this, m_shaderProgram, m_shaderObject);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			l_pFrameVariable = l_dialog.GetFrameVariable();

			if (l_pFrameVariable)
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
		wxFrameVariableDialog l_dialog( this, m_shaderProgram, m_shaderObject, l_pFrameVariable);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			m_pListsFrameVariables[p_eType]->SetString( l_iRow, l_pFrameVariable->GetName() + wxT( "=") + l_pFrameVariable->GetStrValue());
		}
	}
}

BEGIN_EVENT_TABLE( wxShaderDialog, wxDialog)
	EVT_BUTTON(					eID_BUTTON_VERTEX,		wxShaderDialog::_onVertexFolder)
	EVT_BUTTON(					eID_BUTTON_FRAGMENT,	wxShaderDialog::_onFragmentFolder)
	EVT_BUTTON(					eID_BUTTON_GEOMETRY,	wxShaderDialog::_onGeometryFolder)
	EVT_BUTTON(					eID_BUTTON_HULL,		wxShaderDialog::_onHullFolder)
	EVT_BUTTON(					eID_BUTTON_DOMAIN,		wxShaderDialog::_onDomainFolder)
	EVT_BUTTON(					eID_BUTTON_LOAD,		wxShaderDialog::_onLoadShader)
	EVT_BUTTON(					eID_BUTTON_OK,			wxShaderDialog::_onOk)
	EVT_BUTTON(					eID_BUTTON_CANCEL,		wxShaderDialog::_onCancel)
	EVT_CLOSE(											wxShaderDialog::_onClose)
	EVT_LISTBOX_DCLICK(			eID_GRID_VERTEX,		wxShaderDialog::_onGridVertexCellChange)
	EVT_LISTBOX_DCLICK(			eID_GRID_FRAGMENT,		wxShaderDialog::_onGridFragmentCellChange)
	EVT_LISTBOX_DCLICK(			eID_GRID_GEOMETRY,		wxShaderDialog::_onGridGeometryCellChange)
	EVT_LISTBOX_DCLICK(			eID_GRID_HULL,			wxShaderDialog::_onGridHullCellChange)
	EVT_LISTBOX_DCLICK(			eID_GRID_DOMAIN,		wxShaderDialog::_onGridDomainCellChange)
END_EVENT_TABLE()

void wxShaderDialog :: _onVertexFolder( wxCommandEvent & event)
{
	_onFolder( eSHADER_TYPE_VERTEX);
}

void wxShaderDialog :: _onFragmentFolder( wxCommandEvent & event)
{
	_onFolder( eSHADER_TYPE_PIXEL);
}

void wxShaderDialog :: _onGeometryFolder( wxCommandEvent & event)
{
	_onFolder( eSHADER_TYPE_GEOMETRY);
}

void wxShaderDialog :: _onHullFolder( wxCommandEvent & event)
{
	_onFolder( eSHADER_TYPE_HULL);
}

void wxShaderDialog :: _onDomainFolder( wxCommandEvent & event)
{
	_onFolder( eSHADER_TYPE_DOMAIN);
}

void wxShaderDialog :: _onLoadShader( wxCommandEvent & event)
{
	_loadShader();
}

void wxShaderDialog :: _onClose( wxCloseEvent & event)
{
	_cleanup();
	EndModal( wxID_CANCEL);
	event.Skip();
}

void wxShaderDialog :: _onOk( wxCommandEvent& event)
{
	_loadShader();
	EndModal( wxID_OK);
	event.Skip();
}

void wxShaderDialog :: _onCancel( wxCommandEvent& event)
{
	_cleanup();
	EndModal( wxID_CANCEL);
	event.Skip();
}

void wxShaderDialog :: _onGridVertexCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eSHADER_TYPE_VERTEX, event.GetInt());
	event.Skip();
}

void wxShaderDialog :: _onGridFragmentCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eSHADER_TYPE_PIXEL, event.GetInt());
	event.Skip();
}

void wxShaderDialog :: _onGridGeometryCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eSHADER_TYPE_GEOMETRY, event.GetInt());
	event.Skip();
}

void wxShaderDialog :: _onGridHullCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eSHADER_TYPE_HULL, event.GetInt());
	event.Skip();
}

void wxShaderDialog :: _onGridDomainCellChange( wxCommandEvent & event)
{
	_onGridCellChange( eSHADER_TYPE_DOMAIN, event.GetInt());
	event.Skip();
}
