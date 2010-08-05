//******************************************************************************
#ifndef ___CSMaterialPanel___
#define ___CSMaterialPanel___
//******************************************************************************
#include "Module_Material.h"
//******************************************************************************
class CSMaterialPanel : public wxPanel
{
private:
	wxTextCtrl * m_materialName;
	wxComboBox * m_passSelector;
	wxButton * m_deletePass;
	wxPanel * m_passesPanel;

	Castor3D::Material * m_material;
	Castor3D::Pass * m_selectedPass;
	int m_selectedPassIndex;
	CSPassPanel * m_selectedPassPanel;

public:
	CSMaterialPanel( wxWindow * parent,
					  const wxPoint & pos = wxPoint( 0, 0),
					  const wxSize & size = wxSize( 370, 470),
					  wxWindowID id = wxID_ANY,
					  wxString name = wxString( (wxChar *)"CSMaterialPanel", 15));
	~CSMaterialPanel();

	void CreateMaterialPanel( const String & p_materialName);
	int GetPassIndex()const;

private:
	void _onDeletePass			( wxCommandEvent & event);
	void _onMaterialName		( wxCommandEvent & event);
	void _onPassSelect			( wxCommandEvent & event);

	void _createMaterialPanel( const Char * p_materialName);
	void _createPassPanel();

DECLARE_EVENT_TABLE()
};
DECLARE_APP( CSCastorShape)
//******************************************************************************
#endif
//******************************************************************************
