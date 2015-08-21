#include "MaterialsListView.hpp"
#include "PassPanel.hpp"

#include <Engine.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Pass.hpp>
#include <Logger.hpp>

#include <wx/imaglist.h>

#ifdef LoadImage
#	undef LoadImage
#	define LoadImage wxBitmap::LoadImage
#endif

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

wxMaterialsListView::wxMaterialsListView( int p_imgWidth, wxWindow * p_pParent, wxWindowID p_id, wxPoint const & p_ptPos, wxSize const & p_size )
#if defined( _WIN32 )
	: wxListView( p_pParent, p_id, p_ptPos, p_size, wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxBORDER_SIMPLE | wxLC_REPORT )
#else
	: wxListView( p_pParent, p_id, p_ptPos, p_size, wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxBORDER_SIMPLE | wxLC_REPORT )
#endif
	, m_pListImages( NULL )
	, m_uiNbItems( 0 )
	, m_iImgWidth( p_imgWidth )
	, m_pEngine( NULL )
{
}

wxMaterialsListView::~wxMaterialsListView()
{
	if ( m_pListImages )
	{
		m_pListImages->RemoveAll();
		delete m_pListImages;
		m_pListImages = NULL;
	}

	clear_container( m_arrayImages );
	m_uiNbItems = 0;
}

void wxMaterialsListView::CreateList( Castor3D::Engine * p_pEngine )
{
	m_pEngine = p_pEngine;
	StringArray l_arrayMaterialsNames;
	String l_strMaterialName;
	ClearAll();

	if ( m_pListImages )
	{
		m_pListImages->RemoveAll();
		delete m_pListImages;
		m_pListImages = NULL;
	}

	clear_container( m_arrayImages );
	m_pListImages = new wxImageList( m_iImgWidth, m_iImgWidth, false );
	SetImageList( m_pListImages, wxIMAGE_LIST_NORMAL );
	m_pEngine->GetMaterialManager().GetNames( l_arrayMaterialsNames );
	InsertColumn( 0, wxT( "Materials" ) );
	SetColumnWidth( 0, GetClientSize().x );
	std::for_each( l_arrayMaterialsNames.begin(), l_arrayMaterialsNames.end(), [&]( String const & p_strName )
	{
		AddItem( p_strName );
	} );
}

void wxMaterialsListView::AddItem( String const & p_strMaterialName )
{
	MaterialSPtr l_pMaterial = m_pEngine->GetMaterialManager().find( p_strMaterialName );
	int l_iIndex = m_pListImages->GetImageCount();
	wxListItem l_item;
	l_item.SetColumn( 0 );
	l_item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE );
	l_item.SetId( l_iIndex );
	l_item.SetImage( l_iIndex );
	l_item.SetAlign( wxLIST_FORMAT_LEFT );
	l_item.SetText( p_strMaterialName );
	l_item.SetWidth( m_iImgWidth + 2 );
	wxImage * l_pImage = CreateMaterialImage( l_pMaterial, m_iImgWidth, m_iImgWidth );
	wxBitmap l_bitmap( * l_pImage );
	m_pListImages->Add( l_bitmap );
	m_arrayImages.push_back( l_pImage );

	if ( InsertItem( l_item ) == -1 )
	{
		Logger::LogWarning( cuT( "wxMaterialsListView::AddItem - Item not inserted" ) );
	}
	else
	{
		SetItemColumnImage( l_iIndex, 0, l_iIndex );
	}
}

wxImage * wxMaterialsListView::CreateMaterialImage( MaterialSPtr p_pMaterial, uint32_t p_uiWidth, uint32_t p_uiHeight )
{
	wxBitmap l_bmpReturn( p_uiWidth, p_uiHeight, 32 );
	wxMemoryDC l_dcReturn( l_bmpReturn );

	if ( p_pMaterial )
	{
		if ( p_pMaterial->GetPassCount() )
		{
			wxImage * l_pPassImage = CreatePassImage( p_pMaterial->GetPass( 0 ), p_uiWidth, p_uiHeight );
			wxBitmap l_bmpPass( *l_pPassImage, 32 );
			l_dcReturn.DrawBitmap( l_bmpPass, 0, 0, true );
			delete l_pPassImage;
		}
	}

	return new wxImage( l_bmpReturn.ConvertToImage() );
}

wxImage * wxMaterialsListView::CreatePassImage( PassSPtr p_pPass, uint32_t p_uiWidth, uint32_t p_uiHeight )
{
	wxImage * l_pReturn = NULL;

	if ( p_pPass )
	{
		wxBitmap l_bmpReturn( p_uiWidth, p_uiHeight, 32 );
		wxMemoryDC l_dcReturn( l_bmpReturn );
		wxImage * l_pMask = NULL;
		typedef uint32_t uint;
		l_pMask = new wxImage( p_uiWidth, p_uiHeight );
		l_pMask->InitAlpha();
		uint8_t l_byRe;
		uint8_t l_byGe;
		uint8_t l_byBe;
		uint8_t l_byRm;
		uint8_t l_byGm;
		uint8_t l_byBm;
		uint8_t l_byRc;
		uint8_t l_byGc;
		uint8_t l_byBc;
		uint8_t l_byR;
		uint8_t l_byG;
		uint8_t l_byB;
		uint8_t l_byA = 255;
		double l_dMiddleX = double( p_uiWidth ) / 2.0;
		double l_dMiddleY = double( p_uiHeight ) / 2.0;
		double l_dMaxRadius = std::sqrt( ( l_dMiddleX * l_dMiddleX ) + ( l_dMiddleY * l_dMiddleY ) );
		double l_dMinRadius = 0.0;
		double l_dMidRadius1 = l_dMaxRadius / 3.0;
		double l_dMidRadius2 = l_dMidRadius1 * 2.0;
		double l_dStep = l_dMidRadius1;
		double l_dX, l_dY;
		double l_dRadius = 0;
		p_pPass->GetAmbient().red().convert_to( l_byRe );
		p_pPass->GetAmbient().green().convert_to( l_byGe );
		p_pPass->GetAmbient().blue().convert_to( l_byBe );
		p_pPass->GetDiffuse().red().convert_to( l_byRm );
		p_pPass->GetDiffuse().green().convert_to( l_byGm );
		p_pPass->GetDiffuse().blue().convert_to( l_byBm );
		p_pPass->GetSpecular().red().convert_to( l_byRc );
		p_pPass->GetSpecular().green().convert_to( l_byGc );
		p_pPass->GetSpecular().blue().convert_to( l_byBc );

		if ( p_pPass->GetTextureUnitsCount() )
		{
			l_byA = 127;
			wxImage * l_pImage = CreateTextureUnitImage( p_pPass->GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE ), p_uiWidth, p_uiHeight );

			if ( l_pImage )
			{
				wxBitmap l_bmpImage( *l_pImage );
				l_dcReturn.DrawBitmap( l_bmpImage, 0, 0, true );
				delete l_pImage;
			}
		}

		for ( uint x = 0; x < p_uiWidth; ++x )
		{
			for ( uint y = 0; y < p_uiHeight; ++y )
			{
				l_dX = double( x ) - l_dMiddleX;
				l_dY = double( y ) - l_dMiddleY;
				l_dRadius = std::sqrt( ( l_dX * l_dX ) + ( l_dY * l_dY ) );

				if ( l_dRadius < l_dMidRadius1 )
				{
					l_byR = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byRc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byRm ) / l_dStep ) );
					l_byG = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byGc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byGm ) / l_dStep ) );
					l_byB = uint8_t( ( ( l_dMidRadius1 - l_dRadius ) * double( l_byBc ) / l_dStep ) + ( ( l_dRadius - l_dMinRadius ) * double( l_byBm ) / l_dStep ) );
					//l_byA = uint8_t( ((l_dMidRadius1 - l_dRadius) * double( l_byAc ) / l_dStep) + ((l_dRadius - l_dMinRadius) * double( l_byAm ) / l_dStep) );
				}
				else if ( l_dRadius < l_dMidRadius2 )
				{
					l_byR = l_byRm;
					l_byG = l_byGm;
					l_byB = l_byBm;
					//l_byA = l_byAm;
				}
				else
				{
					l_byR = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byRm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byRe ) / l_dStep ) );
					l_byG = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byGm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byGe ) / l_dStep ) );
					l_byB = uint8_t( ( ( l_dMaxRadius - l_dRadius ) * double( l_byBm ) / l_dStep ) + ( ( l_dRadius - l_dMidRadius2 ) * double( l_byBe ) / l_dStep ) );
					//l_byA = uint8_t( ((l_dMaxRadius - l_dRadius) * double( l_byAm ) / l_dStep) +  ((l_dRadius - l_dMidRadius2) * double( l_byAe ) / l_dStep) );
				}

				l_pMask->SetRGB( x, y, l_byR, l_byG, l_byB );
				l_pMask->SetAlpha( x, y, l_byA / 2 );
			}
		}

		wxBitmap l_bmpMask( *l_pMask );
		delete l_pMask;
		l_dcReturn.DrawBitmap( l_bmpMask, 0, 0, true );
		l_pReturn = new wxImage( l_bmpReturn.ConvertToImage() );
	}

	return l_pReturn;
}

wxImage * wxMaterialsListView::CreateTextureUnitImage( TextureUnitSPtr p_pUnit, uint32_t p_uiWidth, uint32_t p_uiHeight )
{
	wxImage * l_pReturn = NULL;

	if ( p_pUnit )
	{
		wxBitmap l_bmp;
		wxCreateBitmapFromBuffer( p_pUnit, l_bmp );

		try
		{
			if ( l_bmp.IsOk() )
			{
				l_pReturn = new wxImage( l_bmp.ConvertToImage() );
				l_pReturn->Rescale( p_uiWidth, p_uiHeight, wxIMAGE_QUALITY_HIGHEST );
			}
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "wxMaterialsListView::CreateTextureUnitImage" ) );
		}
	}

	return l_pReturn;
}
