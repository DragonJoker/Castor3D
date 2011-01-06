#ifndef ___CS_NEWMATERIALDIALOG___
#define ___CS_NEWMATERIALDIALOG___

#include "Module_Material.h"

namespace CastorShape
{
	class NewMaterialDialog : public wxDialog
	{
	public:
		enum eIDs
		{
			nmdName,
			nmdOK,
			nmdCancel,
			nmdDiffuse,
			nmdAmbient,
			nmdSpecular,
			nmdEmissive,
			nmdShininess,
			nmdTexture,
			nmdTextureButton,
		};

	private:
		GUICommon::MaterialPanel * m_materialPanel;
		wxButton * m_okButton;
		wxButton * m_cancelButton;
		Castor3D::MaterialPtr m_material;
		Castor3D::MaterialManager * m_pManager;

	public:
		NewMaterialDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id,
							 const wxString & p_name = CU_T( "New Material"),
							 const wxPoint & pos = wxDefaultPosition,
							 const wxSize & size = wxSize( 380, 500),
							 long style = wxDEFAULT_DIALOG_STYLE);
		~NewMaterialDialog();

		Castor3D::MaterialPtr GetMaterial()const {return m_material;}

	private:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & event);
		void OnOk( wxCommandEvent& event);
		void OnCancel( wxCommandEvent& event);
	};
}

#endif
