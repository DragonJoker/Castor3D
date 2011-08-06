#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/PassPanel.hpp"
#include "GuiCommon/ShaderDialog.hpp"

using namespace Castor3D;
using namespace GuiCommon;


wxPassPanel :: wxPassPanel( wxWindow * parent, const wxPoint & pos, const wxSize & size)
	:	wxScrolledWindow( parent, wxID_ANY, pos, size, 524288 | wxBORDER_NONE)
	,	m_textureUnitPanel( NULL)
{
}

wxPassPanel :: ~wxPassPanel()
{
	m_selectedTextureUnit.reset();
}

void wxPassPanel :: CreatePassPanel( PassPtr p_pass)
{
	m_pass = p_pass;
	m_selectedTextureUnit.reset();
	m_selectedUnitIndex = -1;
	_createPassPanel();
}

void wxPassPanel :: GetDiffuse( real & red, real & green, real & blue)const
{
	red = ((real)m_diffuseColour.Red()) / 255.0;
	green = ((real)m_diffuseColour.Green()) / 255.0;
	blue = ((real)m_diffuseColour.Blue()) / 255.0;
}

void wxPassPanel :: GetAmbient( real & red, real & green, real & blue)const
{
	red = ((real)m_ambientColour.Red()) / 255.0;
	green = ((real)m_ambientColour.Green()) / 255.0;
	blue = ((real)m_ambientColour.Blue()) / 255.0;
}

void wxPassPanel :: GetEmissive( real & red, real & green, real & blue)const
{
	red = ((real)m_emissiveColour.Red()) / 255.0;
	green = ((real)m_emissiveColour.Green()) / 255.0;
	blue = ((real)m_emissiveColour.Blue()) / 255.0;
}

void wxPassPanel :: GetSpecular( real & red, real & green, real & blue)const
{
	red = ((real)m_specularColour.Red()) / 255.0;
	green = ((real)m_specularColour.Green()) / 255.0;
	blue = ((real)m_specularColour.Blue()) / 255.0;
}

void wxPassPanel :: GetBlendColour( real & red, real & green, real & blue)const
{
	red = ((real)m_blendColour.Red()) / 255.0;
	green = ((real)m_blendColour.Green()) / 255.0;
	blue = ((real)m_blendColour.Blue()) / 255.0;
}

real wxPassPanel :: GetShininess()const
{
	String l_value = (const wxChar *)m_shininessText->GetValue();

	if (l_value.is_floating())
	{
		real l_res = l_value.to_real();
		if (l_res < 0.0)
		{
			l_res = 0.0;
		}
		if (l_res > 128.0)
		{
			l_res = 128.0;
		}
		return l_res;
	}

	return 0.0f;
}

int wxPassPanel :: GetTextureUnitIndex()const
{
	wxString l_value = m_texturesCubeBox->GetValue();
	Logger::LogMessage( cuT( "wxPassPanel :: GetTextureUnitIndex - l_value : %s"), l_value.char_str());
	int l_iReturn = -2;

	if (l_value.IsNumber())
	{
		long l_res;
		l_value.ToLong( & l_res);
		Logger::LogMessage( cuT( "wxPassPanel :: GetTextureUnitIndex - l_res : %i"), l_res);
		l_iReturn = l_res;
	}
	else if (l_value == wxT( "New..."))
	{
		l_iReturn = -1;
	}

	return l_iReturn;
}

void wxPassPanel :: SetMaterialImage( unsigned int p_index, unsigned int p_width, unsigned int p_height)
{
	if (m_pass && p_index < m_pass->GetNbTexUnits())
	{
		SetMaterialImage( m_pass->GetTextureUnit( p_index), p_width, p_height);
	}
}

void wxPassPanel :: SetMaterialImage( TextureUnitPtr p_pTexture, unsigned int p_width, unsigned int p_height)
{
	if (p_pTexture)
	{
		wxBitmap l_bmp;
		_createBitmapFromBuffer( l_bmp, p_pTexture->GetImagePixels(), p_pTexture->GetWidth(), p_pTexture->GetHeight());
		m_selectedUnitImage = l_bmp.ConvertToImage();

		m_selectedUnitImage.Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGHEST);
		m_texImageButton->SetBitmapLabel( wxBitmap( m_selectedUnitImage));
	}
}

void wxPassPanel :: _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height)
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

void wxPassPanel :: _createPassPanel()
{
	m_selectedTextureUnit.reset();

	if ((m_textureUnitPanel && ! m_textureUnitPanel->DestroyChildren()) || ! DestroyChildren() || ! m_pass)
	{
		return;
	}

	wxSize l_size = GetClientSize();
	l_size.x -= 10;
	l_size.y = 120;
	new wxStaticBox( this, wxID_ANY, wxT( "General"), wxPoint( 5, 5), l_size);

	new wxStaticText( this, wxID_ANY, wxT( "Ambient"), wxPoint( 15, 25));
	m_ambientButton = new wxButton( this, eID_BUTTON_AMBIENT, wxEmptyString, wxPoint( 15, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_ambientButton->SetBackgroundColour( wxColour( m_pass->GetAmbient().RGB()));

	new wxStaticText( this, wxID_ANY, wxT( "Diffuse"), wxPoint( 75, 25));
	m_diffuseButton = new wxButton( this, eID_BUTTON_DIFFUSE, wxEmptyString, wxPoint( 75, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_diffuseButton->SetBackgroundColour( wxColour( m_pass->GetDiffuse().RGB()));

	new wxStaticText( this, wxID_ANY, wxT( "Specular"), wxPoint( 135, 25));
	m_specularButton = new wxButton( this, eID_BUTTON_SPECULAR, wxEmptyString, wxPoint( 135, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_specularButton->SetBackgroundColour( wxColour( m_pass->GetSpecular().RGB()));

	new wxStaticText( this, wxID_ANY, wxT( "Emissive"), wxPoint( 195, 25));
	m_emissiveButton = new wxButton( this, eID_BUTTON_EMISSIVE, wxEmptyString, wxPoint( 195, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_emissiveButton->SetBackgroundColour( wxColour( m_pass->GetEmissive().RGB()));

	new wxStaticText( this, wxID_ANY, wxT( "Blend"), wxPoint( 255, 25));
	m_blendButton = new wxButton( this, eID_BUTTON_BLEND_COLOUR, wxEmptyString, wxPoint( 255, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_blendButton->SetBackgroundColour( wxColour( m_pass->GetTexBaseColour().RGB()));

	new wxStaticText( this, wxID_ANY, wxT( "Exponent"), wxPoint( 20, 105));
	wxString l_strTmp;
	l_strTmp << m_pass->GetShininess();
	m_shininessText = new wxTextCtrl( this, eID_EDIT_SHININESS, l_strTmp, wxPoint( 80, 100), wxSize( 40, 20), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);

	m_doubleFace = new wxCheckBox( this, eID_CHECK_DOUBLE_FACE, wxT( "Double face"), wxPoint( 130, 100), wxSize( 100, 20), wxBORDER_SIMPLE);
	m_doubleFace->SetValue( m_pass->IsDoubleFace());

	m_sliderAlpha = new wxSlider( this, eID_SLIDER_ALPHA, 255, 0, 255, wxPoint( 230, 100), wxSize( l_size.x - 230, 20));

	l_size.y = 205;
	wxStaticBox * l_texturesBox = new wxStaticBox( this, wxID_ANY, wxT( "Texture Units"), wxPoint( 5, 125), l_size);
	wxString * l_names = new wxString[m_pass->GetNbTexUnits() + 1];

	for (unsigned int i = 0 ; i < m_pass->GetNbTexUnits() ; i++)
	{
		l_names[i].clear();
		l_names[i] << i;
	}

	l_names[m_pass->GetNbTexUnits()] = cuT( "New...");
	m_texturesCubeBox = new wxComboBox( this, eID_COMBO_TEXTURE_UNITS, wxT( "New..."), wxPoint( 20, 145), wxSize( 170, 20), m_pass->GetNbTexUnits() + 1, l_names, wxBORDER_SIMPLE | wxCB_READONLY);
	m_deleteTextureUnit = new wxButton( this, eID_BUTTON_DELETE, wxT( "Delete"), wxPoint( 200, 144), wxSize( 80, 23), wxBORDER_SIMPLE);
	m_deleteTextureUnit->Disable();

	if (m_pass->GetNbTexUnits() > 0)
	{
		m_deleteTextureUnit->Enable();
	}

	m_textureUnitPanel = new wxPanel( this, wxID_ANY, wxPoint( 20, 175), wxSize( l_texturesBox->GetClientSize().x - 20, l_texturesBox->GetClientSize().y - 40));
	_createTextureUnitPanel( 0);

	m_buttonShader = new wxButton( this, eID_BUTTON_HAS_SHADER, wxT( "Edit shaders"), wxPoint( 20, 345), wxSize( 150, 20));
}

void wxPassPanel :: _createTextureUnitPanel( int p_index)
{
	if ( ! m_textureUnitPanel->DestroyChildren()
		|| p_index >= static_cast <int>( m_pass->GetNbTexUnits())
		|| p_index < -1)
	{
		return;
	}

	m_deleteTextureUnit->Enable();

	if (p_index == -1)
	{
		wxString l_value;
		l_value << m_pass->GetNbTexUnits();
		m_texturesCubeBox->Insert( l_value, m_pass->GetNbTexUnits());
		m_texturesCubeBox->Update();
		m_selectedUnitIndex = m_pass->GetNbTexUnits();
		m_selectedTextureUnit = m_pass->AddTextureUnit();
		m_texturesCubeBox->SetValue( l_value);
	}
	else
	{
		m_selectedTextureUnit = m_pass->GetTextureUnit( p_index);
		m_selectedUnitIndex = static_cast <unsigned int>( p_index);
		wxString l_value;
		l_value << p_index;
		m_texturesCubeBox->SetValue( l_value);
	}

	if ( ! m_selectedTextureUnit)
	{
		return;
	}

	wxSize l_size = m_textureUnitPanel->GetClientSize();
	new wxStaticText( m_textureUnitPanel, wxID_ANY, wxT( "Image path : "), wxPoint( 0, -3));
	m_texPathText = new wxStaticText( m_textureUnitPanel, wxID_ANY, m_selectedTextureUnit->GetTexturePath(), wxPoint( 120, -3), wxSize( l_size.x - 120, 16));
	m_texImageButton = new wxBitmapButton( m_textureUnitPanel, eID_BUTTON_TEXTURE_IMAGE, wxBitmap( 78, 78), wxPoint( 0, 15), wxSize( 80, 80), wxBORDER_SIMPLE);
	if (p_index == -1 || m_selectedTextureUnit->GetTexturePath().empty())
	{
		m_selectedUnitImage.Create( 78, 78, true);
	}
	else
	{
		SetMaterialImage( p_index, 78, 78);
	}

	wxArrayString l_modes;
	l_modes.push_back( wxT( "Normal"));
	l_modes.push_back( wxT( "Reflexion map"));
	l_modes.push_back( wxT( "Sphere map"));
	m_texMode = new wxRadioBox( m_textureUnitPanel, eID_RADIO_MAP_MODE, wxT( "Mode"), wxPoint( 0, 93), wxSize( m_textureUnitPanel->GetClientSize().x, 45), l_modes, 3, wxRA_SPECIFY_COLS);
	m_texMode->SetSelection( (int)m_selectedTextureUnit->GetTextureMapMode());
}

BEGIN_EVENT_TABLE( wxPassPanel, wxPanel)
	EVT_BUTTON(		eID_BUTTON_DELETE,			wxPassPanel::_onDeleteTextureUnit)
	EVT_BUTTON(		eID_BUTTON_AMBIENT,			wxPassPanel::_onAmbientColour)
	EVT_BUTTON(		eID_BUTTON_DIFFUSE,			wxPassPanel::_onDiffuseColour)
	EVT_BUTTON(		eID_BUTTON_SPECULAR,		wxPassPanel::_onSpecularColour)
	EVT_BUTTON(		eID_BUTTON_EMISSIVE,		wxPassPanel::_onEmissiveColour)
	EVT_BUTTON(		eID_BUTTON_BLEND_COLOUR,	wxPassPanel::_onBlendColour)
	EVT_BUTTON(		eID_BUTTON_TEXTURE_IMAGE,	wxPassPanel::_onTextureImage)
	EVT_BUTTON(		eID_BUTTON_HAS_SHADER,		wxPassPanel::_onHasShader)
	EVT_TEXT_ENTER( eID_EDIT_SHININESS,			wxPassPanel::_onShininess)
	EVT_TEXT_ENTER( eID_EDIT_TEXTURE_PATH,		wxPassPanel::_onTexturePath)
	EVT_COMBOBOX(	eID_COMBO_TEXTURE_UNITS,	wxPassPanel::_onTextureSelect)
	EVT_CHECKBOX(	eID_CHECK_DOUBLE_FACE,		wxPassPanel::_onDoubleFace)
	EVT_RADIOBOX(	eID_RADIO_MAP_MODE,			wxPassPanel::_onTextureMode)
	EVT_SLIDER(		eID_SLIDER_ALPHA,			wxPassPanel::_onAlpha)
END_EVENT_TABLE()

void wxPassPanel :: _onDeleteTextureUnit( wxCommandEvent & event)
{
	if (m_pass)
	{
		m_texturesCubeBox->Clear();
		m_pass->DestroyTextureUnit( m_selectedUnitIndex);
		m_pass->Initialise();
		unsigned int i;
		wxString l_name;

		for (i = 0 ; i < m_pass->GetNbTexUnits() ; i++)
		{
			l_name.clear();
			l_name << i;
			m_texturesCubeBox->Insert( l_name, i);
		}

		l_name = wxT( "New...");
		m_texturesCubeBox->Insert( l_name, i);
		m_texturesCubeBox->SetValue( wxT( "New..."));
		_createTextureUnitPanel( -2);
		m_texturesCubeBox->Update();

		if (m_pass->GetNbTexUnits() == 0)
		{
			m_deleteTextureUnit->Disable();
		}
	}
}

void wxPassPanel :: _onAmbientColour( wxCommandEvent & event)
{
	wxColourDialog l_colourDialor( this);

	if (l_colourDialor.ShowModal())
	{
		wxColourData retData = l_colourDialor.GetColourData();
		m_ambientColour = retData.GetColour().GetAsString();
		m_ambientButton->SetBackgroundColour( m_ambientColour);
		real l_ambient[3];
		GetAmbient(l_ambient[0], l_ambient[1], l_ambient[2]);
		m_pass->SetAmbient( l_ambient[0], l_ambient[1], l_ambient[2], 1.0f);
	}
}

void wxPassPanel :: _onDiffuseColour( wxCommandEvent & event)
{
	wxColourDialog l_colourDialor( this);

	if (l_colourDialor.ShowModal())
	{
		wxColourData retData = l_colourDialor.GetColourData();
		m_diffuseColour = retData.GetColour().GetAsString();
		m_diffuseButton->SetBackgroundColour( m_diffuseColour);
		real l_diffuse[3];
		GetDiffuse(l_diffuse[0], l_diffuse[1], l_diffuse[2]);
		m_pass->SetDiffuse( l_diffuse[0], l_diffuse[1], l_diffuse[2], 1.0f);
	}
}

void wxPassPanel :: _onSpecularColour( wxCommandEvent & event)
{
	wxColourDialog l_colourDialor( this);

	if (l_colourDialor.ShowModal())
	{
		wxColourData retData = l_colourDialor.GetColourData();
		m_specularColour = retData.GetColour().GetAsString();
		m_specularButton->SetBackgroundColour( m_specularColour);
		real l_specular[3];
		GetSpecular(l_specular[0], l_specular[1], l_specular[2]);
		m_pass->SetSpecular( l_specular[0], l_specular[1], l_specular[2], 1.0f);
	}
}

void wxPassPanel :: _onEmissiveColour( wxCommandEvent & event)
{
	wxColourDialog l_colourDialor( this);

	if (l_colourDialor.ShowModal())
	{
		wxColourData retData = l_colourDialor.GetColourData();
		m_emissiveColour = retData.GetColour().GetAsString();
		m_emissiveButton->SetBackgroundColour( m_emissiveColour);
		real l_emissive[3];
		GetEmissive(l_emissive[0], l_emissive[1], l_emissive[2]);
		m_pass->SetEmissive( l_emissive[0], l_emissive[1], l_emissive[2], 1.0f);
	}
}

void wxPassPanel :: _onBlendColour( wxCommandEvent & event)
{
	wxColourDialog l_colourDialor( this);

	if (l_colourDialor.ShowModal())
	{
		wxColourData retData = l_colourDialor.GetColourData();
		m_blendColour = retData.GetColour().GetAsString();
		m_blendButton->SetBackgroundColour( m_blendColour);
		real l_blendColour[3];
		GetBlendColour( l_blendColour[0], l_blendColour[1], l_blendColour[2]);
		m_pass->SetTexBaseColour( l_blendColour[0], l_blendColour[1], l_blendColour[2], 1.0);
	}
}

void wxPassPanel :: _onTextureImage( wxCommandEvent & event)
{
	wxString l_strWildcard = wxT( "BITMAP images (*.bmp)|*.bmp|GIF images (*.gif)|*.gif|JPEG images (*.jpg)|*.jpg|PNG images (*.png)|*.png|TARGA images (*.tga)|*.tga|All images (*.bmp;*.gif;*.png;*.jpg;*.tga)|*.bmp;*.gif;*.png;*.jpg;*.tga");
	wxFileDialog l_fdlg( this, wxT( "Choose an image"), wxEmptyString, wxEmptyString, l_strWildcard);

	if (l_fdlg.ShowModal() == wxID_OK)
	{
		wxString l_imagePath = l_fdlg.GetPath();
		m_texPathText->SetLabel( l_imagePath);

		if (m_selectedTextureUnit)
		{
			Path l_pathImage = (const wxChar *)l_imagePath.c_str();
			Collection<Image, String> l_imgCollection;
			ImagePtr l_pImage = l_imgCollection.GetElement( l_pathImage);
			if ( ! l_pImage)
			{
				l_pImage = ImagePtr( new Image( l_pathImage));
				l_imgCollection.AddElement( l_pathImage, l_pImage);
			}
			Loader<Image>::Load( * l_pImage, l_pathImage);
			m_selectedTextureUnit->SetTexture( eTEXTURE_TYPE_2D, l_pImage);
			SetMaterialImage( m_selectedTextureUnit, 78, 78);
			Root::GetSingleton()->GetMaterialManager()->SetToInitialise( m_pass->GetParent());
		}
	}
}

void wxPassPanel :: _onShininess( wxCommandEvent & event)
{
	real l_shininess = GetShininess();
	m_pass->SetShininess( l_shininess);
	m_pass->Initialise();
}

void wxPassPanel :: _onTexturePath( wxCommandEvent & event)
{
/*
	String l_path = m_texPathText->GetValue().c_str();
	if (l_path.empty())
	{
		return;
	}
	if (m_selectedUnitImage)
	{
		delete m_selectedUnitImage;
		m_selectedUnitImage = nullptr;
	}
	SetMaterialImage( l_path.c_str(), 78, 78);
	if (m_selectedTextureUnit)
	{
		Collection<Image, String> l_imgCollection;
		ImagePtr l_pImage = l_imgCollection.GetElement( l_path);
		if ( ! l_pImage)
		{
			l_pImage = ImagePtr( new Image( l_path));
			l_imgCollection.AddElement( l_path, l_pImage);
		}
		Loader<Image>::Load( * l_pImage, l_path);
		m_selectedTextureUnit->SetTexture( TextureUnit::eTex2D, l_pImage);
		Root::GetSingleton()->SetToInitialise( m_pass->GetParent());
	}
*/
}

void wxPassPanel :: _onTextureSelect( wxCommandEvent & event)
{
	int l_selectedUnit = GetTextureUnitIndex();
	_createTextureUnitPanel( l_selectedUnit);
}

void wxPassPanel :: _onDoubleFace( wxCommandEvent & event)
{
	bool l_double = m_doubleFace->IsChecked();
	m_pass->SetDoubleFace( l_double);
}

void wxPassPanel :: _onHasShader( wxCommandEvent & event)
{
	wxShaderDialog l_dialog( this, m_pass);

	l_dialog.ShowModal();
}

void wxPassPanel :: _onTextureMode( wxCommandEvent & event)
{
	m_selectedTextureUnit->SetTextureMapMode( (TextureUnit::eMAP_MODE)m_texMode->GetSelection());
}

void wxPassPanel :: _onAlpha( wxCommandEvent & event)
{
	m_pass->SetAlpha( real( event.GetInt()) / 255.0f);
}
