#include "MaterialsList.hpp"

#include "PassPanel.hpp"
#include "ImagesLoader.hpp"
#include "MaterialTreeItemProperty.hpp"
#include "PassTreeItemProperty.hpp"
#include "RenderTargetTreeItemProperty.hpp"
#include "TextureTreeItemProperty.hpp"
#include "PropertiesHolder.hpp"

#include <Engine.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Pass.hpp>
#include <Logger.hpp>
#include <DynamicTexture.hpp>

#include <wx/imaglist.h>

#include "xpms/material.xpm"
#include "xpms/material_sel.xpm"
#include "xpms/pass.xpm"
#include "xpms/pass_sel.xpm"
#include "xpms/texture.xpm"
#include "xpms/texture_sel.xpm"

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	wxMaterialsList::wxMaterialsList( wxPropertiesHolder * p_propertiesHolder, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxTreeCtrl( p_pParent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_pEngine( NULL )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor l_wait;
		wxImagesLoader::AddBitmap( eBMP_MATERIAL, material_xpm );
		wxImagesLoader::AddBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_PASS, pass_xpm );
		wxImagesLoader::AddBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		wxImagesLoader::AddBitmap( eBMP_TEXTURE, texture_xpm );
		wxImagesLoader::AddBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		wxImagesLoader::WaitAsyncLoads();

		wxImage * l_icons[] =
		{
			wxImagesLoader::GetBitmap( eBMP_MATERIAL ),
			wxImagesLoader::GetBitmap( eBMP_MATERIAL_SEL ),
			wxImagesLoader::GetBitmap( eBMP_PASS ),
			wxImagesLoader::GetBitmap( eBMP_PASS_SEL ),
			wxImagesLoader::GetBitmap( eBMP_TEXTURE ),
			wxImagesLoader::GetBitmap( eBMP_TEXTURE_SEL ),
		};

		wxImageList * l_imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto && l_image : l_icons )
		{
			int l_sizeOrig = l_image->GetWidth();

			if ( l_sizeOrig != GC_IMG_SIZE )
			{
				l_image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			l_imageList->Add( wxImage( *l_image ) );
		}

		AssignImageList( l_imageList );
	}

	wxMaterialsList::~wxMaterialsList()
	{
		DeleteAllItems();
	}

	void wxMaterialsList::LoadMaterials( Castor3D::Engine * p_pEngine )
	{
		m_pEngine = p_pEngine;
		wxTreeItemId l_root = AddRoot( _( "Root" ), eBMP_SCENE, eBMP_SCENE_SEL );
		m_pEngine->GetMaterialManager().lock();

		for ( auto && l_pair : m_pEngine->GetMaterialManager() )
		{
			DoAddMaterial( l_root, l_pair.second );
		}

		m_pEngine->GetMaterialManager().unlock();
	}

	void wxMaterialsList::UnloadMaterials()
	{
		DeleteAllItems();
	}

	void wxMaterialsList::DoAddMaterial( wxTreeItemId p_id, Castor3D::MaterialSPtr p_material )
	{
		wxTreeItemId l_id = AppendItem( p_id, p_material->GetName(), eBMP_MATERIAL - eBMP_MATERIAL, eBMP_MATERIAL_SEL - eBMP_MATERIAL, new wxMaterialTreeItemProperty( p_material ) );
		uint32_t l_index = 0;

		for ( auto && l_pass : *p_material )
		{
			DoAddPass( l_id, l_index++, l_pass );
		}
	}

	void wxMaterialsList::DoAddPass( wxTreeItemId p_id, uint32_t p_index, Castor3D::PassSPtr p_pass )
	{
		wxTreeItemId l_id = AppendItem( p_id, wxString( _( "Pass " ) ) << p_index, eBMP_PASS - eBMP_MATERIAL, eBMP_PASS_SEL - eBMP_MATERIAL, new wxPassTreeItemProperrty( p_pass ) );
		uint32_t l_index = 0;

		for ( auto && l_pass : *p_pass )
		{
			DoAddTexture( l_id, l_index++, l_pass );
		}
	}

	void wxMaterialsList::DoAddTexture( wxTreeItemId p_id, uint32_t p_index, Castor3D::TextureUnitSPtr p_texture )
	{
		wxTreeItemId l_id = AppendItem( p_id, wxString( _( "Texture Unit " ) ) << p_index, eBMP_TEXTURE - eBMP_MATERIAL, eBMP_TEXTURE_SEL - eBMP_MATERIAL, new wxTextureTreeItemProperty( p_texture ) );
		TextureBaseSPtr l_texture = p_texture->GetTexture();

		if ( l_texture->GetType() == eTEXTURE_TYPE_DYNAMIC )
		{
			RenderTargetSPtr l_target = std::static_pointer_cast< DynamicTexture >( l_texture )->GetRenderTarget();

			if ( l_target )
			{
				wxString l_name = _( "Render Target" );
				AppendItem( l_id, l_name, eBMP_RENDER_TARGET, eBMP_RENDER_TARGET_SEL, new wxRenderTargetTreeItemProperty( l_target ) );
			}
		}
	}

	BEGIN_EVENT_TABLE( wxMaterialsList, wxTreeCtrl )
		EVT_CLOSE( wxMaterialsList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, wxMaterialsList::OnSelectItem )
	END_EVENT_TABLE()

	void wxMaterialsList::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void wxMaterialsList::OnSelectItem( wxTreeEvent & p_event )
	{
		wxTreeItemProperty * l_data = reinterpret_cast< wxTreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( l_data );
		p_event.Skip();
	}

	//void wxMaterialsList::AddItem( String const & p_strMaterialName )
	//{
	//	MaterialSPtr l_pMaterial = m_pEngine->GetMaterialManager().find( p_strMaterialName );
	//	int l_iIndex = m_pListImages->GetImageCount();
	//	wxListItem l_item;
	//	l_item.SetColumn( 0 );
	//	l_item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE );
	//	l_item.SetId( l_iIndex );
	//	l_item.SetImage( l_iIndex );
	//	l_item.SetAlign( wxLIST_FORMAT_LEFT );
	//	l_item.SetText( p_strMaterialName );
	//	l_item.SetWidth( m_iImgWidth + 2 );
	//	wxImage * l_pImage = CreateMaterialImage( l_pMaterial, m_iImgWidth, m_iImgWidth );
	//	wxBitmap l_bitmap( * l_pImage );
	//	m_pListImages->Add( l_bitmap );
	//	m_arrayImages.push_back( l_pImage );
	//
	//	if ( InsertItem( l_item ) == -1 )
	//	{
	//		Logger::LogWarning( cuT( "wxMaterialsList::AddItem - Item not inserted" ) );
	//	}
	//	else
	//	{
	//		SetItemColumnImage( l_iIndex, 0, l_iIndex );
	//	}
	//}
	//
	//wxImage * wxMaterialsList::CreateMaterialImage( MaterialSPtr p_pMaterial, uint32_t p_uiWidth, uint32_t p_uiHeight )
	//{
	//	wxBitmap l_bmpReturn( p_uiWidth, p_uiHeight, 32 );
	//	wxMemoryDC l_dcReturn( l_bmpReturn );
	//
	//	if ( p_pMaterial )
	//	{
	//		if ( p_pMaterial->GetPassCount() )
	//		{
	//			wxImage * l_pPassImage = CreatePassImage( p_pMaterial->GetPass( 0 ), p_uiWidth, p_uiHeight );
	//			wxBitmap l_bmpPass( *l_pPassImage, 32 );
	//			l_dcReturn.DrawBitmap( l_bmpPass, 0, 0, true );
	//			delete l_pPassImage;
	//		}
	//	}
	//
	//	return new wxImage( l_bmpReturn.ConvertToImage() );
	//}
	//
	//wxImage * wxMaterialsList::CreatePassImage( PassSPtr p_pPass, uint32_t p_uiWidth, uint32_t p_uiHeight )
	//{
	//	wxImage * l_pReturn = NULL;
	//
	//	if ( p_pPass )
	//	{
	//		wxBitmap l_bmpReturn( p_uiWidth, p_uiHeight, 32 );
	//		wxMemoryDC l_dcReturn( l_bmpReturn );
	//		wxImage * l_pMask = NULL;
	//		typedef uint32_t uint;
	//		l_pMask = new wxImage( p_uiWidth, p_uiHeight );
	//		l_pMask->InitAlpha();
	//		uint8_t l_byRe;
	//		uint8_t l_byGe;
	//		uint8_t l_byBe;
	//		uint8_t l_byRm;
	//		uint8_t l_byGm;
	//		uint8_t l_byBm;
	//		uint8_t l_byRc;
	//		uint8_t l_byGc;
	//		uint8_t l_byBc;
	//		uint8_t l_byR;
	//		uint8_t l_byG;
	//		uint8_t l_byB;
	//		uint8_t l_byA = 255;
	//		double l_dMiddleX = double( p_uiWidth ) / 2.0;
	//		double l_dMiddleY = double( p_uiHeight ) / 2.0;
	//		double l_dMaxRadius = std::sqrt( ( l_dMiddleX * l_dMiddleX ) + ( l_dMiddleY * l_dMiddleY ) );
	//		double l_dMinRadius = 0.0;
	//		double l_dMidRadius1 = l_dMaxRadius / 3.0;
	//		double l_dMidRadius2 = l_dMidRadius1 * 2.0;
	//		double l_dStep = l_dMidRadius1;
	//		double l_dX, l_dY;
	//		double l_dRadius = 0;
	//		p_pPass->GetAmbient().red().convert_to( l_byRe );
	//		p_pPass->GetAmbient().green().convert_to( l_byGe );
	//		p_pPass->GetAmbient().blue().convert_to( l_byBe );
	//		p_pPass->GetDiffuse().red().convert_to( l_byRm );
	//		p_pPass->GetDiffuse().green().convert_to( l_byGm );
	//		p_pPass->GetDiffuse().blue().convert_to( l_byBm );
	//		p_pPass->GetSpecular().red().convert_to( l_byRc );
	//		p_pPass->GetSpecular().green().convert_to( l_byGc );
	//		p_pPass->GetSpecular().blue().convert_to( l_byBc );
	//
	//		if ( p_pPass->GetTextureUnitsCount() )
	//		{
	//			l_byA = 127;
	//			wxImage * l_pImage = CreateTextureUnitImage( p_pPass->GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE ), p_uiWidth, p_uiHeight );
	//
	//			if ( l_pImage )
	//			{
	//				wxBitmap l_bmpImage( *l_pImage );
	//				l_dcReturn.DrawBitmap( l_bmpImage, 0, 0, true );
	//				delete l_pImage;
	//			}
	//		}
	//
	//		for ( uint x = 0; x < p_uiWidth; ++x )
	//		{
	//			for ( uint y = 0; y < p_uiHeight; ++y )
	//			{
	//				l_dX = double( x ) - l_dMiddleX;
	//				l_dY = double( y ) - l_dMiddleY;
	//				l_dRadius = std::sqrt( ( l_dX * l_dX ) + ( l_dY * l_dY ) );
	//
	//				if ( l_dRadius < l_dMidRadius1 )
	//				{
	//					l_byR = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byRc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byRm ) / l_dStep ) );
	//					l_byG = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byGc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byGm ) / l_dStep ) );
	//					l_byB = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byBc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byBm ) / l_dStep ) );
	//					//l_byA = uint8_t( ((l_dMidRadius1 - l_dRadius) * double( l_byAc ) / l_dStep) + ((l_dRadius - l_dMinRadius) * double( l_byAm ) / l_dStep) );
	//				}
	//				else if ( l_dRadius < l_dMidRadius2 )
	//				{
	//					l_byR = l_byRm;
	//					l_byG = l_byGm;
	//					l_byB = l_byBm;
	//					//l_byA = l_byAm;
	//				}
	//				else
	//				{
	//					l_byR = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byRm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byRe ) / l_dStep ) );
	//					l_byG = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byGm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byGe ) / l_dStep ) );
	//					l_byB = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byBm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byBe ) / l_dStep ) );
	//					//l_byA = uint8_t( ((l_dMaxRadius - l_dRadius) * double( l_byAm ) / l_dStep) +  ((l_dRadius - l_dMidRadius2) * double( l_byAe ) / l_dStep) );
	//				}
	//
	//				l_pMask->SetRGB( x, y, l_byR, l_byG, l_byB );
	//				l_pMask->SetAlpha( x, y, l_byA / 2 );
	//			}
	//		}
	//
	//		wxBitmap l_bmpMask( *l_pMask );
	//		delete l_pMask;
	//		l_dcReturn.DrawBitmap( l_bmpMask, 0, 0, true );
	//		l_pReturn = new wxImage( l_bmpReturn.ConvertToImage() );
	//	}
	//
	//	return l_pReturn;
	//}
	//
	//wxImage * wxMaterialsList::CreateTextureUnitImage( TextureUnitSPtr p_pUnit, uint32_t p_uiWidth, uint32_t p_uiHeight )
	//{
	//	wxImage * l_pReturn = NULL;
	//
	//	if ( p_pUnit )
	//	{
	//		wxBitmap l_bmp;
	//		wxCreateBitmapFromBuffer( p_pUnit, l_bmp );
	//
	//		try
	//		{
	//			if ( l_bmp.IsOk() )
	//			{
	//				l_pReturn = new wxImage( l_bmp.ConvertToImage() );
	//				l_pReturn->Rescale( p_uiWidth, p_uiHeight, wxIMAGE_QUALITY_HIGHEST );
	//			}
	//		}
	//		catch ( ... )
	//		{
	//			Logger::LogWarning( cuT( "wxMaterialsList::CreateTextureUnitImage" ) );
	//		}
	//	}
	//
	//	return l_pReturn;
	//}
}
