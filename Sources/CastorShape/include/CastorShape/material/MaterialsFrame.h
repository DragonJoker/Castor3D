//******************************************************************************
#ifndef ___CSMaterialsFrame___
#define ___CSMaterialsFrame___
//******************************************************************************
#include "Module_Material.h"
//******************************************************************************
class CSMaterialsFrame : public wxFrame
{
private:
	CSMaterialsListView * m_materialsList;
	int m_listWidth;
	Castor3D::Material * m_selectedMaterial;

	CSMaterialPanel * m_materialPanel;
	wxButton * m_newMaterial;
	wxButton * m_deleteMaterial;
	wxTextCtrl * m_newMaterialName;

public:
	CSMaterialsFrame( wxWindow * parent, const wxString & title,
					  const wxPoint & pos = wxDefaultPosition,
					  const wxSize & size = wxSize( 500, 500),
					  wxWindowID id = mfMaterials,
					  long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR,
					  wxString name = C3D_T( "MaterialsFrame"));
	~CSMaterialsFrame();

	void CreateMaterialPanel( const String & p_materialName);

private:
    void _onSelected	( wxListEvent & event);
    void _onDeselected	( wxListEvent & event);

	void _onNewMaterial			( wxCommandEvent & event);
	void _onNewMaterialName		( wxCommandEvent & event);
	void _onDeleteMaterial		( wxCommandEvent & event);

DECLARE_EVENT_TABLE()
};
DECLARE_APP( CSCastorShape)
//******************************************************************************
#endif
//******************************************************************************
