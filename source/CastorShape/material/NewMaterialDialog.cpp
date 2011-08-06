#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/material/NewMaterialDialog.hpp"

using namespace Castor3D;
using namespace CastorShape;
using namespace GuiCommon;

NewMaterialDialog :: NewMaterialDialog( wxWindow * parent, wxWindowID p_id,
										    const wxString & p_name,
										    const wxPoint & pos, const wxSize & size,
										    long style)
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name)
{
	wxSize l_size = GetClientSize();
	l_size.y -= 30;
	m_material = Factory<Material>::Create( cuT( "NewMaterial"), 1);
	m_materialPanel = new wxMaterialPanel( this, wxPoint( 0, 0), l_size);
	m_materialPanel->CreateMaterialPanel( cuT( "NewMaterial"));
	m_okButton = new wxButton( this, nmdOK, cuT( "OK"), wxPoint( 20, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
	m_cancelButton = new wxButton( this, nmdCancel, cuT( "Annuler"), wxPoint( 120, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
}

NewMaterialDialog :: ~NewMaterialDialog()
{
}

BEGIN_EVENT_TABLE( NewMaterialDialog, wxDialog)
	EVT_CLOSE(					NewMaterialDialog::OnClose)
	EVT_BUTTON(	nmdOK,			NewMaterialDialog::OnOk)
	EVT_BUTTON(	nmdCancel,		NewMaterialDialog::OnCancel)
END_EVENT_TABLE()

void NewMaterialDialog :: OnClose( wxCloseEvent & event)
{
	Collection<Material, String> l_mtlCollection;
	l_mtlCollection.RemoveElement( m_material->GetName());
	EndModal( wxID_CANCEL);
}

void NewMaterialDialog :: OnOk( wxCommandEvent& event)
{
	EndModal( wxID_OK);
}

void NewMaterialDialog :: OnCancel( wxCommandEvent& event)
{
	if (m_material)
	{
		Collection<Material, String> l_mtlCollection;
		l_mtlCollection.RemoveElement( m_material->GetName());
	}

	EndModal( wxID_CANCEL);
}
