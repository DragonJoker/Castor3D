#include "PrecompiledHeader.h"

#include "MaterialPanel.h"
#include "PassPanel.h"
#include "EnvironmentFrame.h"
#include "MainFrame.h"
#include "CastorViewer.h"

using namespace Castor3D;
using namespace CastorViewer;

DECLARE_APP( CastorViewerApp)

MaterialPanel :: MaterialPanel( wxWindow * parent, const wxPoint & pos, const wxSize & size)
	:	wxPanel( parent, wxID_ANY, pos, size, 524288 | wxBORDER_NONE),
		m_material( NULL),
		m_selectedPass( NULL),
		m_selectedPassIndex( -1),
		m_selectedPassPanel( NULL)
{
}

MaterialPanel :: ~MaterialPanel()
{
	m_selectedPass = NULL;
}

void MaterialPanel :: CreateMaterialPanel( const String & p_materialName)
{
	_createMaterialPanel( (p_materialName.size() > 0 ? p_materialName.c_str() : NULL));
}

int MaterialPanel :: GetPassIndex()const
{
	wxString l_value = m_passSelector->GetValue();
	Log::LogMessage( C3D_T( "GetPassIndex - l_value : %s"), l_value.c_str());
	
	if (l_value.IsNumber())
	{
		int l_res = atoi( l_value.char_str());
		Log::LogMessage( "GetPassIndex - l_res : %d", l_res);
		return l_res;
	}
	if (l_value == C3D_T( "New..."))
	{
		return -1;
	}
	return -2;
}

BEGIN_EVENT_TABLE( MaterialPanel, wxPanel)
	EVT_BUTTON(		eDeletePass,		MaterialPanel::_onDeletePass)
	EVT_TEXT_ENTER( eMaterialName,		MaterialPanel::_onMaterialName)
	EVT_COMBOBOX(	ePass,				MaterialPanel::_onPassSelect)
END_EVENT_TABLE()

void MaterialPanel :: _onMaterialName( wxCommandEvent & event)
{
	String l_name = m_materialName->GetValue().c_str();
	m_material->SetName( l_name);
	m_material->Initialise();
	wxGetApp().GetMainFrame()->ShowPanels();
}

void MaterialPanel :: _onDeletePass( wxCommandEvent & event)
{
	if (m_material != NULL && m_material->GetNbPasses() > 1)
	{
		m_passSelector->Clear();
		m_material->DestroyPass( m_selectedPassIndex);
		m_material->Initialise();
		unsigned int i;
		wxString l_name;
		for (i = 0 ; i < m_material->GetNbPasses() ; i++)
		{
			l_name.clear();
			l_name << i;
			m_passSelector->Insert( l_name, i);
		}
		l_name = C3D_T( "New...");
		m_passSelector->Insert( l_name, i);
		m_passSelector->SetValue( C3D_T( "New..."));
		m_passSelector->Update();
		this->RemoveChild( m_selectedPassPanel);
		wxGetApp().GetMainFrame()->ShowPanels();
		if (m_material->GetNbPasses() == 1)
		{
			m_deletePass->Disable();
		}
	}
}

void MaterialPanel :: _onPassSelect( wxCommandEvent & event)
{
	m_selectedPassIndex = GetPassIndex();
	_createPassPanel();
}

void MaterialPanel :: _createMaterialPanel( const Char * p_materialName)
{
	if (m_material)
	{
		m_material->Initialise();
		m_material = NULL;
	}
	m_selectedPass = NULL;
	m_material = MaterialManager::GetSingletonPtr()->GetElementByName( (p_materialName != NULL ? p_materialName : C3DEmptyString));
	if ( ! DestroyChildren() || m_material == NULL)
	{
		return;
	}
	m_selectedPass = m_material->GetPass( 0);
	m_selectedPassIndex = 0;
	new wxStaticText( this, wxID_ANY, C3D_T( "Nom : "), wxPoint( 10, 5));
	m_materialName = new wxTextCtrl( this, eMaterialName, String( p_materialName), wxPoint( 50, 2), wxSize( 100, 20), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	wxSize l_size = GetClientSize();

	l_size.x -= 20;
	l_size.y -= 30;
	wxStaticBox * l_passBox = new wxStaticBox( this, wxID_ANY, C3D_T( "Passes"), wxPoint( 10, 30), l_size);

	wxArrayString l_names;
	wxString l_name;
	for (unsigned int i = 0 ; i < m_material->GetNbPasses() ; i++)
	{
		l_name.clear();
		l_name << i;
		l_names.push_back( l_name);
	}
	l_names.push_back( C3D_T( "New..."));
	m_passSelector = new wxComboBox( this, ePass, C3D_T( "0"), wxPoint( 20, 45), wxSize( 50, 20), l_names, wxBORDER_SIMPLE | wxCB_READONLY);
	m_deletePass = new wxButton( this, eDeletePass, C3D_T( "Supprimer"), wxPoint( 80, 44), wxSize( 80, 23), wxBORDER_SIMPLE);
	m_passesPanel = new wxPanel( this, wxID_ANY, wxPoint( 15, 70), wxSize( l_passBox->GetClientSize().x - 10, l_passBox->GetClientSize().y - 30));
	m_selectedPassPanel = new PassPanel( m_passesPanel, wxPoint( 0, 0), m_passesPanel->GetClientSize());
	_createPassPanel();
}

void MaterialPanel :: _createPassPanel()
{
	if (m_selectedPassIndex == -1)
	{
		m_selectedPass = m_material->CreatePass();
		wxString l_value;
		l_value << (m_material->GetNbPasses() - 1);
		m_passSelector->Insert( l_value, m_material->GetNbPasses() - 1);
		m_passSelector->Update();
		m_passSelector->SetValue( l_value);
	}
	else
	{
		m_selectedPass = m_material->GetPass( m_selectedPassIndex);
	}
	m_selectedPassPanel->CreatePassPanel( m_selectedPass);
}