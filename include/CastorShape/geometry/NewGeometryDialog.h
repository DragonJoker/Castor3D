#ifndef ___CS_NEWGEOMETRYDIALOG___
#define ___CS_NEWGEOMETRYDIALOG___

#include "Module_Geometry.h"

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
		typedef KeyedContainer<wxString,	wxTextCtrl *>::Map TextCtrlPtrStrMap;
		typedef KeyedContainer<wxString,	wxComboBox *>::Map CubeBoxPtrStrMap;

		TextCtrlPtrStrMap m_textCtrls;
		CubeBoxPtrStrMap m_comboBoxes;
		int m_actualY;
		wxButton * m_okButton;
		Castor3D::MaterialManager * m_pManager;

	public:
		NewGeometryDialog( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id,
							 const wxString & p_name = CU_T( "New Geometry"),
							 const wxPoint & pos = wxPoint( 200, 200),
							 const wxSize & size = wxSize( 200, 70),
							 long style = wxDEFAULT_DIALOG_STYLE);
		~NewGeometryDialog();

		void AddTextCtrl( const wxString & p_name, const wxString & p_defaultValue,
						  int p_width);
		void AddCubeBox( const wxString & p_name, const wxArrayString & p_values,
						  const wxString & p_defaultValue, wxWindowID p_id,
						  int p_width);

		wxString GetTextValue( const wxString & p_name)const;
		wxString GetCBValue( const wxString & p_name)const;

		void RemoveTextCtrl( const wxString & p_name);
		void RemoveCubeBox( const wxString & p_name);

		String GetGeometryName()const;
		String GetMaterialName()const;

		void Initialise()
		{
			_initialise();
		}

	protected:
		void _updateY( int p_offset);
		virtual void _initialise()=0;
		void _defaultInit();
		void _endInit();

	private:
		void _onOk( wxCommandEvent& event);
		void _onMaterialSelect( wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
	};
}

#endif
