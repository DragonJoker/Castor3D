#ifndef ___GC_MaterialsListViewFrame___
#define ___GC_MaterialsListViewFrame___

const static unsigned int c_materialIconSize = 40;

namespace GUICommon
{
	class MaterialsListView : public wxListCtrl
	{
	private:
		const static unsigned int c_columnWidth = c_materialIconSize + 2;

	private:
		wxImageList * m_images;
		wxListItem * m_items;
		unsigned int m_nbItems;
		Container<wxImage *>::Vector m_imagesArray;
		Castor3D::MaterialManager * m_pManager;

	public:
		MaterialsListView( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID id = wxID_ANY, const wxPoint & pos = wxDefaultPosition,const wxSize & size = wxDefaultSize);
		~MaterialsListView();

		wxImage * GetMaterialImage( const String & p_materialName, unsigned int p_index, unsigned int p_width, unsigned int p_height);
		void CreateList();
		void AddItem( const String & p_materialName);

	private:
		void _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height);
	};
}

#endif