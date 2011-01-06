#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/material/NewMaterialDialog.h"

using namespace Castor3D;
using namespace CastorShape;
using namespace GUICommon;

NewMaterialDialog :: NewMaterialDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id,
										    const wxString & p_name,
										    const wxPoint & pos, const wxSize & size,
										    long style)
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name)
	,	m_pManager( p_pManager)
{
	wxSize l_size = GetClientSize();
	l_size.y -= 30;
	m_material = p_pManager->CreateMaterial( CU_T( "NewMaterial"));
	m_materialPanel = new MaterialPanel( m_pManager, this, wxPoint( 0, 0), l_size);
	m_materialPanel->CreateMaterialPanel( CU_T( "NewMaterial"));
	m_okButton = new wxButton( this, nmdOK, CU_T( "OK"), wxPoint( 20, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
	m_cancelButton = new wxButton( this, nmdCancel, CU_T( "Annuler"), wxPoint( 120, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
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
	m_pManager->RemoveElement( m_material);
	EndModal( wxID_CANCEL);
}

void NewMaterialDialog :: OnOk( wxCommandEvent& event)
{
	EndModal( wxID_OK);
}

void NewMaterialDialog :: OnCancel( wxCommandEvent& event)
{
	if ( ! m_material == NULL)
	{
		m_pManager->RemoveElement( m_material);
	}

	EndModal( wxID_CANCEL);
}
