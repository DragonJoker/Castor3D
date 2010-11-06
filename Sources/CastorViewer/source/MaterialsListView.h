#ifndef ___CV_MaterialsListViewFrame___
#define ___CV_MaterialsListViewFrame___

const static unsigned int c_materialIconSize = 40;

namespace CastorViewer
{
	class MaterialsListView : public wxListCtrl
	{
	private:
		const static unsigned int c_columnWidth = c_materialIconSize + 2;

	private:
		wxImageList * m_images;
		wxListItem * m_items;
		unsigned int m_nbItems;
		C3DVector( wxImage *) m_imagesArray;

	public:
		MaterialsListView( wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint & pos = wxDefaultPosition,const wxSize & size = wxDefaultSize);
		~MaterialsListView();

		wxImage * GetMaterialImage( const String & p_materialName, unsigned int p_index, unsigned int p_width, unsigned int p_height);
		void CreateList();
		void AddItem( const String & p_materialName);
	};
}

#endif