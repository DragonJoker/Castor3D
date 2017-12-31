#include "MaterialsList.hpp"

#include "ImagesLoader.hpp"
#include "MaterialTreeItemProperty.hpp"
#include "PassTreeItemProperty.hpp"
#include "PropertiesContainer.hpp"
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

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	MaterialsList::MaterialsList( PropertiesContainer * p_propertiesHolder
		, wxWindow * p_parent
		, wxPoint const & p_ptPos
		, wxSize const & p_size )
		: wxTreeCtrl( p_parent, wxID_ANY, p_ptPos, p_size, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxNO_BORDER )
		, m_propertiesHolder( p_propertiesHolder )
	{
		wxBusyCursor wait;
		ImagesLoader::addBitmap( eBMP_MATERIAL, material_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		ImagesLoader::addBitmap( eBMP_PASS, pass_xpm );
		ImagesLoader::addBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE, texture_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		ImagesLoader::waitAsyncLoads();

		wxImage * icons[] =
		{
			ImagesLoader::getBitmap( eBMP_MATERIAL ),
			ImagesLoader::getBitmap( eBMP_MATERIAL_SEL ),
			ImagesLoader::getBitmap( eBMP_PASS ),
			ImagesLoader::getBitmap( eBMP_PASS_SEL ),
			ImagesLoader::getBitmap( eBMP_TEXTURE ),
			ImagesLoader::getBitmap( eBMP_TEXTURE_SEL ),
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

	void MaterialsList::LoadMaterials( Engine * engine
		, Scene & p_scene )
	{
		m_engine = engine;
		m_scene = &p_scene;
		wxTreeItemId root = AddRoot( _( "Root" ), eBMP_SCENE, eBMP_SCENE_SEL );
		auto lock = castor::makeUniqueLock( m_engine->getMaterialCache() );

		for ( auto pair : m_engine->getMaterialCache() )
		{
			doAddMaterial( root, pair.second );
		}
	}

	void MaterialsList::UnloadMaterials()
	{
		DeleteAllItems();
	}

	void MaterialsList::doAddMaterial( wxTreeItemId p_id
		, castor3d::MaterialSPtr p_material )
	{
		wxTreeItemId id = AppendItem( p_id
			, p_material->getName()
			, eBMP_MATERIAL - eBMP_MATERIAL
			, eBMP_MATERIAL_SEL - eBMP_MATERIAL
			, new MaterialTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_material ) );
		uint32_t index = 0;

		for ( auto pass : *p_material )
		{
			doAddPass( id, index++, pass );
		}
	}

	void MaterialsList::doAddPass( wxTreeItemId p_id
		, uint32_t p_index
		, castor3d::PassSPtr p_pass )
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
			doAddTexture( id, index++, unit, p_pass->getType() );
		}
	}

	void MaterialsList::doAddTexture( wxTreeItemId p_id
		, uint32_t p_index
		, castor3d::TextureUnitSPtr p_texture
		, castor3d::MaterialType p_type )
	{
		wxTreeItemId id = AppendItem( p_id
			, wxString( _( "Texture Unit " ) ) << p_index
			, eBMP_TEXTURE - eBMP_MATERIAL
			, eBMP_TEXTURE_SEL - eBMP_MATERIAL
			, new TextureTreeItemProperty( m_propertiesHolder->IsEditable()
				, p_texture
				, p_type ) );

		if ( p_texture->getRenderTarget() )
		{
			RenderTargetSPtr target = p_texture->getRenderTarget();

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
		m_propertiesHolder->setPropertyData( data );
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
	//	MaterialSPtr pMaterial = m_engine->getMaterialCache().find( p_strMaterialName );
	//	int iIndex = m_pListImages->getImageCount();
	//	wxListItem item;
	//	item.setColumn( 0 );
	//	item.setMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE );
	//	item.setId( iIndex );
	//	item.setImage( iIndex );
	//	item.setAlign( wxLIST_FORMAT_LEFT );
	//	item.setText( p_strMaterialName );
	//	item.setWidth( m_iImgWidth + 2 );
	//	wxImage * pImage = CreateMaterialImage( pMaterial, m_iImgWidth, m_iImgWidth );
	//	wxBitmap bitmap( * pImage );
	//	m_pListImages->add( bitmap );
	//	m_arrayImages.push_back( pImage );
	//
	//	if ( InsertItem( item ) == -1 )
	//	{
	//		Logger::logWarning( cuT( "MaterialsList::AddItem - Item not inserted" ) );
	//	}
	//	else
	//	{
	//		setItemColumnImage( iIndex, 0, iIndex );
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
	//		if ( p_material->getPassCount() )
	//		{
	//			wxImage * pPassImage = CreatePassImage( p_material->getPass( 0 ), p_width, p_height );
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
	//		p_pass->getDiffuse().red().convertTo( byRm );
	//		p_pass->getDiffuse().green().convertTo( byGm );
	//		p_pass->getDiffuse().blue().convertTo( byBm );
	//		p_pass->getSpecular().red().convertTo( byRc );
	//		p_pass->getSpecular().green().convertTo( byGc );
	//		p_pass->getSpecular().blue().convertTo( byBc );
	//
	//		if ( p_pass->getTextureUnitsCount() )
	//		{
	//			byA = 127;
	//			wxImage * pImage = CreateTextureUnitImage( p_pass->getTextureUnit( TextureChannel::eDiffuse ), p_width, p_height );
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
	//				pMask->setRGB( x, y, byR, byG, byB );
	//				pMask->setAlpha( x, y, byA / 2 );
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
	//			if ( bmp.isOk() )
	//			{
	//				result = new wxImage( bmp.ConvertToImage() );
	//				result->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGHEST );
	//			}
	//		}
	//		catch ( ... )
	//		{
	//			Logger::logWarning( cuT( "MaterialsList::CreateTextureUnitImage" ) );
	//		}
	//	}
	//
	//	return result;
	//}
}
