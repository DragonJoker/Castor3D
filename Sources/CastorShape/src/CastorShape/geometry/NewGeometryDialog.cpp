//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewGeometryDialog.h"
#include "material/NewMaterialDialog.h"
//******************************************************************************
BEGIN_EVENT_TABLE( CSNewGeometryDialog, wxDialog)
	EVT_BUTTON(		ngpOK,				CSNewGeometryDialog::_onOk)
	EVT_COMBOBOX(	ngpMaterialName,	CSNewGeometryDialog::_onMaterialSelect)
END_EVENT_TABLE()
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

CSNewGeometryDialog :: CSNewGeometryDialog( wxWindow * parent, wxWindowID p_id,
										    const wxString & p_name,
										    const wxPoint & pos, const wxSize & size,
										    long style)
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name),
		m_actualY( 10)
{

	m_okButton = new wxButton( this, ngpOK, CU_T( "OK"), wxPoint( 70, m_actualY),
							   wxSize( 60, 30), wxBORDER_SIMPLE);

	AddTextCtrl( CU_T( "Name"), CU_T( "Geometry Name"), 110);

	StringArray l_choices1;
	MaterialManager::GetMaterialNames( l_choices1);
	wxArrayString l_choices;
	l_choices.push_back( CU_T( "New..."));
	for (size_t i = 0 ; i < l_choices1.size() ; i++)
	{
		l_choices.push_back( wxString( l_choices1[i].c_str()));
	}

	AddComboBox( CU_T( "Material"), l_choices, CU_T( "New..."), ngpMaterialName, 110);
}

//******************************************************************************

CSNewGeometryDialog :: ~CSNewGeometryDialog()
{
}

//******************************************************************************

void CSNewGeometryDialog :: AddTextCtrl( const wxString & p_name, 
										 const wxString & p_defaultValue,
										 int p_width)
{
	if (m_textCtrls.find( p_name) != m_textCtrls.end())
	{
		return;
	}
	wxSize l_size = GetClientSize() - wxSize( 30 + p_width, 0);
	int l_width = l_size.x;
	new wxStaticText( this, wxID_ANY, p_name + CU_T( " : "), wxPoint( 10, m_actualY), wxSize( l_width, 20));
	wxTextCtrl * l_edit = new wxTextCtrl( this, wxID_ANY, p_defaultValue, 
										  wxPoint( 80, m_actualY), wxSize( p_width, 20),
										  wxBORDER_SIMPLE);
	m_textCtrls.insert( C3DMap( wxString, wxTextCtrl *)::value_type( p_name, l_edit));
	_updateY( 30);
}

//******************************************************************************

void CSNewGeometryDialog :: AddComboBox( const wxString & p_name,
										 const wxArrayString & p_values,
										 const wxString & p_defaultValue,
										 wxWindowID p_id, int p_width)
{
	if (m_comboBoxes.find( p_name) != m_comboBoxes.end())
	{
		return;
	}
	wxSize l_size = GetClientSize() - wxSize( 30 + p_width, 0);
	int l_width = l_size.x;
	new wxStaticText( this, wxID_ANY, p_name + CU_T( " : "), wxPoint( 10, m_actualY), wxSize( l_width, 20));
	wxComboBox * l_box = new wxComboBox( this, p_id, p_defaultValue,
										 wxPoint( 80, m_actualY), wxSize( p_width, 20), 
										 p_values, wxBORDER_SIMPLE | wxCB_READONLY);
	m_comboBoxes.insert( C3DMap( wxString, wxComboBox *)::value_type( p_name, l_box));
	_updateY( 30);
}

//******************************************************************************

wxString CSNewGeometryDialog :: GetTextValue( const wxString & p_name)const
{
	if (m_textCtrls.find( p_name) != m_textCtrls.end())
	{
		return m_textCtrls.find( p_name)->second->GetValue();
	}
	return wxEmptyString;
}

//******************************************************************************

wxString CSNewGeometryDialog :: GetCBValue( const wxString & p_name)const
{
	if (m_comboBoxes.find( p_name) != m_comboBoxes.end())
	{
		return m_comboBoxes.find( p_name)->second->GetValue();
	}
	return wxEmptyString;
}

//******************************************************************************

void CSNewGeometryDialog :: RemoveTextCtrl( const wxString & p_name)
{
	C3DMap( wxString, wxTextCtrl *)::iterator l_it = m_textCtrls.find( p_name);
	RemoveChild( l_it->second);
	delete l_it->second;
	m_textCtrls.erase( l_it);
}

//******************************************************************************

void CSNewGeometryDialog :: RemoveComboBox( const wxString & p_name)
{
	C3DMap( wxString, wxComboBox *)::iterator l_it = m_comboBoxes.find( p_name);
	RemoveChild( l_it->second);
	delete l_it->second;
	m_comboBoxes.erase( l_it);
}

//******************************************************************************

String CSNewGeometryDialog :: GetGeometryName()const
{
	return String( GetTextValue( CU_T( "Name")).c_str());
}

//******************************************************************************

String CSNewGeometryDialog :: GetMaterialName()const
{
	wxString l_value = GetCBValue( CU_T( "Material"));
	String l_res = l_value.c_str();
	CSNewMaterialDialog * l_dialog;
	MaterialPtr l_material;

	while (l_res == CU_T( "New..."))
	{
		l_material.reset();
		l_dialog = new CSNewMaterialDialog( NULL, wxID_ANY);

		if (l_dialog->ShowModal() == wxID_OK)
		{
			l_material = l_dialog->GetMaterial();

			if ( ! l_material.null())
			{
				l_res = l_material->GetName();
			}
		}
		l_dialog->Destroy();
	}
	return l_res;
}

//******************************************************************************

void CSNewGeometryDialog :: _updateY( int p_offset)
{
	SetSize( GetSize() + wxSize( 0, p_offset));
	m_actualY += p_offset;
	m_okButton->SetPosition( wxPoint( 60, m_actualY));
}

//******************************************************************************

void CSNewGeometryDialog :: _onOk(wxCommandEvent& event)
{
	EndModal( wxID_OK);
}

//******************************************************************************

void CSNewGeometryDialog :: _onMaterialSelect( wxCommandEvent& event)
{
/*
	wxString l_value = GetCBValue( CU_T( "Material"));
	wxString l_res;
	if (l_value == CU_T( "New..."))
	{
		CSNewMaterialDialog * l_dialog = new CSNewMaterialDialog( NULL, wxID_ANY);
		MaterialPtr l_material = NULL;
		if (l_dialog->ShowModal() == wxID_OK)
		{
			l_material = l_dialog->GetMaterial();
		}
		l_dialog->Destroy();

		if (l_material != NULL)
		{
			C3DVector <String> l_choices1;
			MaterialManager::GetMaterialNames( l_choices1);

			wxArrayString l_choices;
			l_choices.push_back( CU_T( "New..."));
			for (size_t i = 0 ; i < l_choices1.size() ; i++)
			{
				l_choices.push_back( l_choices1[i].c_str());
			}
			l_res = l_material->GetName();

			C3DMap( wxString, wxComboBox *)::iterator l_it = m_comboBoxes.find( CU_T( "Material"));
			RemoveChild( l_it->second);
			delete l_it->second;
			l_it->second = new wxComboBox( this, ngpMaterialName, l_res, wxPoint( 80, 40), wxSize( 110, 20), l_choices, wxBORDER_SIMPLE | wxCB_READONLY);
		}
	}
*/
}

//******************************************************************************
