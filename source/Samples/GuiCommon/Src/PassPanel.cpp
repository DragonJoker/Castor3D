#include "PassPanel.hpp"
#include "ShaderDialog.hpp"

#include <InitialiseEvent.hpp>
#include <StaticTexture.hpp>

#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <Image.hpp>

#include <wx/colordlg.h>
#include <wx/rawbmp.h>

using namespace Castor3D;
using namespace GuiCommon;
using namespace Castor;

//*************************************************************************************************

wxPassPanel::wxPassPanel( wxWindow * p_pParent, Engine * p_pEngine, bool p_bCanEdit, wxPoint const & p_ptPos, wxSize const & p_size )
	:	wxScrolledWindow( p_pParent, wxID_ANY, p_ptPos, p_size, 524288 | wxBORDER_NONE )
	,	m_pPanelTextureUnit( NULL	)
	,	m_bCanEdit( p_bCanEdit	)
	,	m_pEngine( p_pEngine	)
{
	wxSize l_sizeClrBtn( 50, 50 );
	wxSize l_sizeTextureImg( 70, 70 );
	m_sizeImage = wxSize( l_sizeTextureImg.x - 2, l_sizeTextureImg.y - 2 );
	wxArrayString l_arrayModes;
	l_arrayModes.push_back( _( "Normal" ) );
	l_arrayModes.push_back( _( "Reflexion map" ) );
	l_arrayModes.push_back( _( "Sphere map" ) );
	wxArrayString l_arrayChannels;
	l_arrayChannels.push_back( _( "Colour" ) ); //	eTEXTURE_CHANNEL_COLOUR		= 0x00000001	//!< Colour map
	l_arrayChannels.push_back( _( "Diffuse" ) ); //	eTEXTURE_CHANNEL_DIFFUSE	= 0x00000002	//!< Diffuse map
	l_arrayChannels.push_back( _( "Normal" ) ); //	eTEXTURE_CHANNEL_NORMAL		= 0x00000004	//!< Normal map
	l_arrayChannels.push_back( _( "Opacity" ) ); //	eTEXTURE_CHANNEL_OPACITY	= 0x00000008	//!< Opacity map
	l_arrayChannels.push_back( _( "Specular" ) );//	eTEXTURE_CHANNEL_SPECULAR	= 0x00000010	//!< Specular map
	l_arrayChannels.push_back( _( "Height" ) ); //	eTEXTURE_CHANNEL_HEIGHT		= 0x00000020	//!< Height map
	l_arrayChannels.push_back( _( "Ambient" ) ); //	eTEXTURE_CHANNEL_AMBIENT	= 0x00000040	//!< Ambient map
	l_arrayChannels.push_back( _( "Gloss" ) ); //	eTEXTURE_CHANNEL_GLOSS		= 0x00000080	//!< Gloss map
	m_pBoxGeneral = new wxStaticBox( this, wxID_ANY, _( "General" ) );
	m_pStaticAmbient = new wxStaticText( this, wxID_ANY, _( "Ambient" ) );
	m_pStaticDiffuse = new wxStaticText( this, wxID_ANY, _( "Diffuse" ) );
	m_pStaticSpecular = new wxStaticText( this, wxID_ANY, _( "Specular" ) );
	m_pStaticEmissive = new wxStaticText( this, wxID_ANY, _( "Emissive" ) );
	m_pButtonAmbient = new wxBitmapButton( this, eID_BUTTON_AMBIENT, wxBitmap(), wxDefaultPosition, l_sizeClrBtn );
	m_pButtonDiffuse = new wxBitmapButton( this, eID_BUTTON_DIFFUSE, wxBitmap(), wxDefaultPosition, l_sizeClrBtn );
	m_pButtonSpecular = new wxBitmapButton( this, eID_BUTTON_SPECULAR, wxBitmap(), wxDefaultPosition, l_sizeClrBtn );
	m_pButtonEmissive = new wxBitmapButton( this, eID_BUTTON_EMISSIVE, wxBitmap(), wxDefaultPosition, l_sizeClrBtn );
	m_pStaticExponent = new wxStaticText( this, wxID_ANY, _( "Exponent" ) );
	m_pEditShininess = new wxTextCtrl( this, eID_EDIT_SHININESS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_SIMPLE );
	m_pEditShininess->Enable( m_bCanEdit );
	m_pCheckTwoSided = new wxCheckBox( this, eID_CHECK_DOUBLE_FACE, _( "Two sided" ) );
	m_pCheckTwoSided->Enable( m_bCanEdit );
	m_pSliderAlpha = new wxSlider( this, eID_SLIDER_ALPHA, 255, 0, 255 );
	m_pSliderAlpha->Enable( m_bCanEdit );
	m_pBoxTextures = new wxStaticBox( this, wxID_ANY, _( "Texture Units" ) );
	m_pComboTextures = new wxComboBox( this, eID_COMBO_TEXTURE_UNITS, _( "New..." ), wxDefaultPosition, wxDefaultSize, wxArrayString(), wxBORDER_SIMPLE | wxCB_READONLY );
	m_pButtonDeleteTextureUnit = new wxButton( this, eID_BUTTON_DELETE, _( "Delete" ) );
	m_pPanelTextureUnit = new wxPanel( this, wxID_ANY );
	m_pButtonShader = new wxButton( this, eID_BUTTON_HAS_SHADER, _( "Edit shaders" ) );
	m_pStaticPathLabel = new wxStaticText( m_pPanelTextureUnit, wxID_ANY, _( "Image path:" ) + wxT( " " ) );
	m_pStaticPath = new wxStaticText( m_pPanelTextureUnit, wxID_ANY, wxEmptyString );
	m_pButtonTextureImage = new wxBitmapButton( m_pPanelTextureUnit, eID_BUTTON_TEXTURE_IMAGE, wxBitmap( m_sizeImage.x, m_sizeImage.y ) );
	m_pStaticTexChannel = new wxStaticText( m_pPanelTextureUnit, wxID_ANY, _( "Channel" ) );
	m_pComboTextureChannel = new wxComboBox( m_pPanelTextureUnit, eID_COMBO_TEXTURE_CHANNEL, _( "Diffuse" ), wxDefaultPosition, wxDefaultSize, l_arrayChannels, wxBORDER_SIMPLE | wxCB_READONLY );
	// The material's base colours
	wxFlexGridSizer * l_pSizerComponents = new wxFlexGridSizer( 2, 4, 5, 5 );
	l_pSizerComponents->SetFlexibleDirection( wxVERTICAL );
	l_pSizerComponents->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	l_pSizerComponents->Add(	m_pStaticAmbient,			wxSizerFlags( 1 ).Expand()	);
	l_pSizerComponents->Add(	m_pStaticDiffuse,			wxSizerFlags( 1 ).Expand()	);
	l_pSizerComponents->Add(	m_pStaticSpecular,			wxSizerFlags( 1 ).Expand()	);
	l_pSizerComponents->Add(	m_pStaticEmissive,			wxSizerFlags( 1 ).Expand()	);
	l_pSizerComponents->Add(	m_pButtonAmbient,			wxSizerFlags( 1 ).Expand().Border( wxALL, 1 )	);
	l_pSizerComponents->Add(	m_pButtonDiffuse,			wxSizerFlags( 1 ).Expand().Border( wxALL, 1 )	);
	l_pSizerComponents->Add(	m_pButtonSpecular,			wxSizerFlags( 1 ).Expand().Border( wxALL, 1 )	);
	l_pSizerComponents->Add(	m_pButtonEmissive,			wxSizerFlags( 1 ).Expand().Border( wxALL, 1 )	);
	// The material's additional components (shininess, double faced and alpha)
	wxBoxSizer * l_pSizerAdditionnal = new wxBoxSizer( wxHORIZONTAL );
	l_pSizerAdditionnal->Add(	m_pStaticExponent,			wxSizerFlags( 0 ).Border( wxRIGHT,	5 ).Align( wxALIGN_CENTER_VERTICAL )	);
	l_pSizerAdditionnal->Add(	m_pEditShininess,			wxSizerFlags( 0 ).Border( wxRIGHT,	5 ).Align( wxALIGN_CENTER_VERTICAL )	);
	l_pSizerAdditionnal->Add(	m_pCheckTwoSided,			wxSizerFlags( 0 ).Border( wxRIGHT,	5 ).Align( wxALIGN_CENTER_VERTICAL )	);
	l_pSizerAdditionnal->Add(	m_pSliderAlpha,				wxSizerFlags( 1 ).Align( wxALIGN_CENTER_VERTICAL )	);
	// The "General" group
	wxStaticBoxSizer * l_pSizerGeneral = new wxStaticBoxSizer( m_pBoxGeneral, wxVERTICAL );
	l_pSizerGeneral->Add(	l_pSizerComponents,			wxSizerFlags( 0 ).Border( wxALL,	5 )	);
	l_pSizerGeneral->Add(	l_pSizerAdditionnal,		wxSizerFlags( 0 ).Border( wxALL,	5 ).Expand()	);
	// The texture selection and deletion
	wxBoxSizer * l_pSizerTexSel = new wxBoxSizer( wxHORIZONTAL );
	l_pSizerTexSel->Add(	m_pComboTextures,			wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL )	);
	l_pSizerTexSel->Add(	m_pButtonDeleteTextureUnit,	wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL ).ReserveSpaceEvenIfHidden()	);
	// The texture file path
	wxBoxSizer * l_pSizerTexPath = new wxBoxSizer( wxHORIZONTAL );
	l_pSizerTexPath->Add(	m_pStaticPathLabel,			wxSizerFlags( 0 ).ReserveSpaceEvenIfHidden()	);
	l_pSizerTexPath->Add(	m_pStaticPath,				wxSizerFlags( 0 ).ReserveSpaceEvenIfHidden()	);
	// The texture channel
	wxBoxSizer * l_pSizerTexChan = new wxBoxSizer( wxHORIZONTAL );
	l_pSizerTexChan->Add(	m_pStaticTexChannel,		wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL ).ReserveSpaceEvenIfHidden()	);
	l_pSizerTexChan->AddSpacer( 20 );
	l_pSizerTexChan->Add(	m_pComboTextureChannel,		wxSizerFlags( 0 ).ReserveSpaceEvenIfHidden()	);
	// The texture panel
	wxBoxSizer * l_pSizerTexPanel = new wxBoxSizer( wxVERTICAL );
	l_pSizerTexPanel->Add(	l_pSizerTexPath,			wxSizerFlags( 0 ).Expand()	);
	l_pSizerTexPanel->Add(	m_pButtonTextureImage,		wxSizerFlags( 0 ).ReserveSpaceEvenIfHidden()	);
	l_pSizerTexPanel->Add(	l_pSizerTexChan,			wxSizerFlags( 0 ).Expand().ReserveSpaceEvenIfHidden()	);
	m_pPanelTextureUnit->SetSizer( l_pSizerTexPanel );
	l_pSizerTexPanel->SetSizeHints( m_pPanelTextureUnit );
	// The "Texture Units" group
	wxStaticBoxSizer * l_pSizerTexture = new wxStaticBoxSizer( m_pBoxTextures, wxVERTICAL );
	l_pSizerTexture->Add(	l_pSizerTexSel,				wxSizerFlags( 0 ).Border( wxALL,	5 ).ReserveSpaceEvenIfHidden()	);
	l_pSizerTexture->Add(	m_pPanelTextureUnit,		wxSizerFlags( 0 ).Border( wxALL,	5 ).Expand().ReserveSpaceEvenIfHidden()	);
	// The whole panel
	wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
	l_pSizer->Add(	l_pSizerGeneral,			wxSizerFlags( 0 ).Expand()	);
	l_pSizer->Add(	l_pSizerTexture,			wxSizerFlags( 0 ).Expand().ReserveSpaceEvenIfHidden()	);
	l_pSizer->Add(	m_pButtonShader,			wxSizerFlags( 0 ).Border( wxTOP,	5 )	);
	SetSizer( l_pSizer );
	l_pSizer->SetSizeHints( this );
}

wxPassPanel::~wxPassPanel()
{
	m_pTextureUnit.reset();
}

void wxPassPanel::SetPass( PassWPtr p_wpPass )
{
	m_wpPass = p_wpPass;
	PassSPtr l_pPass = m_wpPass.lock();

	if ( l_pPass )
	{
		wxString	l_strTmp;
		wxSize		l_sizeClrBtn(	50, 50 );
		wxBitmap	l_bmpAmbient(	l_sizeClrBtn.x, l_sizeClrBtn.y, 24 );
		wxBitmap	l_bmpDiffuse(	l_sizeClrBtn.x, l_sizeClrBtn.y, 24 );
		wxBitmap	l_bmpSpecular(	l_sizeClrBtn.x, l_sizeClrBtn.y, 24 );
		wxBitmap	l_bmpEmissive(	l_sizeClrBtn.x, l_sizeClrBtn.y, 24 );
		wxPen		l_penAmbient(	wxColour( l_pPass->GetAmbient().to_bgr()	) );
		wxPen		l_penDiffuse(	wxColour( l_pPass->GetDiffuse().to_bgr()	) );
		wxPen		l_penSpecular(	wxColour( l_pPass->GetSpecular().to_bgr()	) );
		wxPen		l_penEmissive(	wxColour( l_pPass->GetEmissive().to_bgr()	) );
		wxBrush		l_brushAmbient(	wxColour( l_pPass->GetAmbient().to_bgr()	) );
		wxBrush		l_brushDiffuse(	wxColour( l_pPass->GetDiffuse().to_bgr()	) );
		wxBrush		l_brushSpecular(	wxColour( l_pPass->GetSpecular().to_bgr()	) );
		wxBrush		l_brushEmissive(	wxColour( l_pPass->GetEmissive().to_bgr()	) );
		wxMemoryDC l_dc;
		l_dc.SelectObject(	l_bmpAmbient	);
		l_dc.SetBrush(	l_brushAmbient	);
		l_dc.SetPen(	l_penAmbient	);
		l_dc.DrawRectangle(	0, 0, l_sizeClrBtn.x, l_sizeClrBtn.y );
		l_dc.SelectObject(	l_bmpDiffuse	);
		l_dc.SetBrush(	l_brushDiffuse	);
		l_dc.SetPen(	l_penDiffuse	);
		l_dc.DrawRectangle(	0, 0, l_sizeClrBtn.x, l_sizeClrBtn.y );
		l_dc.SelectObject(	l_bmpSpecular	);
		l_dc.SetBrush(	l_brushSpecular	);
		l_dc.SetPen(	l_penSpecular	);
		l_dc.DrawRectangle(	0, 0, l_sizeClrBtn.x, l_sizeClrBtn.y );
		l_dc.SelectObject(	l_bmpEmissive	);
		l_dc.SetBrush(	l_brushEmissive	);
		l_dc.SetPen(	l_penEmissive	);
		l_dc.DrawRectangle(	0, 0, l_sizeClrBtn.x, l_sizeClrBtn.y );
		m_pTextureUnit.reset();
		m_uiSelectedUnitIndex = 0xFFFFFFFF;
		l_strTmp << l_pPass->GetShininess();
		m_pButtonAmbient->SetBitmapLabel(	l_bmpAmbient	);
		m_pButtonDiffuse->SetBitmapLabel(	l_bmpDiffuse	);
		m_pButtonSpecular->SetBitmapLabel(	l_bmpSpecular	);
		m_pButtonEmissive->SetBitmapLabel(	l_bmpEmissive	);
		m_pEditShininess->SetValue(	l_strTmp	);
		m_pCheckTwoSided->SetValue(	l_pPass->IsTwoSided() );
		m_pComboTextures->Clear();

		if ( l_pPass->GetTextureUnitsCount() > 0 )
		{
			for ( uint32_t i = 0; i < l_pPass->GetTextureUnitsCount(); i++ )
			{
				wxString l_strName;
				l_strName << i;
				m_pComboTextures->Append( l_strName );
			}

			DoSelectTextureUnit( 0 );
		}
		else
		{
			DoShowTextureFields( m_bCanEdit );
		}

		if ( m_bCanEdit )
		{
			m_pComboTextures->Append( wxCOMBO_NEW );
		}

		DoShowPassFields( true );
	}
}

void wxPassPanel::GetDiffuse( Colour & clrColour )const
{
	clrColour = Colour::from_components( m_clrDiffuse.Red(), m_clrDiffuse.Green(), m_clrDiffuse.Blue(), m_clrDiffuse.Alpha() );
}

void wxPassPanel::GetAmbient( Colour & clrColour )const
{
	clrColour = Colour::from_components( m_clrAmbient.Red(), m_clrAmbient.Green(), m_clrAmbient.Blue(), m_clrAmbient.Alpha() );
}

void wxPassPanel::GetEmissive( Colour & clrColour )const
{
	clrColour = Colour::from_components( m_clrEmissive.Red(), m_clrEmissive.Green(), m_clrEmissive.Blue(), m_clrEmissive.Alpha() );
}

void wxPassPanel::GetSpecular( Colour & clrColour )const
{
	clrColour = Colour::from_components( m_clrSpecular.Red(), m_clrSpecular.Green(), m_clrSpecular.Blue(), m_clrSpecular.Alpha() );
}

real wxPassPanel::GetShininess()const
{
	real l_rReturn = real( 0 );
	String l_strValue = ( wxChar const * )m_pEditShininess->GetValue();

	if ( str_utils::is_floating( l_strValue ) )
	{
		l_rReturn = str_utils::to_real( l_strValue );

		if ( l_rReturn < 0 )
		{
			l_rReturn = 0;
		}

		if ( l_rReturn > 128 )
		{
			l_rReturn = 128;
		}
	}

	return l_rReturn;
}

int wxPassPanel::GetTextureUnitIndex()const
{
	String l_strValue = ( wxChar const * )m_pComboTextures->GetValue().c_str();
	int l_iReturn = -2;

	if ( str_utils::is_integer( l_strValue ) )
	{
		l_iReturn = str_utils::to_int( l_strValue );
	}
	else if ( l_strValue == wxCOMBO_NEW )
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
				m_pButtonTextureImage->SetBitmapLabel( wxBitmap( m_imgSelected ) );
				m_pButtonTextureImage->SetBitmapDisabled( wxBitmap( m_imgSelected ) );
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
				m_uiSelectedUnitIndex = uint32_t( l_pPass->GetTextureUnitsCount() );
				m_pComboTextures->Insert( l_strValue, m_uiSelectedUnitIndex );
				m_pComboTextures->Update();
				m_pTextureUnit = l_pPass->AddTextureUnit();
			}
			else
			{
				m_uiSelectedUnitIndex = static_cast< uint32_t >( p_iIndex );
				m_pTextureUnit = l_pPass->GetTextureUnit( m_uiSelectedUnitIndex );
			}

			l_strValue << m_uiSelectedUnitIndex;
			m_pComboTextures->SetValue( l_strValue );

			if ( !m_pTextureUnit.expired() )
			{
				m_pStaticPath->SetLabel( m_pTextureUnit.lock()->GetTexturePath() );

				if ( p_iIndex == -1 || m_pTextureUnit.lock()->GetTexturePath().empty() )
				{
					m_imgSelected.Create( m_sizeImage.x, m_sizeImage.y, true );
				}
				else
				{
					SetMaterialImage( p_iIndex, m_sizeImage.x, m_sizeImage.y );
				}

				switch ( m_pTextureUnit.lock()->GetChannel() )
				{
				case eTEXTURE_CHANNEL_COLOUR	:
					m_pComboTextureChannel->SetSelection( 0 );
					break;

				case eTEXTURE_CHANNEL_DIFFUSE	:
					m_pComboTextureChannel->SetSelection( 1 );
					break;

				case eTEXTURE_CHANNEL_NORMAL	:
					m_pComboTextureChannel->SetSelection( 2 );
					break;

				case eTEXTURE_CHANNEL_OPACITY	:
					m_pComboTextureChannel->SetSelection( 3 );
					break;

				case eTEXTURE_CHANNEL_SPECULAR	:
					m_pComboTextureChannel->SetSelection( 4 );
					break;

				case eTEXTURE_CHANNEL_HEIGHT	:
					m_pComboTextureChannel->SetSelection( 5 );
					break;

				case eTEXTURE_CHANNEL_AMBIENT	:
					m_pComboTextureChannel->SetSelection( 6 );
					break;

				case eTEXTURE_CHANNEL_GLOSS		:
					m_pComboTextureChannel->SetSelection( 7 );
					break;
				}
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
	m_pBoxGeneral->Show(	p_bShow );
	m_pStaticAmbient->Show(	p_bShow );
	m_pStaticDiffuse->Show(	p_bShow );
	m_pStaticSpecular->Show(	p_bShow );
	m_pStaticEmissive->Show(	p_bShow );
	m_pStaticExponent->Show(	p_bShow );
	m_pButtonShader->Show(	p_bShow );
	m_pButtonAmbient->Show(	p_bShow );
	m_pButtonDiffuse->Show(	p_bShow );
	m_pButtonSpecular->Show(	p_bShow );
	m_pButtonEmissive->Show(	p_bShow );
	m_pCheckTwoSided->Show(	p_bShow );
	m_pEditShininess->Show(	p_bShow );
	m_pSliderAlpha->Show(	p_bShow );
	m_pCheckTwoSided->Enable(	m_bCanEdit );
	m_pEditShininess->Enable(	m_bCanEdit );
	m_pSliderAlpha->Enable(	m_bCanEdit );
}

void wxPassPanel::DoShowTextureFields( bool p_bShow )
{
	m_pBoxTextures->Show(	p_bShow );
	m_pComboTextures->Show(	p_bShow );
	m_pStaticPathLabel->Show(	p_bShow );
	m_pPanelTextureUnit->Show(	p_bShow );
	m_pStaticPath->Show(	p_bShow );
	m_pStaticTexChannel->Show(	p_bShow );
	m_pComboTextureChannel->Show(	p_bShow );
	m_pButtonTextureImage->Show(	p_bShow );
	m_pButtonDeleteTextureUnit->Show(	p_bShow );
	m_pComboTextureChannel->Enable(	m_bCanEdit );
	m_pButtonTextureImage->Enable(	m_bCanEdit );
	m_pButtonDeleteTextureUnit->Enable(	m_bCanEdit );
}

BEGIN_EVENT_TABLE( wxPassPanel, wxPanel )
	EVT_BUTTON(	eID_BUTTON_DELETE,			wxPassPanel::OnDeleteTextureUnit	)
	EVT_BUTTON(	eID_BUTTON_AMBIENT,			wxPassPanel::OnAmbientColour	)
	EVT_BUTTON(	eID_BUTTON_DIFFUSE,			wxPassPanel::OnDiffuseColour	)
	EVT_BUTTON(	eID_BUTTON_SPECULAR,		wxPassPanel::OnSpecularColour	)
	EVT_BUTTON(	eID_BUTTON_EMISSIVE,		wxPassPanel::OnEmissiveColour	)
	EVT_BUTTON(	eID_BUTTON_TEXTURE_IMAGE,	wxPassPanel::OnTextureImage	)
	EVT_BUTTON(	eID_BUTTON_HAS_SHADER,		wxPassPanel::OnHasShader	)
	EVT_TEXT_ENTER( eID_EDIT_SHININESS,			wxPassPanel::OnShininess	)
	EVT_TEXT_ENTER( eID_EDIT_TEXTURE_PATH,		wxPassPanel::OnTexturePath	)
	EVT_COMBOBOX(	eID_COMBO_TEXTURE_UNITS,	wxPassPanel::OnTextureSelect	)
	EVT_CHECKBOX(	eID_CHECK_DOUBLE_FACE,		wxPassPanel::OnDoubleFace	)
	EVT_COMBOBOX(	eID_COMBO_TEXTURE_CHANNEL,	wxPassPanel::OnTextureChannel	)
	EVT_SLIDER(	eID_SLIDER_ALPHA,			wxPassPanel::OnAlpha	)
END_EVENT_TABLE()

void wxPassPanel::OnDeleteTextureUnit( wxCommandEvent & p_event )
{
	PassSPtr l_pPass = m_wpPass.lock();

	if ( l_pPass )
	{
		m_pComboTextures->Clear();
		l_pPass->DestroyTextureUnit( m_uiSelectedUnitIndex );
		l_pPass->Initialise();

		for ( uint32_t i = 0; i < l_pPass->GetTextureUnitsCount(); i++ )
		{
			wxString l_strName;
			l_strName << i;
			m_pComboTextures->Append( l_strName );
		}

		m_pComboTextures->Append( wxCOMBO_NEW );
		m_pComboTextures->SetValue( wxCOMBO_NEW );
		DoSelectTextureUnit( -2 );
		m_pComboTextures->Update();

		if ( l_pPass->GetTextureUnitsCount() == 0 )
		{
			m_pButtonDeleteTextureUnit->Disable();
		}
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
			m_clrAmbient = l_retData.GetColour().GetAsString();
			m_pButtonAmbient->SetBackgroundColour( m_clrAmbient );
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
			m_clrDiffuse = l_retData.GetColour().GetAsString();
			m_pButtonDiffuse->SetBackgroundColour( m_clrDiffuse );
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
			m_clrSpecular = l_retData.GetColour().GetAsString();
			m_pButtonSpecular->SetBackgroundColour( m_clrSpecular );
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
			m_clrEmissive = l_retData.GetColour().GetAsString();
			m_pButtonEmissive->SetBackgroundColour( m_clrEmissive );
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
			m_pStaticPath->SetLabel( l_strPathImage );

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
				m_pEngine->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_pPass->GetParent() ) );
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
		bool l_bDouble = m_pCheckTwoSided->IsChecked();
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

void GuiCommon::wxCreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap )
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

void GuiCommon::wxCreateBitmapFromBuffer( TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap )
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

//*************************************************************************************************
