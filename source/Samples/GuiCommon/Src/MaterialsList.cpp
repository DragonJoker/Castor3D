#include "MaterialsList.hpp"

#include "ImagesLoader.hpp"
#include "MaterialTreeItemProperty.hpp"
#include "PassTreeItemProperty.hpp"
#include "PropertiesHolder.hpp"
#include "RenderTargetTreeItemProperty.hpp"
#include "TextureTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Log/Logger.hpp>

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
	MaterialsList::MaterialsList( PropertiesHolder * p_propertiesHolder
		, wxWindow * p_parent
		, wxPoint const & p_ptPos
		, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_engine( nullptr )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor wait;
		ImagesLoader::AddBitmap( eBMP_MATERIAL, material_xpm );
		ImagesLoader::AddBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_PASS, pass_xpm );
		ImagesLoader::AddBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXTURE, texture_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		ImagesLoader::WaitAsyncLoads();

		wxImage * icons[] =
		{
			ImagesLoader::GetBitmap( eBMP_MATERIAL ),
			ImagesLoader::GetBitmap( eBMP_MATERIAL_SEL ),
			ImagesLoader::GetBitmap( eBMP_PASS ),
			ImagesLoader::GetBitmap( eBMP_PASS_SEL ),
			ImagesLoader::GetBitmap( eBMP_TEXTURE ),
			ImagesLoader::GetBitmap( eBMP_TEXTURE_SEL ),
		};

		wxImageList * imageList = new wxImageList( GC_IMG_SIZE, GC_IMG_SIZE, true );

		for ( auto image : icons )
		{
			int sizeOrig = image->GetWidth();

			if ( sizeOrig != GC_IMG_SIZE )
			{
				image->Rescale( GC_IMG_SIZE, GC_IMG_SIZE, wxIMAGE_QUALITY_HIGHEST );
			}

			imageList->Add( wxImage( *image ) );
		}

		AssignImageList( imageList );
	}

	MaterialsList::~MaterialsList()
	{
		DeleteAllItems();
	}

	void MaterialsList::LoadMaterials( Engine * p_engine
		, Scene & p_scene )
	{
		m_engine = p_engine;
		m_scene = &p_scene;
		wxTreeItemId root = AddRoot( _( "Root" ), eBMP_SCENE, eBMP_SCENE_SEL );
		auto lock = Castor::make_unique_lock( m_engine->GetMaterialCache() );

		for ( auto pair : m_engine->GetMaterialCache() )
		{
			DoAddMaterial( root, pair.second );
		}
	}

	void MaterialsList::UnloadMaterials()
	{
		DeleteAllItems();
	}

	void MaterialsList::DoAddMaterial( wxTreeItemId p_id
		, Castor3D::MaterialSPtr p_material )
	{
		wxTreeItemId id = AppendItem( p_id
			, p_material->GetName()
			, eBMP_MATERIAL - eBMP_MATERIAL
			, eBMP_MATERIAL_SEL - eBMP_MATERIAL
			, new MaterialTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_material ) );
		uint32_t index = 0;

		for ( auto pass : *p_material )
		{
			DoAddPass( id, index++, pass );
		}
	}

	void MaterialsList::DoAddPass( wxTreeItemId p_id
		, uint32_t p_index
		, Castor3D::PassSPtr p_pass )
	{
		wxTreeItemId id = AppendItem( p_id
			, wxString( _( "Pass " ) ) << p_index
			, eBMP_PASS - eBMP_MATERIAL
			, eBMP_PASS_SEL - eBMP_MATERIAL
			, new PassTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_pass
				, *m_scene ) );
		uint32_t index = 0;

		for ( auto unit : *p_pass )
		{
			DoAddTexture( id, index++, unit, p_pass->GetType() );
		}
	}

	void MaterialsList::DoAddTexture( wxTreeItemId p_id
		, uint32_t p_index
		, Castor3D::TextureUnitSPtr p_texture
		, Castor3D::MaterialType p_type )
	{
		wxTreeItemId id = AppendItem( p_id
			, wxString( _( "Texture Unit " ) ) << p_index
			, eBMP_TEXTURE - eBMP_MATERIAL
			, eBMP_TEXTURE_SEL - eBMP_MATERIAL
			, new TextureTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_texture
				, p_type ) );

		if ( p_texture->GetRenderTarget() )
		{
			RenderTargetSPtr target = p_texture->GetRenderTarget();

			if ( target )
			{
				wxString name = _( "Render Target" );
				AppendItem( id
					, name
					, eBMP_RENDER_TARGET
					, eBMP_RENDER_TARGET_SEL
					, new RenderTargetTreeItemProperty( m_propertiesHolder->IsEditable()
						, target ) );
			}
		}
	}

	BEGIN_EVENT_TABLE( MaterialsList, wxTreeCtrl )
		EVT_CLOSE( MaterialsList::OnClose )
		EVT_TREE_SEL_CHANGED( wxID_ANY, MaterialsList::OnSelectItem )
		EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, MaterialsList::OnMouseRButtonUp )
	END_EVENT_TABLE()

	void MaterialsList::OnClose( wxCloseEvent & p_event )
	{
		DeleteAllItems();
		p_event.Skip();
	}

	void MaterialsList::OnSelectItem( wxTreeEvent & p_event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		m_propertiesHolder->SetPropertyData( data );
		p_event.Skip();
	}

	void MaterialsList::OnMouseRButtonUp( wxTreeEvent & p_event )
	{
		TreeItemProperty * data = reinterpret_cast< TreeItemProperty * >( p_event.GetClientObject() );
		wxPoint position = wxGetMousePosition();
		data->DisplayTreeItemMenu( this, position.x, position.y );
	}

	//void MaterialsList::AddItem( String const & p_strMaterialName )
	//{
	//	MaterialSPtr pMaterial = m_engine->GetMaterialCache().find( p_strMaterialName );
	//	int iIndex = m_pListImages->GetImageCount();
	//	wxListItem item;
	//	item.SetColumn( 0 );
	//	item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE );
	//	item.SetId( iIndex );
	//	item.SetImage( iIndex );
	//	item.SetAlign( wxLIST_FORMAT_LEFT );
	//	item.SetText( p_strMaterialName );
	//	item.SetWidth( m_iImgWidth + 2 );
	//	wxImage * pImage = CreateMaterialImage( pMaterial, m_iImgWidth, m_iImgWidth );
	//	wxBitmap bitmap( * pImage );
	//	m_pListImages->Add( bitmap );
	//	m_arrayImages.push_back( pImage );
	//
	//	if ( InsertItem( item ) == -1 )
	//	{
	//		Logger::LogWarning( cuT( "MaterialsList::AddItem - Item not inserted" ) );
	//	}
	//	else
	//	{
	//		SetItemColumnImage( iIndex, 0, iIndex );
	//	}
	//}
	//
	//wxImage * MaterialsList::CreateMaterialImage( MaterialSPtr p_material, uint32_t p_width, uint32_t p_height )
	//{
	//	wxBitmap bmpReturn( p_width, p_height, 32 );
	//	wxMemoryDC dcReturn( bmpReturn );
	//
	//	if ( p_material )
	//	{
	//		if ( p_material->GetPassCount() )
	//		{
	//			wxImage * pPassImage = CreatePassImage( p_material->GetPass( 0 ), p_width, p_height );
	//			wxBitmap bmpPass( *pPassImage, 32 );
	//			dcReturn.DrawBitmap( bmpPass, 0, 0, true );
	//			delete pPassImage;
	//		}
	//	}
	//
	//	return new wxImage( bmpReturn.ConvertToImage() );
	//}
	//
	//wxImage * MaterialsList::CreatePassImage( PassSPtr p_pass, uint32_t p_width, uint32_t p_height )
	//{
	//	wxImage * result = nullptr;
	//
	//	if ( p_pass )
	//	{
	//		wxBitmap bmpReturn( p_width, p_height, 32 );
	//		wxMemoryDC dcReturn( bmpReturn );
	//		wxImage * pMask = nullptr;
	//		typedef uint32_t uint;
	//		pMask = new wxImage( p_width, p_height );
	//		pMask->InitAlpha();
	//		uint8_t byRe;
	//		uint8_t byGe;
	//		uint8_t byBe;
	//		uint8_t byRm;
	//		uint8_t byGm;
	//		uint8_t byBm;
	//		uint8_t byRc;
	//		uint8_t byGc;
	//		uint8_t byBc;
	//		uint8_t byR;
	//		uint8_t byG;
	//		uint8_t byB;
	//		uint8_t byA = 255;
	//		double dMiddleX = double( p_width ) / 2.0;
	//		double dMiddleY = double( p_height ) / 2.0;
	//		double dMaxRadius = std::sqrt( ( dMiddleX * dMiddleX ) + ( dMiddleY * dMiddleY ) );
	//		double dMinRadius = 0.0;
	//		double dMidRadius1 = dMaxRadius / 3.0;
	//		double dMidRadius2 = dMidRadius1 * 2.0;
	//		double dStep = dMidRadius1;
	//		double dX, dY;
	//		double dRadius = 0;
	//		p_pass->GetDiffuse().red().convert_to( byRm );
	//		p_pass->GetDiffuse().green().convert_to( byGm );
	//		p_pass->GetDiffuse().blue().convert_to( byBm );
	//		p_pass->GetSpecular().red().convert_to( byRc );
	//		p_pass->GetSpecular().green().convert_to( byGc );
	//		p_pass->GetSpecular().blue().convert_to( byBc );
	//
	//		if ( p_pass->GetTextureUnitsCount() )
	//		{
	//			byA = 127;
	//			wxImage * pImage = CreateTextureUnitImage( p_pass->GetTextureUnit( TextureChannel::eDiffuse ), p_width, p_height );
	//
	//			if ( pImage )
	//			{
	//				wxBitmap bmpImage( *pImage );
	//				dcReturn.DrawBitmap( bmpImage, 0, 0, true );
	//				delete pImage;
	//			}
	//		}
	//
	//		for ( uint x = 0; x < p_width; ++x )
	//		{
	//			for ( uint y = 0; y < p_height; ++y )
	//			{
	//				dX = double( x ) - dMiddleX;
	//				dY = double( y ) - dMiddleY;
	//				dRadius = std::sqrt( ( dX * dX ) + ( dY * dY ) );
	//
	//				if ( dRadius < dMidRadius1 )
	//				{
	//					byR = uint8_t( ( ( dMidRadius1 - dRadius ) * double( byRc ) / dStep ) + ( ( dRadius - dMinRadius ) * double( byRm ) / dStep ) );
	//					byG = uint8_t( ( ( dMidRadius1 - dRadius ) * double( byGc ) / dStep ) + ( ( dRadius - dMinRadius ) * double( byGm ) / dStep ) );
	//					byB = uint8_t( ( ( dMidRadius1 - dRadius ) * double( byBc ) / dStep ) + ( ( dRadius - dMinRadius ) * double( byBm ) / dStep ) );
	//					//l_byA = uint8_t( ((dMidRadius1 - dRadius) * double( byAc ) / dStep) + ((dRadius - dMinRadius) * double( byAm ) / dStep) );
	//				}
	//				else if ( dRadius < dMidRadius2 )
	//				{
	//					byR = byRm;
	//					byG = byGm;
	//					byB = byBm;
	//					//l_byA = byAm;
	//				}
	//				else
	//				{
	//					byR = uint8_t( ( ( dMaxRadius - dRadius ) * double( byRm ) / dStep ) + ( ( dRadius - dMidRadius2 ) * double( byRe ) / dStep ) );
	//					byG = uint8_t( ( ( dMaxRadius - dRadius ) * double( byGm ) / dStep ) + ( ( dRadius - dMidRadius2 ) * double( byGe ) / dStep ) );
	//					byB = uint8_t( ( ( dMaxRadius - dRadius ) * double( byBm ) / dStep ) + ( ( dRadius - dMidRadius2 ) * double( byBe ) / dStep ) );
	//					//l_byA = uint8_t( ((dMaxRadius - dRadius) * double( byAm ) / dStep) +  ((dRadius - dMidRadius2) * double( byAe ) / dStep) );
	//				}
	//
	//				pMask->SetRGB( x, y, byR, byG, byB );
	//				pMask->SetAlpha( x, y, byA / 2 );
	//			}
	//		}
	//
	//		wxBitmap bmpMask( *pMask );
	//		delete pMask;
	//		dcReturn.DrawBitmap( bmpMask, 0, 0, true );
	//		result = new wxImage( bmpReturn.ConvertToImage() );
	//	}
	//
	//	return result;
	//}
	//
	//wxImage * MaterialsList::CreateTextureUnitImage( TextureUnitSPtr p_pUnit, uint32_t p_width, uint32_t p_height )
	//{
	//	wxImage * result = nullptr;
	//
	//	if ( p_pUnit )
	//	{
	//		wxBitmap bmp;
	//		CreateBitmapFromBuffer( p_pUnit, bmp );
	//
	//		try
	//		{
	//			if ( bmp.IsOk() )
	//			{
	//				result = new wxImage( bmp.ConvertToImage() );
	//				result->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGHEST );
	//			}
	//		}
	//		catch ( ... )
	//		{
	//			Logger::LogWarning( cuT( "MaterialsList::CreateTextureUnitImage" ) );
	//		}
	//	}
	//
	//	return result;
	//}
}
