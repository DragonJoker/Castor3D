//******************************************************************************
#include "PrecompiledHeaders.h"

#include "material/NewMaterialDialog.h"
#include "material/MaterialPanel.h"
//******************************************************************************
BEGIN_EVENT_TABLE( CSNewMaterialDialog, wxDialog)
	EVT_CLOSE(	CSNewMaterialDialog::OnClose)
	EVT_BUTTON(	nmdOK,			CSNewMaterialDialog::OnOk)
	EVT_BUTTON(	nmdCancel,		CSNewMaterialDialog::OnCancel)
END_EVENT_TABLE()
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

CSNewMaterialDialog :: CSNewMaterialDialog( wxWindow * parent, wxWindowID p_id,
										    const wxString & p_name,
										    const wxPoint & pos, const wxSize & size,
										    long style)
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name),
		m_material( NULL)
{
	wxSize l_size = GetClientSize();
	l_size.y -= 30;
	m_material = MaterialManager::GetSingletonPtr()->CreateMaterial( C3D_T( "NewMaterial"));
	m_materialPanel = new CSMaterialPanel( this, wxPoint( 0, 0), l_size);
	m_materialPanel->CreateMaterialPanel( C3D_T( "NewMaterial"));
	m_okButton = new wxButton( this, nmdOK, C3D_T( "OK"), wxPoint( 20, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
	m_cancelButton = new wxButton( this, nmdCancel, C3D_T( "Annuler"), wxPoint( 120, l_size.y + 5), wxSize( 60, 20), wxBORDER_SIMPLE);
}

//******************************************************************************

CSNewMaterialDialog :: ~CSNewMaterialDialog()
{
}

//******************************************************************************

void CSNewMaterialDialog :: OnClose( wxCloseEvent & event)
{
	MaterialManager::GetSingletonPtr()->RemoveElement( m_material);
	delete m_material;
	m_material = NULL;
	EndModal( wxID_CANCEL);
}

//******************************************************************************

void CSNewMaterialDialog :: OnOk( wxCommandEvent& event)
{
	EndModal( wxID_OK);
}

//******************************************************************************

void CSNewMaterialDialog :: OnCancel( wxCommandEvent& event)
{
	if (m_material != NULL)
	{
		MaterialManager::GetSingletonPtr()->RemoveElement( m_material);
		delete m_material;
		m_material = NULL;
	}
	EndModal( wxID_CANCEL);
}

//******************************************************************************
