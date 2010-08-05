//******************************************************************************
#ifndef ___CSMaterialsListViewFrame___
#define ___CSMaterialsListViewFrame___
//******************************************************************************
#include "Module_Material.h"
//******************************************************************************
const static unsigned int c_materialIconSize = 40;
//******************************************************************************
class CSMaterialsListView : public wxListCtrl
{
private:
	const static unsigned int c_columnWidth = c_materialIconSize + 2;
//	static unsigned char * s_data;

private:
	wxImageList * m_images;
	wxListItem * m_items;
	unsigned int m_nbItems;
	std::vector <wxImage *> m_imagesArray;

public:
	CSMaterialsListView( wxWindow * parent, wxWindowID id = wxID_ANY, 
						 const wxPoint & pos = wxDefaultPosition,
						 const wxSize & size = wxDefaultSize,
						 long style = wxLC_ICON | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING,
						 const wxValidator& validator = wxDefaultValidator,
						 wxString name = wxString( (wxChar *)"CSMaterialsListView", 19));
	~CSMaterialsListView();

	wxImage * GetMaterialImage( const String & p_materialName, unsigned int p_index,
								unsigned int p_width, unsigned int p_height);
	void CreateList();
	void AddItem( const String & p_materialName);

DECLARE_EVENT_TABLE()
};
DECLARE_APP( CSCastorShape)
//******************************************************************************
#endif
//******************************************************************************