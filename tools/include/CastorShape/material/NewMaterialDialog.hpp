#ifndef ___CS_NEWMATERIALDIALOG___
#define ___CS_NEWMATERIALDIALOG___

#include "Module_Material.hpp"

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
		GuiCommon::wxMaterialPanel * m_materialPanel;
		wxButton * m_okButton;
		wxButton * m_cancelButton;
		Castor3D::MaterialWPtr m_material;
		Castor3D::Engine * m_pEngine;

	public:
		NewMaterialDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id,
							 wxString const & p_name = cuT( "New Material"),
							 wxPoint const & pos = wxDefaultPosition,
							 wxSize const & size = wxSize( 380, 500),
							 long style = wxDEFAULT_DIALOG_STYLE);
		~NewMaterialDialog();

		Castor3D::MaterialSPtr GetMaterial()const {return m_material.lock();}

	private:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & event);
		void OnOk( wxCommandEvent& event);
		void OnCancel( wxCommandEvent& event);
	};
}

#endif
