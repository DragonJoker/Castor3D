#ifndef ___GC_MaterialsListViewFrame___
#define ___GC_MaterialsListViewFrame___

const static unsigned int c_materialIconSize = 40;

namespace GuiCommon
{
	class wxMaterialsListView : public wxListCtrl
	{
	private:
		const static unsigned int c_columnWidth = c_materialIconSize + 2;

	private:
		wxImageList * m_images;
		wxListItem * m_items;
		unsigned int m_nbItems;
		Container<wxImage *>::Vector m_imagesArray;

	public:
		wxMaterialsListView( wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint & pos = wxDefaultPosition,const wxSize & size = wxDefaultSize);
		~wxMaterialsListView();

		wxImage * GetMaterialImage( String const & p_materialName, unsigned int p_index, unsigned int p_width, unsigned int p_height);
		void CreateList();
		void AddItem( String const & p_materialName);

	private:
		void _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height);
	};
}

#endif
