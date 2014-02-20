#ifndef ___CS_NEWGEOMETRYDIALOG___
#define ___CS_NEWGEOMETRYDIALOG___

#include "Module_Geometry.hpp"

namespace CastorShape
{
	class NewGeometryDialog : public wxDialog
	{
	public:
		enum IDs
		{
			eOK,
			eCancel,
			eMaterialName,
		};

	protected:
		typedef std::map<wxString,	wxTextCtrl *> TextCtrlPtrStrMap;
		typedef std::map<wxString,	wxComboBox *> CubeBoxPtrStrMap;

		TextCtrlPtrStrMap	m_textCtrls;
		CubeBoxPtrStrMap	m_comboBoxes;
		int					m_actualY;
		wxButton *			m_okButton;
		Castor3D::Engine *	m_pEngine;

	public:
		NewGeometryDialog( Castor3D::Engine * p_pEngine, wxWindow * parent, wxWindowID p_id,
							 wxString const & p_name = cuT( "New Geometry"),
							 wxPoint const & pos = wxPoint( 200, 200),
							 wxSize const & size = wxSize( 200, 70),
							 long style = wxDEFAULT_DIALOG_STYLE);
		~NewGeometryDialog();

		wxSizer * AddTextCtrl( wxString const & p_name, wxString const & p_defaultValue, int p_width );
		wxSizer * AddCubeBox( wxString const & p_name, wxArrayString const & p_values, wxString const & p_defaultValue, wxWindowID p_id, int p_width );

		wxString GetTextValue( wxString const & p_name)const;
		wxString GetCBValue( wxString const & p_name)const;

		void RemoveTextCtrl( wxString const & p_name);
		void RemoveCubeBox( wxString const & p_name);

		Castor::String GetGeometryName()const;
		Castor::String GetMaterialName()const;

		void Initialise()
		{
			DoInitialise();
		}

	protected:
		void _updateY( int p_offset);
		virtual void DoInitialise()=0;
		wxSizer *	DoDefaultInit();
		void 		DoEndInit( wxSizer * p_pSizer );

	private:
		void _onOk( wxCommandEvent& event);
		void _onMaterialSelect( wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
	};
}

#endif
