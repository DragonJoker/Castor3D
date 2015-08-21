#include "PassPanel.hpp"
#include "ShaderDialog.hpp"

#include <InitialiseEvent.hpp>
#include <StaticTexture.hpp>

#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <Image.hpp>

#include <wx/colordlg.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/rawbmp.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	typedef enum eID
	{
		eID_BUTTON_DELETE,
		eID_BUTTON_AMBIENT,
		eID_BUTTON_DIFFUSE,
		eID_BUTTON_SPECULAR,
		eID_BUTTON_EMISSIVE,
		eID_BUTTON_TEXTURE_IMAGE,
		eID_BUTTON_HAS_SHADER,
		eID_EDIT_SHININESS,
		eID_EDIT_TEXTURE_PATH,
		eID_COMBO_TEXTURE_UNITS,
		eID_CHECK_DOUBLE_FACE,
		eID_RADIO_MAP_MODE,
		eID_SLIDER_ALPHA,
		eID_COMBO_TEXTURE_CHANNEL,
		eID_PROPGRID_PASS,
	}	eID;

	wxPassPanel::wxPassPanel( bool p_small, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPanel( p_pParent, wxID_ANY, p_ptPos, p_size,wxBORDER_NONE )
		, m_bCanEdit( p_bCanEdit )
		, m_pEngine( NULL )
	{
		wxSize l_sizeClrBtn( 50, 50 );
		wxSize l_sizeTextureImg( 70, 70 );

		if ( p_small )
		{
			l_sizeClrBtn = wxSize( 50, 20 );
			l_sizeTextureImg = wxSize( 50, 50 );
		}

		m_sizeImage = wxSize( l_sizeTextureImg.x - 2, l_sizeTextureImg.y - 2 );
		m_pButtonShader = new wxButton( this, eID_BUTTON_HAS_SHADER, _( "Edit shaders" ) );

		m_propertyGridPass = new wxPropertyGrid( this, eID_PROPGRID_PASS, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE );
		m_propertyGridPass->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );

		m_propertyGridPass->Append( new wxPropertyCategory( _( "Pass" ) ) );
		m_propertyGridPass->Append( m_propertyAmbient = new wxColourProperty( _( "Ambient" ) ) );
		m_propertyGridPass->Append( m_propertyDiffuse = new wxColourProperty( _( "Diffuse" ) ) );
		m_propertyGridPass->Append( m_propertySpecular = new wxColourProperty( _( "Specular" ) ) );
		m_propertyGridPass->Append( m_propertyEmissive = new wxColourProperty( _( "Emissive" ) ) );
		m_propertyGridPass->Append( m_propertyExponent = new wxFloatProperty( _( "Exponent" ) ) );
		m_propertyGridPass->Append( m_propertyTwoSided = new wxBoolProperty( _( "Two sided" ) ) );
		m_propertyGridPass->Append( m_propertyOpacity = new wxFloatProperty( _( "Opacity" ) ) );
		m_propertyGridPass->Append( m_propertyTextures = new wxEnumProperty( _( "Textures" ) ) );

		m_propertyGridPass->Append( new wxPropertyCategory( _( "Texture Unit" ) ) );
		m_propertyGridPass->Append( m_propertyImage = new wxImageFileProperty( _( "Texture" ) ) );
		m_propertyGridPass->Append( m_propertyChannel = new wxEnumProperty( _( "Channel" ) ) );

		// The whole panel
		wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
		l_pSizer->Add( m_pButtonShader, wxSizerFlags( 0 ).Border( wxBOTTOM, 5 ) );
		l_pSizer->Add( m_propertyGridPass, wxSizerFlags( 1 ).Expand().Border( wxTOP, 5 ) );
		SetSizer( l_pSizer );
		l_pSizer->SetSizeHints( this );
	}

	wxPassPanel::~wxPassPanel()
	{
		m_pTextureUnit.reset();
	}

	void wxPassPanel::SetPass( Engine * p_pEngine, PassWPtr p_wpPass )
	{
		m_pEngine = p_pEngine;
		m_wpPass = p_wpPass;
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			m_pTextureUnit.reset();
			m_uiSelectedUnitIndex = 0xFFFFFFFF;
			m_propertyAmbient->SetValue( wxVariant( wxColour( l_pPass->GetAmbient().to_bgr() ) ) );
			m_propertyDiffuse->SetValue( wxVariant( wxColour( l_pPass->GetDiffuse().to_bgr() ) ) );
			m_propertySpecular->SetValue( wxVariant( wxColour( l_pPass->GetSpecular().to_bgr() ) ) );
			m_propertyEmissive->SetValue( wxVariant( wxColour( l_pPass->GetEmissive().to_bgr() ) ) );
			m_propertyExponent->SetValue( l_pPass->GetShininess() );
			m_propertyTwoSided->SetValue( l_pPass->IsTwoSided() );
			
			DoFillTexturesProperty( l_pPass->GetTextureUnitsCount() );

			if ( l_pPass->GetTextureUnitsCount() > 0 )
			{
				DoSelectTextureUnit( 0 );
			}
			else
			{
				DoShowTextureFields( m_bCanEdit );
			}

			DoShowPassFields( true );
		}
	}

	void wxPassPanel::GetDiffuse( Colour & clrColour )const
	{
		wxColour l_colour = m_propertyDiffuse->GetValue();
		clrColour = Colour::from_components( l_colour.Red(), l_colour.Green(), l_colour.Blue(), l_colour.Alpha() );
	}

	void wxPassPanel::GetAmbient( Colour & clrColour )const
	{
		wxColour l_colour = m_propertyAmbient->GetValue();
		clrColour = Colour::from_components( l_colour.Red(), l_colour.Green(), l_colour.Blue(), l_colour.Alpha() );
	}

	void wxPassPanel::GetEmissive( Colour & clrColour )const
	{
		wxColour l_colour = m_propertySpecular->GetValue();
		clrColour = Colour::from_components( l_colour.Red(), l_colour.Green(), l_colour.Blue(), l_colour.Alpha() );
	}

	void wxPassPanel::GetSpecular( Colour & clrColour )const
	{
		wxColour l_colour = m_propertyEmissive->GetValue();
		clrColour = Colour::from_components( l_colour.Red(), l_colour.Green(), l_colour.Blue(), l_colour.Alpha() );
	}

	real wxPassPanel::GetShininess()const
	{
		real l_return = m_propertyGridPass->GetPropertyValueAsDouble( m_propertyExponent );

		if ( l_return < 0 )
		{
			l_return = 0;
		}

		if ( l_return > 128 )
		{
			l_return = 128;
		}

		return l_return;
	}

	int wxPassPanel::GetTextureUnitIndex()const
	{
		int l_selected = m_propertyTextures->GetChoiceSelection();
		int l_iReturn = -2;

		if ( l_selected < int( m_wpPass.lock()->GetTextureUnitsCount() ) )
		{
			l_iReturn = l_selected;
		}
		else
		{
			l_iReturn = -1;
		}

		return l_iReturn;
	}

	void wxPassPanel::SetMaterialImage( uint32_t p_uiIndex, uint32_t p_uiWidth, uint32_t p_uiHeight )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass && p_uiIndex < l_pPass->GetTextureUnitsCount() )
		{
			SetMaterialImage( l_pPass->GetTextureUnit( p_uiIndex ), p_uiWidth, p_uiHeight );
		}
	}

	void wxPassPanel::SetMaterialImage( TextureUnitSPtr p_pTexture, uint32_t p_uiWidth, uint32_t p_uiHeight )
	{
		if ( p_pTexture )
		{
			wxBitmap l_bmp;
			wxCreateBitmapFromBuffer( p_pTexture, l_bmp );

			try
			{
				if ( l_bmp.IsOk() )
				{
					m_imgSelected = l_bmp.ConvertToImage();
					m_imgSelected.Rescale( p_uiWidth, p_uiHeight, wxIMAGE_QUALITY_HIGHEST );
				}
			}
			catch ( ... )
			{
				Logger::LogWarning( cuT( "wxPassPanel::SetMaterialImage" ) );
			}
		}
	}

	void wxPassPanel::DoSelectTextureUnit( int p_iIndex )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			if ( p_iIndex < int( l_pPass->GetTextureUnitsCount() ) && p_iIndex >= -1 )
			{
				DoShowTextureFields( true );
				wxString l_strValue;

				if ( p_iIndex == -1 )
				{
					DoFillTexturesProperty( l_pPass->GetTextureUnitsCount() + 1 );
					m_uiSelectedUnitIndex = uint32_t( l_pPass->GetTextureUnitsCount() );
					m_pTextureUnit = l_pPass->AddTextureUnit();
				}
				else
				{
					m_uiSelectedUnitIndex = static_cast< uint32_t >( p_iIndex );
					m_pTextureUnit = l_pPass->GetTextureUnit( m_uiSelectedUnitIndex );
				}

				l_strValue << m_uiSelectedUnitIndex;
				//m_pComboTextures->SetValue( l_strValue );

				if ( !m_pTextureUnit.expired() )
				{
					if ( p_iIndex == -1 || m_pTextureUnit.lock()->GetTexturePath().empty() )
					{
						m_imgSelected.Create( m_sizeImage.x, m_sizeImage.y, true );
					}
					else
					{
						SetMaterialImage( p_iIndex, m_sizeImage.x, m_sizeImage.y );
					}

					wxPGChoices l_choices;
					l_choices.Add( _( "Colour" ) ); //	eTEXTURE_CHANNEL_COLOUR		= 0x00000001	//!< Colour map
					l_choices.Add( _( "Diffuse" ) ); //	eTEXTURE_CHANNEL_DIFFUSE	= 0x00000002	//!< Diffuse map
					l_choices.Add( _( "Normal" ) ); //	eTEXTURE_CHANNEL_NORMAL		= 0x00000004	//!< Normal map
					l_choices.Add( _( "Opacity" ) ); //	eTEXTURE_CHANNEL_OPACITY	= 0x00000008	//!< Opacity map
					l_choices.Add( _( "Specular" ) );//	eTEXTURE_CHANNEL_SPECULAR	= 0x00000010	//!< Specular map
					l_choices.Add( _( "Height" ) ); //	eTEXTURE_CHANNEL_HEIGHT		= 0x00000020	//!< Height map
					l_choices.Add( _( "Ambient" ) ); //	eTEXTURE_CHANNEL_AMBIENT	= 0x00000040	//!< Ambient map
					l_choices.Add( _( "Gloss" ) ); //	eTEXTURE_CHANNEL_GLOSS		= 0x00000080	//!< Gloss map
					int l_selected;

					switch ( m_pTextureUnit.lock()->GetChannel() )
					{
					case eTEXTURE_CHANNEL_COLOUR:
						l_selected = 0;
						break;

					case eTEXTURE_CHANNEL_DIFFUSE:
						l_selected = 1;
						break;

					case eTEXTURE_CHANNEL_NORMAL:
						l_selected = 2;
						break;

					case eTEXTURE_CHANNEL_OPACITY:
						l_selected = 3;
						break;

					case eTEXTURE_CHANNEL_SPECULAR:
						l_selected = 4;
						break;

					case eTEXTURE_CHANNEL_HEIGHT:
						l_selected = 5;
						break;

					case eTEXTURE_CHANNEL_AMBIENT:
						l_selected = 6;
						break;

					case eTEXTURE_CHANNEL_GLOSS:
						l_selected = 7;
						break;
					}

					m_propertyChannel->SetChoices( l_choices );
				}
			}
			else
			{
				DoShowTextureFields( false );
			}
		}
	}

	void wxPassPanel::DoShowPassFields( bool p_bShow )
	{
		m_propertyGridPass->Show( p_bShow );
		m_pButtonShader->Show( p_bShow );
	}

	void wxPassPanel::DoShowTextureFields( bool p_bShow )
	{
		m_propertyTextures->Enable( p_bShow );
		m_propertyChannel->Enable( p_bShow );
		m_propertyImage->Enable( p_bShow );
	}

	void wxPassPanel::DoFillTexturesProperty( uint32_t p_count )
	{
		wxPGChoices l_units;

		for ( uint32_t i = 0; i < p_count; i++ )
		{
			wxString l_index;
			l_index << i;
			l_units.Add( l_index );
		}

		if ( m_bCanEdit )
		{
			l_units.Add( wxCOMBO_NEW );
		}

		m_propertyTextures->SetChoices( l_units );
	}

	BEGIN_EVENT_TABLE( wxPassPanel, wxPanel )
		EVT_BUTTON( eID_BUTTON_DELETE, wxPassPanel::OnDeleteTextureUnit )
		EVT_BUTTON( eID_BUTTON_AMBIENT, wxPassPanel::OnAmbientColour )
		EVT_BUTTON( eID_BUTTON_DIFFUSE, wxPassPanel::OnDiffuseColour )
		EVT_BUTTON( eID_BUTTON_SPECULAR, wxPassPanel::OnSpecularColour )
		EVT_BUTTON( eID_BUTTON_EMISSIVE, wxPassPanel::OnEmissiveColour )
		EVT_BUTTON( eID_BUTTON_TEXTURE_IMAGE, wxPassPanel::OnTextureImage )
		EVT_BUTTON( eID_BUTTON_HAS_SHADER, wxPassPanel::OnHasShader )
		EVT_TEXT_ENTER( eID_EDIT_SHININESS, wxPassPanel::OnShininess )
		EVT_TEXT_ENTER( eID_EDIT_TEXTURE_PATH, wxPassPanel::OnTexturePath )
		EVT_COMBOBOX( eID_COMBO_TEXTURE_UNITS, wxPassPanel::OnTextureSelect )
		EVT_CHECKBOX( eID_CHECK_DOUBLE_FACE, wxPassPanel::OnDoubleFace )
		EVT_COMBOBOX( eID_COMBO_TEXTURE_CHANNEL, wxPassPanel::OnTextureChannel )
		EVT_SLIDER( eID_SLIDER_ALPHA, wxPassPanel::OnAlpha )
	END_EVENT_TABLE()

	void wxPassPanel::OnDeleteTextureUnit( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			l_pPass->DestroyTextureUnit( m_uiSelectedUnitIndex );
			l_pPass->Initialise();
			DoFillTexturesProperty( l_pPass->GetTextureUnitsCount() );
			DoSelectTextureUnit( -2 );
		}

		p_event.Skip();
	}

	void wxPassPanel::OnAmbientColour( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass && m_bCanEdit )
		{
			wxColourDialog l_colourDialor( this );

			if ( l_colourDialor.ShowModal() )
			{
				wxColourData l_retData = l_colourDialor.GetColourData();
				m_propertyAmbient->SetValue( wxVariant( l_retData.GetColour() ) );
				Colour l_clrColour;
				GetAmbient( l_clrColour );
				l_pPass->SetAmbient( l_clrColour );
			}
		}

		p_event.Skip();
	}

	void wxPassPanel::OnDiffuseColour( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass && m_bCanEdit )
		{
			wxColourDialog l_colourDialor( this );

			if ( l_colourDialor.ShowModal() )
			{
				wxColourData l_retData = l_colourDialor.GetColourData();
				m_propertyDiffuse->SetValue( wxVariant( l_retData.GetColour() ) );
				Colour l_clrColour;
				GetDiffuse( l_clrColour );
				l_pPass->SetDiffuse( l_clrColour );
			}
		}

		p_event.Skip();
	}

	void wxPassPanel::OnSpecularColour( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass && m_bCanEdit )
		{
			wxColourDialog l_colourDialor( this );

			if ( l_colourDialor.ShowModal() )
			{
				wxColourData l_retData = l_colourDialor.GetColourData();
				m_propertySpecular->SetValue( wxVariant( l_retData.GetColour() ) );
				Colour l_clrColour;
				GetSpecular( l_clrColour );
				l_pPass->SetSpecular( l_clrColour );
			}
		}

		p_event.Skip();
	}

	void wxPassPanel::OnEmissiveColour( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass && m_bCanEdit )
		{
			wxColourDialog l_colourDialor( this );

			if ( l_colourDialor.ShowModal() )
			{
				wxColourData l_retData = l_colourDialor.GetColourData();
				m_propertyEmissive->SetValue( wxVariant( l_retData.GetColour() ) );
				Colour l_clrColour;
				GetEmissive( l_clrColour );
				l_pPass->SetEmissive( l_clrColour );
			}
		}

		p_event.Skip();
	}

	void wxPassPanel::OnTextureImage( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			wxString l_strWildcard = _( "BITMAP images (*.bmp)|*.bmp|GIF images (*.gif)|*.gif|JPEG images (*.jpg)|*.jpg|PNG images (*.png)|*.png|TARGA images (*.tga)|*.tga|All images (*.bmp;*.gif;*.png;*.jpg;*.tga)|*.bmp;*.gif;*.png;*.jpg;*.tga" );
			wxFileDialog l_fdlg( this, _( "Choose an image" ), wxEmptyString, wxEmptyString, l_strWildcard );

			if ( l_fdlg.ShowModal() == wxID_OK )
			{
				wxString l_strPathImage = l_fdlg.GetPath();

				if ( !m_pTextureUnit.expired() )
				{
					Path l_pathImage = ( wxChar const * )l_strPathImage.c_str();
					Collection< Image, String > l_imgCollection;
					ImageSPtr l_pImage = l_imgCollection.find( l_pathImage );

					if ( !l_pImage )
					{
						l_pImage = std::make_shared< Image >( l_pathImage, l_pathImage );
						l_imgCollection.insert( l_pathImage, l_pImage );
					}

					StaticTextureSPtr l_pTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
					l_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
					l_pTexture->SetImage( l_pImage->GetPixels() );
					m_pTextureUnit.lock()->SetTexture( l_pTexture );
					SetMaterialImage( m_pTextureUnit.lock(), 78, 78 );
					m_pEngine->PostEvent( MakeInitialiseEvent( *l_pPass->GetParent() ) );
				}
			}
		}

		p_event.Skip();
	}

	void wxPassPanel::OnShininess( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			real l_shininess = GetShininess();
			l_pPass->SetShininess( l_shininess );
			l_pPass->Initialise();
		}

		p_event.Skip();
	}

	void wxPassPanel::OnTexturePath( wxCommandEvent & p_event )
	{
		p_event.Skip();
	}

	void wxPassPanel::OnTextureSelect( wxCommandEvent & p_event )
	{
		DoSelectTextureUnit( GetTextureUnitIndex() );
		p_event.Skip();
	}

	void wxPassPanel::OnDoubleFace( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			bool l_bDouble = m_propertyTwoSided->GetValue();
			l_pPass->SetTwoSided( l_bDouble );
		}

		p_event.Skip();
	}

	void wxPassPanel::OnHasShader( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			wxShaderDialog * l_dialog = new wxShaderDialog( m_pEngine, m_bCanEdit, this, l_pPass );
			l_dialog->Show();
		}

		p_event.Skip();
	}

	void wxPassPanel::OnTextureChannel( wxCommandEvent & p_event )
	{
		switch ( p_event.GetInt() )
		{
		case 0	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_COLOUR );
			break;

		case 1	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_DIFFUSE );
			break;

		case 2	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_NORMAL );
			break;

		case 3	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_OPACITY );
			break;

		case 4	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_SPECULAR );
			break;

		case 5	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_HEIGHT );
			break;

		case 6	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_AMBIENT );
			break;

		case 7	:
			m_pTextureUnit.lock()->SetChannel( eTEXTURE_CHANNEL_GLOSS );
			break;
		}

		p_event.Skip();
	}

	void wxPassPanel::OnAlpha( wxCommandEvent & p_event )
	{
		PassSPtr l_pPass = m_wpPass.lock();

		if ( l_pPass )
		{
			l_pPass->SetAlpha( real( p_event.GetInt() ) / real( 255 ) );
		}

		p_event.Skip();
	}

	//*************************************************************************************************

	void wxCreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap )
	{
		p_bitmap.Create( p_uiWidth, p_uiHeight, 24 );
		wxNativePixelData l_data( p_bitmap );

		if ( p_bitmap.IsOk() && uint32_t( l_data.GetWidth() ) == p_uiWidth && uint32_t( l_data.GetHeight() ) == p_uiHeight )
		{
			wxNativePixelData::Iterator l_it( l_data );
			uint8_t const * l_pBuffer = p_pBuffer;

			try
			{
				for ( uint32_t i = 0; i < p_uiHeight && l_it.IsOk(); i++ )
				{
	#if defined( _WIN32 )
					wxNativePixelData::Iterator l_rowStart = l_it;
	#endif

					for ( uint32_t j = 0; j < p_uiWidth && l_it.IsOk(); j++ )
					{
						l_it.Blue()		= *l_pBuffer;
						l_pBuffer++;
						l_it.Green()	= *l_pBuffer;
						l_pBuffer++;
						l_it.Red()		= *l_pBuffer;
						l_pBuffer++;
						l_pBuffer++;
						l_it++;
					}

	#if defined( _WIN32 )
					l_it = l_rowStart;
					l_it.OffsetY( l_data, 1 );
	#endif
				}
			}
			catch ( ... )
			{
				Logger::LogWarning( cuT( "wxCreateBitmapFromBuffer encountered an exception" ) );
			}
		}
	}

	void wxCreateBitmapFromBuffer( TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap )
	{
		if ( p_pUnit->GetImageBuffer() )
		{
			PxBufferBaseSPtr l_pBuffer = PxBufferBase::create( Size( p_pUnit->GetWidth(), p_pUnit->GetHeight() ), ePIXEL_FORMAT_A8R8G8B8, p_pUnit->GetImageBuffer(), p_pUnit->GetPixelFormat() );
			wxCreateBitmapFromBuffer( l_pBuffer->const_ptr(), l_pBuffer->width(), l_pBuffer->height(), p_bitmap );
		}
		else if ( !p_pUnit->GetTexturePath().empty() )
		{
			wxImageHandler * l_pHandler = wxImage::FindHandler( p_pUnit->GetTexturePath().GetExtension(), wxBITMAP_TYPE_ANY );

			if ( l_pHandler )
			{
				wxImage l_image;

				if ( l_image.LoadFile( p_pUnit->GetTexturePath(), l_pHandler->GetType() ) && l_image.IsOk() )
				{
					p_bitmap = wxBitmap( l_image );
				}
				else
				{
					Logger::LogWarning( cuT( "wxCreateBitmapFromBuffer encountered a problem loading file [" ) + p_pUnit->GetTexturePath() + cuT( "]" ) );
				}
			}
			else
			{
				Logger::LogWarning( cuT( "wxCreateBitmapFromBuffer encountered a problem loading file [" ) + p_pUnit->GetTexturePath() + cuT( "] : Unsupported format" ) );
			}
		}
	}
}
