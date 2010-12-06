#include "PrecompiledHeaders.h"

#include "material/MaterialsListView.h"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using Castor::Templates::Manager;
using namespace Castor3D;

BEGIN_EVENT_TABLE( CSMaterialsListView, wxListCtrl)
END_EVENT_TABLE()

CSMaterialsListView :: CSMaterialsListView( wxWindow * parent, wxWindowID id, 
										    const wxPoint & pos,
											const wxSize & size, long style,
											const wxValidator& validator,
											wxString name)
	:	wxListCtrl( parent, id, pos, size, style | wxBORDER_SIMPLE, validator, name),
		m_images( NULL),
		m_nbItems( 0)
{
	SetColumnWidth( -1, c_columnWidth);
	CreateList();
}

CSMaterialsListView :: ~CSMaterialsListView()
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

void CSMaterialsListView :: CreateList()
{
	ClearAll();
	if (m_images)
	{
		m_images->RemoveAll();
		delete m_images;
		m_images = NULL;
	}
	m_nbItems = 0;

	MaterialManager * l_materialManager = MaterialManager::GetSingletonPtr();
	m_nbItems = static_cast <unsigned int>( l_materialManager->m_objectMap.size());
	m_images = new wxImageList( c_materialIconSize, c_materialIconSize, false);
	StringArray l_materialsNames;
	l_materialManager->GetMaterialNames( l_materialsNames);

    SetImageList( m_images, wxIMAGE_LIST_NORMAL);

	String l_materialName;
	vector::deleteAll( m_imagesArray);
	for (unsigned int i = 0 ; i < m_nbItems ; i++)
	{
		l_materialName = l_materialsNames[i];
		AddItem( l_materialName);
	}
}

void CSMaterialsListView :: AddItem( const String & p_materialName)
{
	real l_col0;
	real l_col1;
	real l_col2;
	unsigned char l_ccol0;
	unsigned char l_ccol1;
	unsigned char l_ccol2;
	const float * l_colour = MaterialManager::GetElementByName( p_materialName)->GetPass( 0)->GetAmbient();
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

	wxImage * l_image = GetMaterialImage( p_materialName, 0, c_materialIconSize, c_materialIconSize);
	if (l_image == NULL)
	{
		l_image = new wxImage( c_materialIconSize, c_materialIconSize);
		l_image->SetRGB( wxRect( 0, 0, c_materialIconSize, c_materialIconSize), l_ccol0, l_ccol1, l_ccol2);
		Logger::LogMessage( CU_T( "CSMaterialsListView :: AddItem - No texture for index %d"), l_index);
	}
	wxBitmap l_bitmap( * l_image);
	m_images->Add( l_bitmap);
	m_imagesArray.push_back( l_image);
	if (InsertItem( l_index, p_materialName.c_str(), l_index) == -1)
	{
		Logger::LogMessage( CU_T( "CSMaterialsListView :: AddItem - Item not inserted"));
	}
}

wxImage * CSMaterialsListView :: GetMaterialImage( const String & p_materialName,
												   unsigned int p_index,
												   unsigned int p_width,
												   unsigned int p_height)
{
	MaterialPtr l_material = MaterialManager::GetElementByName( p_materialName);

	if ( ! l_material.null() && p_index < l_material->GetPass( 0)->GetNbTexUnits())
	{
		String l_path;
		l_path = l_material->GetPass( 0)->GetTextureUnit( p_index)->GetTexturePath();

		if (l_path.size() > 0)
		{
			wxImage * l_tmp = new wxImage;

			if (l_tmp->LoadFile( l_path.c_str()))
			{
				l_tmp->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGH);
				return l_tmp;
			}
		}
	}

	return NULL;
}
