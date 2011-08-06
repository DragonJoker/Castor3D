#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/MaterialsListView.hpp"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using Castor::Templates::Collection;
using namespace Castor3D;
using namespace GuiCommon;

wxMaterialsListView :: wxMaterialsListView( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size)
	:	wxListCtrl( parent, id, pos, size, wxLC_ICON | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxBORDER_SIMPLE)
	,	m_images( nullptr)
	,	m_nbItems( 0)
{
	SetColumnWidth( -1, c_columnWidth);
	CreateList();
}

wxMaterialsListView :: ~wxMaterialsListView()
{
	if (m_images)
	{
		m_images->RemoveAll();
		delete m_images;
		m_images = nullptr;
	}

	ClearContainer( m_imagesArray);
	m_nbItems = 0;
}

void wxMaterialsListView :: CreateList()
{
	ClearAll();

	if (m_images)
	{
		m_images->RemoveAll();
		delete m_images;
		m_images = nullptr;
	}

	m_images = new wxImageList( c_materialIconSize, c_materialIconSize, false);
	StringArray l_materialsNames;
	Root::GetSingleton()->GetMaterialManager()->GetNames( l_materialsNames);
//	Root::GetSingletonPtr()->GetMaterialManager()->GetMaterialNames( l_materialsNames);

    SetImageList( m_images, wxIMAGE_LIST_NORMAL);

	String l_materialName;
	vector::deleteAll( m_imagesArray);

	for (size_t i = 0 ; i < l_materialsNames.size() ; i++)
	{
		l_materialName = l_materialsNames[i];
		AddItem( l_materialName);
	}
}

void wxMaterialsListView :: AddItem( String const & p_materialName)
{
	Collection<Material, String> l_mtlCollection;
	Point<byte, 3> l_colour;
	l_mtlCollection.GetElement( p_materialName)->GetPass( 0)->GetAmbient().RGB( l_colour);

	wxListItem l_item;
	int l_index = m_images->GetImageCount();
	l_item.SetMask( wxLIST_MASK_TEXT);
	l_item.SetId( l_index);
	l_item.SetAlign( wxLIST_FORMAT_LEFT);

	wxImage * l_pImage = GetMaterialImage( p_materialName, 0, c_materialIconSize, c_materialIconSize);

	if (l_pImage == NULL)
	{
		l_pImage = new wxImage( c_materialIconSize, c_materialIconSize);
		l_pImage->SetRGB( wxRect( 0, 0, c_materialIconSize, c_materialIconSize), l_colour[0], l_colour[1], l_colour[2]);
	}

	wxBitmap l_bitmap( * l_pImage);
	m_images->Add( l_bitmap);
	m_imagesArray.push_back( l_pImage);

	if (InsertItem( l_index, p_materialName, l_index) == -1)
	{
		Logger::LogWarning( cuT( "wxMaterialsListView :: AddItem - Item not inserted"));
	}
}

wxImage * wxMaterialsListView :: GetMaterialImage( String const & p_materialName, unsigned int p_index, unsigned int p_width, unsigned int p_height)
{
	wxImage * l_pReturn = NULL;
	Collection<Material, String> l_mtlCollection;
	MaterialPtr l_material = l_mtlCollection.GetElement( p_materialName);

	if (l_material && p_index < l_material->GetPass( 0)->GetNbTexUnits())
	{
		int l_iWidth = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetWidth();
		int l_iHeight = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetHeight();
		unsigned char * l_pData = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetImagePixels();
		wxBitmap l_bmp;
		_createBitmapFromBuffer( l_bmp, l_pData, l_iWidth, l_iHeight);
		l_pReturn = new wxImage( l_bmp.ConvertToImage());
		l_pReturn->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGHEST);
	}

	return l_pReturn;
}

void wxMaterialsListView :: _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height)
{
	p_bitmap.Create( p_width, p_height, 24);
	wxNativePixelData l_data( p_bitmap);

	if (l_data.GetWidth() == p_width && l_data.GetHeight() == p_height)
	{
		wxNativePixelData::Iterator l_it( l_data);
		unsigned char * l_pBuffer = p_pBuffer;

		try
		{
			for (int i = 0; i < p_width ; i++)
			{
				wxNativePixelData::Iterator l_rowStart = l_it;

				for (int j = 0; j < p_height ; j++)
				{
					l_it.Red() = *l_pBuffer;	l_pBuffer++;
					l_it.Green() = *l_pBuffer;	l_pBuffer++;
					l_it.Blue() = *l_pBuffer;	l_pBuffer++;
					l_pBuffer++;
					l_it++;
				}

				l_it = l_rowStart;
				l_it.OffsetY( l_data, 1);
			}
		}
		catch ( ... )
		{
			Logger::LogError( cuT( "coin"));
		}
	}
}
