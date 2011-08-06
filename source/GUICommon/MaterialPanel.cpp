#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/MaterialPanel.hpp"
#include "GuiCommon/PassPanel.hpp"

using namespace Castor3D;
using namespace GuiCommon;

wxMaterialPanel :: wxMaterialPanel( wxWindow * parent, const wxPoint & pos, const wxSize & size)
	:	wxPanel( parent, wxID_ANY, pos, size, 524288 | wxBORDER_NONE)
	,	m_selectedPassIndex( -1)
	,	m_selectedPassPanel( NULL)
{
}

wxMaterialPanel :: ~wxMaterialPanel()
{
}

void wxMaterialPanel :: CreateMaterialPanel( String const & p_materialName)
{
	_createMaterialPanel( p_materialName);
}

int wxMaterialPanel :: GetPassIndex()const
{
	wxString l_value = m_passSelector->GetValue();
	Logger::LogMessage( cuT( "GetPassIndex - l_value : %s"), (const wxChar *)l_value.c_str());
	int l_iReturn = -2;

	if (l_value.IsNumber())
	{
		long l_res;
		l_value.ToLong( & l_res);
		Logger::LogMessage( cuT( "GetPassIndex - l_res : %i"), l_res);
		l_iReturn = l_res;
	}
	else if (l_value == wxT( "New..."))
	{
		l_iReturn = -1;
	}

	return l_iReturn;
}

void wxMaterialPanel :: _createMaterialPanel( const wxString & p_materialName)
{
	if (m_material)
	{
		m_material->Initialise();
		m_material.reset();
	}

	m_selectedPass.reset();
	Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
	m_material = l_pManager->GetElement( (const wxChar *)p_materialName.c_str());

	if ( ! DestroyChildren() || ! m_material)
	{
		return;
	}

	m_selectedPass = m_material->GetPass( 0);
	m_selectedPassIndex = 0;
	new wxStaticText( this, wxID_ANY, wxT( "Name : "), wxPoint( 10, 5));
	m_materialName = new wxTextCtrl( this, eID_EDIT_MATERIAL_NAME, p_materialName, wxPoint( 50, 2), wxSize( 100, 20), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	wxSize l_size = GetClientSize();

	l_size.x -= 20;
	l_size.y -= 30;
	wxStaticBox * l_passBox = new wxStaticBox( this, wxID_ANY, wxT( "Passes"), wxPoint( 10, 30), l_size);

	wxArrayString l_names;
	wxString l_name;

	for (unsigned int i = 0 ; i < m_material->GetNbPasses() ; i++)
	{
		l_name.clear();
		l_name << i;
		l_names.push_back( l_name);
	}

	l_names.push_back( wxT( "New..."));
	m_passSelector = new wxComboBox( this, eID_COMBO_PASS, wxT( "0"), wxPoint( 20, 45), wxSize( 50, 20), l_names, wxBORDER_SIMPLE | wxCB_READONLY);
	m_deletePass = new wxButton( this, eID_BUTTON_DELETE, wxT( "Delete"), wxPoint( 80, 44), wxSize( 80, 23), wxBORDER_SIMPLE);
	m_passesPanel = new wxPanel( this, wxID_ANY, wxPoint( 15, 70), wxSize( l_passBox->GetClientSize().x - 10, l_passBox->GetClientSize().y - 30));
	m_selectedPassPanel = new wxPassPanel( m_passesPanel, wxPoint( 0, 0), m_passesPanel->GetClientSize());
	_createPassPanel();
}

void wxMaterialPanel :: _createPassPanel()
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

BEGIN_EVENT_TABLE( wxMaterialPanel, wxPanel)
	EVT_BUTTON(		eID_BUTTON_DELETE,			wxMaterialPanel::_onDeletePass)
	EVT_TEXT_ENTER( eID_EDIT_MATERIAL_NAME,		wxMaterialPanel::_onMaterialName)
	EVT_COMBOBOX(	eID_COMBO_PASS,				wxMaterialPanel::_onPassSelect)
END_EVENT_TABLE()

void wxMaterialPanel :: _onDeletePass( wxCommandEvent & event)
{
	if (m_material && m_material->GetNbPasses() > 1)
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

		l_name = wxT( "New...");
		m_passSelector->Insert( l_name, i);
		m_passSelector->SetValue( wxT( "New..."));
		m_passSelector->Update();
		this->RemoveChild( m_selectedPassPanel);

		if (m_material->GetNbPasses() == 1)
		{
			m_deletePass->Disable();
		}
	}
}

void wxMaterialPanel :: _onMaterialName( wxCommandEvent & event)
{
	String l_name = (char const *)m_materialName->GetValue().c_str();
	m_material->SetName( l_name);
	m_material->Initialise();
}

void wxMaterialPanel :: _onPassSelect( wxCommandEvent & event)
{
	m_selectedPassIndex = GetPassIndex();
	_createPassPanel();
}
