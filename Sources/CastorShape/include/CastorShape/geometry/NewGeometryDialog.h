//******************************************************************************
#ifndef ___CS_NEWGEOMETRYDIALOG___
#define ___CS_NEWGEOMETRYDIALOG___
//******************************************************************************
#include "Module_Geometry.h"
//******************************************************************************
class CSNewGeometryDialog : public wxDialog
{
protected:
	std::map <wxString, wxTextCtrl *> m_textCtrls;
	std::map <wxString, wxComboBox *> m_comboBoxes;
	int m_actualY;
	wxButton * m_okButton;

public:
	CSNewGeometryDialog( wxWindow * parent, wxWindowID p_id,
						 const wxString & p_name = C3D_T( "New Geometry"),
						 const wxPoint & pos = wxPoint( 200, 200),
						 const wxSize & size = wxSize( 200, 70),
						 long style = wxDEFAULT_DIALOG_STYLE);
	~CSNewGeometryDialog();

	void AddTextCtrl( const wxString & p_name, const wxString & p_defaultValue,
					  int p_width);
	void AddComboBox( const wxString & p_name, const wxArrayString & p_values,
					  const wxString & p_defaultValue, wxWindowID p_id,
					  int p_width);

	wxString GetTextValue( const wxString & p_name)const;
	wxString GetCBValue( const wxString & p_name)const;

	void RemoveTextCtrl( const wxString & p_name);
	void RemoveComboBox( const wxString & p_name);

	String GetGeometryName()const;
	String GetMaterialName()const;

protected:
	void _updateY( int p_offset);

private:
	void _onOk( wxCommandEvent& event);
	void _onMaterialSelect( wxCommandEvent& event);
DECLARE_EVENT_TABLE()
};
//******************************************************************************
#endif
//******************************************************************************
