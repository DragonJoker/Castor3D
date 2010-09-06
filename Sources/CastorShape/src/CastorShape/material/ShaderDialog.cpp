#include "PrecompiledHeaders.h"

#include "material/ShaderDialog.h"
#include "material/EnvironmentFrame.h"
#include "main/CastorShape.h"
#include "main/MainFrame.h"
#include "glsl/TextPanel.h"

using namespace Castor3D;
using namespace CastorShape;
DECLARE_APP( CSCastorShape)

ShaderDialog :: ShaderDialog( wxWindow * p_pParent, Pass * p_pPass, const wxPoint & p_ptPosition, const wxSize p_ptSize)
	:	wxDialog( p_pParent, wxID_ANY, "Shaders", p_ptPosition, p_ptSize),
		m_pPass( p_pPass),
		m_bCompiled( true),
		m_shaderProgram( NULL),
		m_bOwnShader( true)
{
	wxSize l_size = GetClientSize();

	m_vertexFolder = new wxButton( this, eVertex, C3D_T( "Vertex"), wxPoint( 10, 10), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_fragmentFolder = new wxButton( this, eFragment, C3D_T( "Fragment"), wxPoint( 10, 30), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_geometryFolder = new wxButton( this, eGeometry, C3D_T( "Geometry"), wxPoint( 10, 50), wxSize( 65, 20), wxBORDER_SIMPLE);

	m_pEditors = new wxNotebook( this, eEditors, wxPoint( 85, 0), wxSize( l_size.x - 95, l_size.y - 60), wxNB_FIXEDWIDTH);

	wxPanel * l_pTmpPanel = new wxPanel( m_pEditors, wxID_ANY, wxPoint( 0, 0));
	m_pEditors->AddPage( l_pTmpPanel, "Vertex", true);
	l_pTmpPanel->SetBackgroundColour( *wxWHITE);
	l_pTmpPanel->SetSize( 0, 22, m_pEditors->GetClientSize().x, m_pEditors->GetClientSize().y - 22);
	m_pVertexEditor = new TextPanel( l_pTmpPanel, wxID_ANY, wxPoint( 0, 0), l_pTmpPanel->GetClientSize(),
									 wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB, true);

	l_pTmpPanel = new wxPanel( m_pEditors, wxID_ANY, wxPoint( 0, 0));
	m_pEditors->AddPage( l_pTmpPanel, "Fragment", true);
	l_pTmpPanel->SetBackgroundColour( *wxWHITE);
	l_pTmpPanel->SetSize( 0, 22, m_pEditors->GetClientSize().x, m_pEditors->GetClientSize().y - 22);
	m_pFragmentEditor = new TextPanel( l_pTmpPanel, wxID_ANY, wxPoint( 0, 0), l_pTmpPanel->GetClientSize(),
									   wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB, true);

	l_pTmpPanel = new wxPanel( m_pEditors, wxID_ANY, wxPoint( 0, 0));
	m_pEditors->AddPage( l_pTmpPanel, "Geometry", true);
	l_pTmpPanel->SetBackgroundColour( *wxWHITE);
	l_pTmpPanel->SetSize( 0, 22, m_pEditors->GetClientSize().x, m_pEditors->GetClientSize().y - 22);
	m_pGeometryEditor = new TextPanel( l_pTmpPanel, wxID_ANY, wxPoint( 0, 0), l_pTmpPanel->GetClientSize(),
									   wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB, true);

	if (m_pPass->GetShader() != NULL)
	{
		m_bOwnShader = false;
		m_shaderProgram = m_pPass->GetShader();
	}

	if (m_shaderProgram == NULL)
	{
		m_shaderProgram = RenderSystem::GetSingletonPtr()->CreateShaderProgram( "", "", "");
	}
	else
	{
		m_strVertexShader = m_shaderProgram->GetVertexFile();
		m_strFragmentShader = m_shaderProgram->GetFragmentFile();

		m_pVertexEditor->LoadFile( m_shaderProgram->GetVertexFile(), m_pCompiler);
		m_pFragmentEditor->LoadFile( m_shaderProgram->GetFragmentFile(), m_pCompiler);

		if (m_shaderProgram->UsesGeometryShader())
		{
			m_strGeometryShader = m_shaderProgram->GetGeometryFile();
			m_pGeometryEditor->LoadFile( m_shaderProgram->GetGeometryFile(), m_pCompiler);
		}
	}
	
	wxArrayString l_arrayChoices;

	if (m_shaderProgram != NULL)
	{
		const std::map <String, UniformVariable *> l_mapVariables = m_shaderProgram->GetUniformVariables();
		int l_iCount = 0;

		for (std::map <String, UniformVariable *>::const_iterator l_it = l_mapVariables.begin() ; l_it != l_mapVariables.end() ; ++l_it)
		{
			l_arrayChoices.push_back( l_it->first);
			m_mapUniformVariables.insert( std::map <int, UniformVariable *>::value_type( l_iCount++, l_it->second));
		}
	}

	l_arrayChoices.push_back( C3D_T( "New..."));
	m_pListUniformVariables = new wxListBox( this, eGrid, wxPoint( 85, l_size.y - 60), wxSize( l_size.x - 95, 60), l_arrayChoices, wxBORDER_SIMPLE | wxWANTS_CHARS);

	m_loadShader = new wxButton( this, eLoadShader, C3D_T( "Appliquer"), wxPoint( 10, l_size.y - 75), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonOk = new wxButton( this, eOK, C3D_T( "OK"), wxPoint( 10, l_size.y - 50), wxSize( 65, 20), wxBORDER_SIMPLE);
	m_pButtonCancel = new wxButton( this, eCancel, C3D_T( "Annuler"), wxPoint( 10, l_size.y - 25), wxSize( 65, 20), wxBORDER_SIMPLE);

	m_pCompiler = new ScriptCompiler( "");
}

ShaderDialog :: ~ShaderDialog()
{
}

void ShaderDialog :: _cleanup()
{
	if (m_bOwnShader && m_shaderProgram != NULL)
	{
		m_pPass->SetShader( NULL);
		ShaderManager::GetSingletonPtr()->RemoveProgram( m_shaderProgram);
		m_shaderProgram = NULL;
	}
}

void ShaderDialog :: _loadShader()
{
	if (m_strVertexShader.empty())
	{
		wxMessageBox( C3D_T( "Remplissez le nom du vertex shader"), C3D_T( "ERROR"));
		return;
	}

	m_pVertexEditor->SaveFile( m_strVertexShader, m_pCompiler);

	if (m_strFragmentShader.empty())
	{
		wxMessageBox( C3D_T( "Remplissez le nom du fragment shader"), C3D_T( "ERROR"));
		return;
	}

	m_pFragmentEditor->SaveFile( m_strFragmentShader, m_pCompiler);

	if ( ! m_strGeometryShader.empty())
	{
		m_pGeometryEditor->SaveFile( m_strGeometryShader, m_pCompiler);
	}

	if (m_shaderProgram == NULL)
	{
		if (m_strGeometryShader.empty())
		{
			m_shaderProgram = RenderSystem::GetSingletonPtr()->CreateShaderProgram( m_strVertexShader.c_str(), m_strFragmentShader.c_str(), C3DEmptyString);
		}
		else
		{
			m_shaderProgram = RenderSystem::GetSingletonPtr()->CreateShaderProgram( m_strVertexShader.c_str(), m_strFragmentShader.c_str(), m_strGeometryShader.c_str());
		}
	}
	else
	{
		m_shaderProgram->SetVertexFileIO( m_strVertexShader.c_str());
		m_shaderProgram->SetFragmentFileIO( m_strFragmentShader.c_str());

		if ( ! m_strGeometryShader.empty())
		{
			m_shaderProgram->UsesGeometryShader( true);
			m_shaderProgram->SetGeometryFileIO( m_strGeometryShader.c_str());
		}
		else
		{
			m_shaderProgram->UsesGeometryShader( false);
		}
	}

	for (std::map <int, UniformVariable *>::iterator l_it = m_mapUniformVariables.begin() ; l_it != m_mapUniformVariables.end() ; ++l_it)
	{
		m_shaderProgram->AddUniformVariable( l_it->second);
	}

	m_pPass->SetShader( m_shaderProgram);

	m_bCompiled = true;
}

BEGIN_EVENT_TABLE( ShaderDialog, wxDialog)
	EVT_BUTTON(					eVertex,		ShaderDialog::_onVertexFolder)
	EVT_BUTTON(					eFragment,		ShaderDialog::_onFragmentFolder)
	EVT_BUTTON(					eGeometry,		ShaderDialog::_onGeometryFolder)
	EVT_BUTTON(					eLoadShader,	ShaderDialog::_onLoadShader)
	EVT_CLOSE(									ShaderDialog::_onClose)
	EVT_BUTTON(					eOK,			ShaderDialog::_onOk)
	EVT_BUTTON(					eCancel,		ShaderDialog::_onCancel)
	EVT_LISTBOX_DCLICK(			eGrid,			ShaderDialog::_onGridCellChange)
END_EVENT_TABLE()

void ShaderDialog :: _onVertexFolder( wxCommandEvent & event)
{
	wxFileDialog l_dialog( this, C3D_T( "Select a glsl vertex shader file"), wxEmptyString, wxEmptyString, C3D_T( ""));

	if (l_dialog.ShowModal() == wxID_OK)
	{
		m_strVertexShader = l_dialog.GetPath();
		m_pVertexEditor->LoadFile( m_strVertexShader, m_pCompiler);
		m_bCompiled = false;
	}
}

void ShaderDialog :: _onFragmentFolder( wxCommandEvent & event)
{
	wxFileDialog l_dialog( this, C3D_T( "Select a glsl fragment shader file"), wxEmptyString, wxEmptyString, C3D_T( ""));

	if (l_dialog.ShowModal() == wxID_OK)
	{
		m_strFragmentShader = l_dialog.GetPath();
		m_pFragmentEditor->LoadFile( m_strFragmentShader, m_pCompiler);
		m_bCompiled = false;
	}
}

void ShaderDialog :: _onGeometryFolder( wxCommandEvent & event)
{
	wxFileDialog l_dialog( this, C3D_T( "Select a glsl geometry shader file"), wxEmptyString, wxEmptyString, C3D_T( ""));

	if (l_dialog.ShowModal() == wxID_OK)
	{
		m_strGeometryShader = l_dialog.GetPath();
		m_pGeometryEditor->LoadFile( m_strGeometryShader, m_pCompiler);
		m_bCompiled = false;
	}
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

void ShaderDialog :: _onGridCellChange( wxCommandEvent & event)
{
	int l_iRow = event.GetInt();
	UniformVariable * l_pUniformVariable = NULL;

	if (m_mapUniformVariables.find( l_iRow) == m_mapUniformVariables.end())
	{
		UniformVariableDialog l_dialog( this);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			l_pUniformVariable = l_dialog.GetUniformVariable();

			if (l_pUniformVariable != NULL)
			{
				m_mapUniformVariables.insert( std::map <int, UniformVariable *>::value_type( l_iRow, l_pUniformVariable));
				wxArrayString l_arrayString;
				l_arrayString.push_back( l_pUniformVariable->GetName() + C3D_T( "=") + l_pUniformVariable->GetStrValue());
				m_pListUniformVariables->InsertItems( l_arrayString, l_iRow);
			}
		}
	}
	else
	{
		l_pUniformVariable = m_mapUniformVariables.find( l_iRow)->second;
		UniformVariableDialog l_dialog( this, l_pUniformVariable);

		if (l_dialog.ShowModal() == wxID_OK)
		{
			m_pListUniformVariables->SetString( l_iRow, l_pUniformVariable->GetName() + C3D_T( "=") + l_pUniformVariable->GetStrValue());
		}
	}
}