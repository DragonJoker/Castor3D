#include "GUICommon/PrecompiledHeader.h"

#include "GUICommon/MaterialsListView.h"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using Castor::Templates::Manager;
using namespace Castor3D;
using namespace GUICommon;

MaterialsListView :: MaterialsListView( Castor3D::MaterialManager * p_pManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size)
	:	wxListCtrl( parent, id, pos, size, wxLC_ICON | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxBORDER_SIMPLE)
	,	m_images( NULL)
	,	m_nbItems( 0)
	,	m_pManager( p_pManager)
{
	SetColumnWidth( -1, c_columnWidth);
	CreateList();
}

MaterialsListView :: ~MaterialsListView()
{
	if (m_images)
	{
		m_images->RemoveAll();
		delete m_images;
		m_images = NULL;
	}
	vector::deleteAll( m_imagesArray);
	m_nbItems = 0;
}

void MaterialsListView :: CreateList()
{
	ClearAll();

	if (m_images != NULL)
	{
		m_images->RemoveAll();
		delete m_images;
		m_images = NULL;
	}

	m_nbItems = 0;

	m_nbItems = static_cast <unsigned int>( m_pManager->m_objectMap.size());
	m_images = new wxImageList( c_materialIconSize, c_materialIconSize, false);
	StringArray l_materialsNames;
	m_pManager->GetMaterialNames( l_materialsNames);

    SetImageList( m_images, wxIMAGE_LIST_NORMAL);

	String l_materialName;
	vector::deleteAll( m_imagesArray);

	for (unsigned int i = 0 ; i < m_nbItems ; i++)
	{
		l_materialName = l_materialsNames[i];
		AddItem( l_materialName);
	}
}

void MaterialsListView :: AddItem( const String & p_materialName)
{
	real l_col0;
	real l_col1;
	real l_col2;
	unsigned char l_ccol0;
	unsigned char l_ccol1;
	unsigned char l_ccol2;
	const float * l_colour = m_pManager->GetElementByName( p_materialName)->GetPass( 0)->GetAmbient().const_ptr();
	l_col0 = l_colour[0] * 255.0;
	l_col1 = l_colour[1] * 255.0;
	l_col2 = l_colour[2] * 255.0;
	l_ccol0 = (unsigned char)l_col0;
	l_ccol1 = (unsigned char)l_col1;
	l_ccol2 = (unsigned char)l_col2;

	wxListItem l_item;
	int l_index = m_images->GetImageCount();
	l_item.SetMask( wxLIST_MASK_TEXT);
	l_item.SetId( l_index);
	l_item.SetAlign( wxLIST_FORMAT_LEFT);

	wxImage * l_pImage = GetMaterialImage( p_materialName, 0, c_materialIconSize, c_materialIconSize);

	if (l_pImage == NULL)
	{
		l_pImage = new wxImage( c_materialIconSize, c_materialIconSize);
		l_pImage->SetRGB( wxRect( 0, 0, c_materialIconSize, c_materialIconSize), l_ccol0, l_ccol1, l_ccol2);
		Logger::LogMessage( CU_T( "MaterialsListView :: AddItem - No texture for index %d"), l_index);
	}

	wxBitmap l_bitmap( * l_pImage);
	m_images->Add( l_bitmap);
	m_imagesArray.push_back( l_pImage);

	if (InsertItem( l_index, p_materialName.c_str(), l_index) == -1)
	{
		Logger::LogWarning( CU_T( "MaterialsListView :: AddItem - Item not inserted"));
	}
}

wxImage * MaterialsListView :: GetMaterialImage( const String & p_materialName, unsigned int p_index, unsigned int p_width, unsigned int p_height)
{
	wxImage * l_pReturn = NULL;
	MaterialPtr l_material = m_pManager->GetElementByName( p_materialName);

	if (l_material != NULL && p_index < l_material->GetPass( 0)->GetNbTexUnits())
	{
		int l_iWidth = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetWidth();
		int l_iHeight = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetHeight();
		unsigned char * l_pData = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetImagePixels();
		wxBitmap l_bmp;
		_createBitmapFromBuffer( l_bmp, l_pData, l_iWidth, l_iHeight);
		l_pReturn = new wxImage( l_bmp.ConvertToImage());
		l_pReturn->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGH);
	}

	return l_pReturn;
}

void MaterialsListView :: _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height)
{
	p_bitmap.Create( p_width, p_height, 24);
	wxNativePixelData l_data( p_bitmap);

	if (l_data != NULL && l_data.GetWidth() == p_width && l_data.GetHeight() == p_height)
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
			Logger::LogError( "coin");
		}
	}
}