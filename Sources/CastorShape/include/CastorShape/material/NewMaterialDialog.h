//******************************************************************************
#ifndef ___CS_NEWMATERIALDIALOG___
#define ___CS_NEWMATERIALDIALOG___
//******************************************************************************
#include "Module_Material.h"
//******************************************************************************
class CSNewMaterialDialog : public wxDialog
{
private:
	CSMaterialPanel * m_materialPanel;
	wxButton * m_okButton;
	wxButton * m_cancelButton;
	Castor3D::Material * m_material;

public:
	CSNewMaterialDialog( wxWindow * parent, wxWindowID p_id,
						 const wxString & p_name = C3D_T( "New Material"),
						 const wxPoint & pos = wxDefaultPosition,
						 const wxSize & size = wxSize( 380, 500),
						 long style = wxDEFAULT_DIALOG_STYLE);
	~CSNewMaterialDialog();

	void OnClose( wxCloseEvent & event);
	void OnOk( wxCommandEvent& event);
	void OnCancel( wxCommandEvent& event);

	Castor3D::Material * GetMaterial()const {return m_material;}
DECLARE_EVENT_TABLE()
};
//******************************************************************************
#endif
//******************************************************************************
