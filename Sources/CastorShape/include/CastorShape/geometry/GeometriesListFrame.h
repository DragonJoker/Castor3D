//******************************************************************************
#ifndef ___CSGeometriesFrame___
#define ___CSGeometriesFrame___
//******************************************************************************
#include "Module_Geometry.h"
//#include <Castor3D/scene/Module_Scene.h>
//******************************************************************************
class GeometryTreeItemData : public wxTreeItemData
{
protected:
	Castor3D::GeometryPtr m_pGeometry;

public:
	GeometryTreeItemData( Castor3D::GeometryPtr p_pGeometry);
	~GeometryTreeItemData();

	inline Castor3D::GeometryPtr GetGeometry() { return m_pGeometry; }
};

class CSGeometriesListFrame : public wxFrame
{
private:
	enum eIDs
	{
		eGeometriesList,
		eMaterialsList,
		eDeleteSelected
	};

	enum TreeImagesIDs
	{
		eFile			= 0,
		eFileSelected	= 1,
		eFolder			= 2,
		eFolderSelected	= 3,
		eFolderOpened	= 4,
		eClassFunction	= 5,
	};

	Castor3D::ScenePtr m_scene;
	wxTreeCtrl * m_pTreeGeometries;
	wxArrayString m_items;
	unsigned int m_nbItems;
	wxButton * m_pButtonDeleteSelected;
	wxComboBox * m_pComboMaterials;
	std::set <wxTreeItemId> m_setGeometriesInTree;

public:
	CSGeometriesListFrame( wxWindow * parent, const wxString & title,
						   Castor3D::ScenePtr p_scene,
						   const wxPoint & pos = wxDefaultPosition,
						   const wxSize & size = wxDefaultSize,
						   wxWindowID id = wxID_ANY,
						   long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR,
						   wxString name = wxString( (wxChar *)"CSGeometriesListFrame", 21));
	~CSGeometriesListFrame();

protected:
	DECLARE_EVENT_TABLE()
	void OnCheck( wxCommandEvent & event);
	void OnDelete( wxCommandEvent & event);
	void OnExpandItem( wxTreeEvent & event);
	void OnCollapseItem( wxTreeEvent & event);
	void OnActivateItem( wxTreeEvent & event);
};
//******************************************************************************
#endif
//******************************************************************************
