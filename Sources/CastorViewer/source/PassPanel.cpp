#include "PrecompiledHeader.h"

#include "PassPanel.h"
#include "EnvironmentFrame.h"
#include "CastorViewer.h"
#include "MainFrame.h"
#include "ShaderDialog.h"

#ifdef __WXMSW__
#	include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

using namespace Castor3D;
using namespace CastorViewer;
DECLARE_APP( CastorViewerApp)


PassPanel :: PassPanel( wxWindow * parent, const wxPoint & pos, const wxSize & size)
	:	wxScrolledWindow( parent, wxID_ANY, pos, size, 524288 | wxBORDER_NONE),
		m_pass( NULL),
		m_selectedTextureUnit( NULL),
		m_selectedUnitImage( NULL),
		m_textureUnitPanel( NULL)
{
}

PassPanel :: ~PassPanel()
{
	if (m_selectedUnitImage)
	{
		delete m_selectedUnitImage;
		m_selectedUnitImage = NULL;
	}
	m_selectedTextureUnit = NULL;
}

void PassPanel :: CreatePassPanel( PassPtr p_pass)
{
	m_pass = p_pass;
	m_selectedTextureUnit = NULL;
	m_selectedUnitIndex = -1;
	_createPassPanel();
}

void PassPanel :: GetDiffuse( real & red, real & green, real & blue)const
{
	red = ((real)m_diffuseColour.Red()) / 255.0;
	green = ((real)m_diffuseColour.Green()) / 255.0;
	blue = ((real)m_diffuseColour.Blue()) / 255.0;
}

void PassPanel :: GetAmbient( real & red, real & green, real & blue)const
{
	red = ((real)m_ambientColour.Red()) / 255.0;
	green = ((real)m_ambientColour.Green()) / 255.0;
	blue = ((real)m_ambientColour.Blue()) / 255.0;
}

void PassPanel :: GetEmissive( real & red, real & green, real & blue)const
{
	red = ((real)m_emissiveColour.Red()) / 255.0;
	green = ((real)m_emissiveColour.Green()) / 255.0;
	blue = ((real)m_emissiveColour.Blue()) / 255.0;
}

void PassPanel :: GetSpecular( real & red, real & green, real & blue)const
{
	red = ((real)m_specularColour.Red()) / 255.0;
	green = ((real)m_specularColour.Green()) / 255.0;
	blue = ((real)m_specularColour.Blue()) / 255.0;
}

void PassPanel :: GetBlendColour( real & red, real & green, real & blue)const
{
	red = ((real)m_blendColour.Red()) / 255.0;
	green = ((real)m_blendColour.Green()) / 255.0;
	blue = ((real)m_blendColour.Blue()) / 255.0;
}

real PassPanel :: GetShininess()const
{
	wxString l_value = m_shininessText->GetValue();
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		real l_res = ator( l_value.char_str());
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

int PassPanel :: GetTextureUnitIndex()const
{
	wxString l_value = m_texturesComboBox->GetValue();
	Log::LogMessage( CU_T( "GetTextureUnitIndex - l_value : %s"), l_value.c_str());
	
	if (l_value.IsNumber())
	{
		int l_res = atoi( l_value.char_str());
		Log::LogMessage( "GetTextureUnitIndex - l_res : %d", l_res);
		return l_res;
	}
	if (l_value == CU_T( "New..."))
	{
		return -1;
	}
	return -2;
}

wxImage * PassPanel :: SetMaterialImage( unsigned int p_index, unsigned int p_width, unsigned int p_height)
{
	if ( ! m_pass.null() && p_index < m_pass->GetNbTexUnits())
	{
		String l_path;
		l_path = m_pass->GetTextureUnit( p_index)->GetTexturePath();
		return SetMaterialImage( l_path, p_width, p_height);
	}	

	return NULL;
}

wxImage * PassPanel :: SetMaterialImage( const String & p_path, unsigned int p_width, unsigned int p_height)
{
	if (p_path.size() > 0)
	{
		m_selectedUnitImage = new wxImage();
		if (m_selectedUnitImage->LoadFile( p_path.c_str()))
		{
			m_selectedUnitImage->Rescale( p_width, p_height, wxIMAGE_QUALITY_HIGH);
			m_texImageButton->SetBitmapLabel( wxBitmap( * m_selectedUnitImage));
		}
	}

	return m_selectedUnitImage;
}

void PassPanel :: _createPassPanel()
{
	m_selectedTextureUnit.reset();

	if ((m_textureUnitPanel != NULL && ! m_textureUnitPanel->DestroyChildren())
		||  ! DestroyChildren() || m_pass.null())
	{
		return;
	}
	wxSize l_size = GetClientSize();
	l_size.x -= 10;
	l_size.y = 120;
	new wxStaticBox( this, wxID_ANY, CU_T( "Général"), wxPoint( 5, 5), l_size);

	const float * l_ambientColour = m_pass->GetAmbient();
	unsigned char l_ambientRed = (unsigned char)(l_ambientColour[0] * 255.0);
	unsigned char l_ambientGreen = (unsigned char)(l_ambientColour[1] * 255.0);
	unsigned char l_ambientBlue = (unsigned char)(l_ambientColour[2] * 255.0);
	new wxStaticText( this, wxID_ANY, CU_T( "Ambiante"), wxPoint( 15, 25));
	m_ambientButton = new wxButton( this, eAmbient, wxEmptyString, wxPoint( 15, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_ambientButton->SetBackgroundColour( wxColour( l_ambientRed, l_ambientGreen, l_ambientBlue));

	const float * l_diffuseColour = m_pass->GetDiffuse();
	unsigned char l_diffuseRed = (unsigned char)(l_diffuseColour[0] * 255.0);
	unsigned char l_diffuseGreen = (unsigned char)(l_diffuseColour[1] * 255.0);
	unsigned char l_diffuseBlue = (unsigned char)(l_diffuseColour[2] * 255.0);
	new wxStaticText( this, wxID_ANY, CU_T( "Diffuse"), wxPoint( 75, 25));
	m_diffuseButton = new wxButton( this, eDiffuse, wxEmptyString, wxPoint( 75, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_diffuseButton->SetBackgroundColour( wxColour( l_diffuseRed, l_diffuseGreen, l_diffuseBlue));

	const float * l_specularColour = m_pass->GetSpecular();
	unsigned char l_specularRed = (unsigned char)(l_specularColour[0] * 255.0);
	unsigned char l_specularGreen = (unsigned char)(l_specularColour[1] * 255.0);
	unsigned char l_specularBlue = (unsigned char)(l_specularColour[2] * 255.0);
	new wxStaticText( this, wxID_ANY, CU_T( "Speculaire"), wxPoint( 135, 25));
	m_specularButton = new wxButton( this, eSpecular, wxEmptyString, wxPoint( 135, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_specularButton->SetBackgroundColour( wxColour( l_specularRed, l_specularGreen, l_specularBlue));

	const float * l_emissiveColour = m_pass->GetEmissive();
	unsigned char l_emissiveRed = (unsigned char)(l_emissiveColour[0] * 255.0);
	unsigned char l_emissiveGreen = (unsigned char)(l_emissiveColour[1] * 255.0);
	unsigned char l_emissiveBlue = (unsigned char)(l_emissiveColour[2] * 255.0);
	new wxStaticText( this, wxID_ANY, CU_T( "Emissive"), wxPoint( 195, 25));
	m_emissiveButton = new wxButton( this, eEmissive, wxEmptyString, wxPoint( 195, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_emissiveButton->SetBackgroundColour( wxColour( l_emissiveRed, l_emissiveGreen, l_emissiveBlue));

	const float * l_colour = m_pass->GetTexBaseColour();
	unsigned char l_red = (unsigned char )(l_colour[0] * 255);
	unsigned char l_green = (unsigned char )(l_colour[1] * 255);
	unsigned char l_blue = (unsigned char )(l_colour[2] * 255);
	new wxStaticText( this, wxID_ANY, CU_T( "Blend"), wxPoint( 255, 25));
	m_blendButton = new wxButton( this, eTextureBlendColour, wxEmptyString, wxPoint( 255, 45), wxSize( 50, 50), wxBORDER_SIMPLE);
	m_blendButton->SetBackgroundColour( wxColour( l_red, l_green, l_blue));

	new wxStaticText( this, wxID_ANY, CU_T( "Exposant"), wxPoint( 20, 105));
	Char * l_shTxt = new Char[255];
	Sprintf( l_shTxt, 255, CU_T( "%d"), m_pass->GetShininess());
	m_shininessText = new wxTextCtrl( this, eShininess, l_shTxt, wxPoint( 80, 100), wxSize( 40, 20), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	delete [] l_shTxt;

	m_doubleFace = new wxCheckBox( this, eDoubleFace, CU_T( "Double face"), wxPoint( 130, 100), wxSize( 100, 20), wxBORDER_SIMPLE);
	m_doubleFace->SetValue( m_pass->IsDoubleFace());

	m_sliderAlpha = new wxSlider( this, eAlpha, 255, 0, 255, wxPoint( 230, 100), wxSize( l_size.x - 230, 20));

	l_size.y = 205;
	wxStaticBox * l_texturesBox = new wxStaticBox( this, wxID_ANY, CU_T( "Texture Units"), wxPoint( 5, 125), l_size);
	wxString * l_names = new wxString[m_pass->GetNbTexUnits() + 1];
	for (unsigned int i = 0 ; i < m_pass->GetNbTexUnits() ; i++)
	{
		l_names[i].clear();
		l_names[i] << i;
		std::cout << l_names[i].char_str() << "\n";
	}
	l_names[m_pass->GetNbTexUnits()] = CU_T( "New...");
	m_texturesComboBox = new wxComboBox( this, eTextureUnits, CU_T( "New..."), wxPoint( 20, 145), wxSize( 170, 20), m_pass->GetNbTexUnits() + 1, l_names, wxBORDER_SIMPLE | wxCB_READONLY);
	m_deleteTextureUnit = new wxButton( this, eDeleteTextureUnit, CU_T( "Supprimer"), wxPoint( 200, 144), wxSize( 80, 23), wxBORDER_SIMPLE);
	m_deleteTextureUnit->Disable();
	if (m_pass->GetNbTexUnits() > 0)
	{
		m_deleteTextureUnit->Enable();
	}
	m_textureUnitPanel = new wxPanel( this, wxID_ANY, wxPoint( 20, 175), wxSize( l_texturesBox->GetClientSize().x - 20, l_texturesBox->GetClientSize().y - 40));
	_createTextureUnitPanel( 0);

	m_buttonShader = new wxButton( this, eHasShader, CU_T( "Editer les shaders"), wxPoint( 20, 345), wxSize( 150, 20));
}

void PassPanel :: _createTextureUnitPanel( int p_index)
{
	if ( ! m_textureUnitPanel->DestroyChildren())
	{
		return;
	}
	if (p_index >= static_cast <int>( m_pass->GetNbTexUnits()))
	{
		return;
	}
	if (p_index < -1)
	{
		return;
	}
	m_deleteTextureUnit->Enable();
	if (p_index == -1)
	{
		m_selectedTextureUnit = new TextureUnit();
		wxString l_value;
		l_value << m_pass->GetNbTexUnits();
		m_texturesComboBox->Insert( l_value, m_pass->GetNbTexUnits());
		m_texturesComboBox->Update();
		m_selectedUnitIndex = m_pass->GetNbTexUnits();
		m_pass->AddTextureUnit( m_selectedTextureUnit);
		m_texturesComboBox->SetValue( l_value);
	}
	else
	{
		m_selectedTextureUnit = m_pass->GetTextureUnit( p_index);
		m_selectedUnitIndex = static_cast <unsigned int>( p_index);
		wxString l_value;
		l_value << p_index;
		m_texturesComboBox->SetValue( l_value);
	}
	if ( ! m_selectedTextureUnit)
	{
		return;
	}
	if (m_selectedUnitImage)
	{
		delete m_selectedUnitImage;
		m_selectedUnitImage = NULL;
	}
	wxSize l_size = m_textureUnitPanel->GetClientSize();
	new wxStaticText( m_textureUnitPanel, wxID_ANY, CU_T( "Chemin de l'image : "), wxPoint( 0, -3));
	m_texPathText = new wxStaticText( m_textureUnitPanel, wxID_ANY, m_selectedTextureUnit->GetTexturePath(), wxPoint( 120, -3), wxSize( l_size.x - 120, 16));
	m_texImageButton = new wxBitmapButton( m_textureUnitPanel, eTextureImage, wxBitmap( 78, 78), wxPoint( 0, 15), wxSize( 80, 80), wxBORDER_SIMPLE);
	if (p_index == -1 || m_selectedTextureUnit->GetTexturePath().empty())
	{
		m_selectedUnitImage = new wxImage( 78, 78);
	}
	else
	{
		m_selectedUnitImage = SetMaterialImage( p_index, 78, 78);
	}

	wxString l_envModes[5];
	l_envModes[0] = CU_T( "Modulate");
	l_envModes[1] = CU_T( "Replace");
	l_envModes[2] = CU_T( "Blend");
	l_envModes[3] = CU_T( "Add");
	l_envModes[4] = CU_T( "Combine");
	wxString l_value = CU_T( "Modulate");
	if (m_selectedTextureUnit->GetEnvironment()->GetMode() == EMReplace)
	{
		l_value = CU_T( "Replace");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetMode() == EMBlend)
	{
		l_value = CU_T( "Blend");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetMode() == EMAdd)
	{
		l_value = CU_T( "Add");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetMode() == EMCombine)
	{
		l_value = CU_T( "Combine");
	}
	new wxStaticText( m_textureUnitPanel, wxID_ANY, CU_T( "Environment mode"), wxPoint( 90, 16), wxSize( 90, 20));
	m_textureEnvMode = new wxComboBox( m_textureUnitPanel, eTextureEnvMode, l_value, wxPoint( 190, 13), wxSize( 80, 20), 5, l_envModes, wxBORDER_SIMPLE | wxCB_READONLY);

	wxString l_rgbCombination[8];
	l_rgbCombination[0] = CU_T( "None");
	l_rgbCombination[1] = CU_T( "Modulate");
	l_rgbCombination[2] = CU_T( "Replace");
	l_rgbCombination[3] = CU_T( "Add");
	l_rgbCombination[4] = CU_T( "Add Signed");
	l_rgbCombination[5] = CU_T( "Dot3 RGB");
	l_rgbCombination[6] = CU_T( "Dot3 RGBA");
	l_rgbCombination[7] = CU_T( "Interpolate");
	l_value = CU_T( "None");
	if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCReplace)
	{
		l_value = CU_T( "Replace");
	}
	else if	(m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCModulate)
	{
		l_value = CU_T( "Modulate");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCAdd)
	{
		l_value = CU_T( "Add");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCAddSigned)
	{
		l_value = CU_T( "Add Signed");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCDot3RGB)
	{
		l_value = CU_T( "Dot3 RGB");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCDot3RGBA)
	{
		l_value = CU_T( "Dot3 RGBA");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetRGBCombination() == CCInterpolate)
	{
		l_value = CU_T( "Interpolate");
	}
	m_rgbCombinationText = new wxStaticText( m_textureUnitPanel, wxID_ANY, CU_T( "RGB Combination"), wxPoint( 90, 36), wxSize( 90, 20));
	m_textureRGBCombination = new wxComboBox( m_textureUnitPanel, eTextureRGBCombination, l_value, wxPoint( 190, 33), wxSize( 80, 20), 8, l_rgbCombination, wxBORDER_SIMPLE | wxCB_READONLY);
	if (m_selectedTextureUnit->GetEnvironment()->GetMode() != EMCombine)
	{
		m_rgbCombinationText->Hide();
		m_textureRGBCombination->Hide();
	}

	wxString l_alphaCombination[6];
	l_alphaCombination[0] = CU_T( "None");
	l_alphaCombination[1] = CU_T( "Modulate");
	l_alphaCombination[2] = CU_T( "Replace");
	l_alphaCombination[3] = CU_T( "Add");
	l_alphaCombination[4] = CU_T( "Add Signed");
	l_alphaCombination[5] = CU_T( "Interpolate");
	l_value = CU_T( "None");
	if (m_selectedTextureUnit->GetEnvironment()->GetAlphaCombination() == ACReplace)
	{
		l_value = CU_T( "Replace");
	}
	else if		(m_selectedTextureUnit->GetEnvironment()->GetAlphaCombination() == ACModulate)
	{
		l_value = CU_T( "Modulate");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetAlphaCombination() == ACAdd)
	{
		l_value = CU_T( "Add");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetAlphaCombination() == ACAddSigned)
	{
		l_value = CU_T( "Add Signed");
	}
	else if (m_selectedTextureUnit->GetEnvironment()->GetAlphaCombination() == ACInterpolate)
	{
		l_value = CU_T( "Interpolate");
	}
	m_alphaCombinationText = new wxStaticText( m_textureUnitPanel, wxID_ANY, CU_T( "Alpha Combination"), wxPoint( 90, 56), wxSize( 90, 17));
	m_textureAlphaCombination = new wxComboBox( m_textureUnitPanel, eTextureAlphaCombination, l_value, wxPoint( 190, 53), wxSize( 80, 20), 8, l_alphaCombination, wxBORDER_SIMPLE | wxCB_READONLY);
	m_environmentConfigButton = new wxButton( m_textureUnitPanel, eEnvironmentConfig, CU_T( "Configure Environment"), wxPoint( 120, 73), wxSize( 140, 20), wxBORDER_SIMPLE);
	if (m_selectedTextureUnit->GetEnvironment()->GetMode() != EMCombine)
	{
		m_alphaCombinationText->Hide();
		m_textureAlphaCombination->Hide();
		m_alphaCombinationText->Hide();
		m_textureAlphaCombination->Hide();
		m_environmentConfigButton->Hide();
	}

	wxArrayString l_modes;
	l_modes.push_back( CU_T( "Normal"));
	l_modes.push_back( CU_T( "Reflexion map"));
	l_modes.push_back( CU_T( "Sphere map"));
	m_texMode = new wxRadioBox( m_textureUnitPanel, eTextureMode, CU_T( "Mode"), wxPoint( 0, 93), wxSize( m_textureUnitPanel->GetClientSize().x, 45), l_modes, 3, wxRA_SPECIFY_COLS);
	m_texMode->SetSelection( (int)m_selectedTextureUnit->GetTextureMapMode());
}

BEGIN_EVENT_TABLE( PassPanel, wxPanel)
	EVT_BUTTON(		eDeleteTextureUnit,			PassPanel::_onDeleteTextureUnit)
	EVT_BUTTON(		eAmbient,					PassPanel::_onAmbientColour)
	EVT_BUTTON(		eDiffuse,					PassPanel::_onDiffuseColour)
	EVT_BUTTON(		eSpecular,					PassPanel::_onSpecularColour)
	EVT_BUTTON(		eEmissive,					PassPanel::_onEmissiveColour)
	EVT_BUTTON(		eTextureBlendColour,		PassPanel::_onBlendColour)
	EVT_BUTTON(		eTextureImage,				PassPanel::_onTextureImage)
	EVT_BUTTON(		eEnvironmentConfig,			PassPanel::_onEnvironmentConfig)
	EVT_BUTTON(		eHasShader,					PassPanel::_onHasShader)
	EVT_TEXT_ENTER( eShininess,					PassPanel::_onShininess)
	EVT_TEXT_ENTER( eTexturePath,				PassPanel::_onTexturePath)
	EVT_COMBOBOX(	eTextureUnits,				PassPanel::_onTextureSelect)
	EVT_COMBOBOX(	eTextureEnvMode,			PassPanel::_onTextureEnvModeSelect)
	EVT_COMBOBOX(	eTextureRGBCombination,		PassPanel::_onTextureRGBCombinationSelect)
	EVT_COMBOBOX(	eTextureAlphaCombination,	PassPanel::_onTextureAlphaCombinationSelect)
	EVT_CHECKBOX(	eDoubleFace,				PassPanel::_onDoubleFace)
	EVT_RADIOBOX(	eTextureMode,				PassPanel::_onTextureMode)
	EVT_SLIDER(		eAlpha,						PassPanel::_onAlpha)
END_EVENT_TABLE()

void PassPanel :: _onDeleteTextureUnit( wxCommandEvent & event)
{
	if ( ! m_pass.null())
	{
		m_texturesComboBox->Clear();
		m_pass->DestroyTextureUnit( m_selectedUnitIndex);
		m_pass->Initialise();
		unsigned int i;
		wxString l_name;
		for (i = 0 ; i < m_pass->GetNbTexUnits() ; i++)
		{
			l_name.clear();
			l_name << i;
			m_texturesComboBox->Insert( l_name, i);
		}
		l_name = CU_T( "New...");
		m_texturesComboBox->Insert( l_name, i);
		m_texturesComboBox->SetValue( CU_T( "New..."));
		_createTextureUnitPanel( -2);
		m_texturesComboBox->Update();
		wxGetApp().GetMainFrame()->ShowPanels();
		if (m_pass->GetNbTexUnits() == 0)
		{
			m_deleteTextureUnit->Disable();
		}
	}
}

void PassPanel :: _onAmbientColour( wxCommandEvent & event)
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
		wxGetApp().GetMainFrame()->ShowPanels();
	}
}

void PassPanel :: _onDiffuseColour( wxCommandEvent & event)
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
		wxGetApp().GetMainFrame()->ShowPanels();
	}
}

void PassPanel :: _onSpecularColour( wxCommandEvent & event)
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
		wxGetApp().GetMainFrame()->ShowPanels();
	}
}

void PassPanel :: _onEmissiveColour( wxCommandEvent & event)
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
		wxGetApp().GetMainFrame()->ShowPanels();
	}
}

void PassPanel :: _onBlendColour( wxCommandEvent & event)
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
		wxGetApp().GetMainFrame()->ShowPanels();
	}
}

void PassPanel :: _onTextureImage( wxCommandEvent & event)
{
	wxFileDialog * l_fdlg = new wxFileDialog( this, CU_T( "Choose an image"), wxEmptyString,
											  wxEmptyString, CU_T( "Fichiers BMP (*.bmp)|*.bmp|Fichiers GIF (*.gif)|*.gif|Fichiers JPG (*.jpg)|*.jpg|Fichiers PNG (*.png)|*.png|Fichiers TGA (*.tga)|*.tga|Images (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.png;*.jpg;*.tga"));
	if (l_fdlg->ShowModal() == wxID_OK)
	{
		wxString l_imagePath = l_fdlg->GetPath();
		m_texPathText->SetLabel( l_imagePath);
		if (m_selectedUnitImage)
		{
			delete m_selectedUnitImage;
			m_selectedUnitImage = NULL;
		}
		SetMaterialImage( l_imagePath.c_str(), 78, 78);
		if ( ! m_selectedTextureUnit.null())
		{
			m_selectedTextureUnit->SetTexture2D( l_imagePath.c_str());
			MaterialManager::SetToInitialise( m_pass->GetParent());
			wxGetApp().GetMainFrame()->ShowPanels();
		}
	}
	l_fdlg->Destroy();
}

void PassPanel :: _onEnvironmentConfig( wxCommandEvent & event)
{
	EnvironmentFrame * l_frame = new EnvironmentFrame( this, CU_T( "Environment Config"), m_pass, m_selectedTextureUnit->GetEnvironment());
	l_frame->Show();
}

void PassPanel :: _onShininess( wxCommandEvent & event)
{
	real l_shininess = GetShininess();
	m_pass->SetShininess( l_shininess);
	m_pass->Initialise();
	wxGetApp().GetMainFrame()->ShowPanels();
}

void PassPanel :: _onTexturePath( wxCommandEvent & event)
{/*
	String l_path = m_texPathText->GetValue().c_str();
	if (l_path.empty())
	{
		return;
	}
	if (m_selectedUnitImage)
	{
		delete m_selectedUnitImage;
		m_selectedUnitImage = NULL;
	}
	SetMaterialImage( l_path.c_str(), 78, 78);
	if (m_selectedTextureUnit)
	{
		m_selectedTextureUnit->SetTexture2D( l_path.c_str());
		MaterialManager::SetToInitialise( m_pass->GetParent());
		wxGetApp().GetMainFrame()->ShowPanels();
	}*/
}

void PassPanel :: _onTextureSelect( wxCommandEvent & event)
{
	int l_selectedUnit = GetTextureUnitIndex();
	_createTextureUnitPanel( l_selectedUnit);
}

void PassPanel :: _onTextureEnvModeSelect( wxCommandEvent & event)
{
	wxString l_value = event.GetString();
	
	if (l_value != CU_T( "Combine"))
	{
		m_rgbCombinationText->Hide();
		m_textureRGBCombination->Hide();
		m_alphaCombinationText->Hide();
		m_textureAlphaCombination->Hide();
		m_environmentConfigButton->Hide();

		if (l_value == CU_T( "Modulate"))
		{
			m_selectedTextureUnit->GetEnvironment()->SetMode( EMModulate);
		}
		else if (l_value == CU_T( "Replace"))
		{
			m_selectedTextureUnit->GetEnvironment()->SetMode( EMReplace);
		}
		else if (l_value == CU_T( "Blend"))
		{
			m_selectedTextureUnit->GetEnvironment()->SetMode( EMBlend);
		}
		else if (l_value == CU_T( "Add"))
		{
			m_selectedTextureUnit->GetEnvironment()->SetMode( EMAdd);
		}
	}
	else
	{
		m_selectedTextureUnit->GetEnvironment()->SetMode( EMCombine);
		m_rgbCombinationText->Show();
		m_textureRGBCombination->Show();
		m_alphaCombinationText->Show();
		m_textureAlphaCombination->Show();
		m_environmentConfigButton->Show();
	}
	wxGetApp().GetMainFrame()->ShowPanels();
}

void PassPanel :: _onTextureRGBCombinationSelect( wxCommandEvent & event)
{
	wxString l_value = event.GetString();

	if (l_value == CU_T( "None"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCNone);
	}
	else if (l_value == CU_T( "Modulate"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCModulate);
	}
	else if (l_value == CU_T( "Replace"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCReplace);
	}
	else if (l_value == CU_T( "Add"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCAdd);
	}
	else if (l_value == CU_T( "Add Signed"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCAddSigned);
	}
	else if (l_value == CU_T( "Dot3 RGB"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCDot3RGB);
	}
	else if (l_value == CU_T( "Dot3 RGBA"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCDot3RGBA);
	}
	else if (l_value == CU_T( "Interpolate"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetRGBCombination( CCInterpolate);
	}
	wxGetApp().GetMainFrame()->ShowPanels();
}

void PassPanel :: _onTextureAlphaCombinationSelect( wxCommandEvent & event)
{
	wxString l_value = event.GetString();

	if (l_value == CU_T( "None"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACNone);
	}
	else if (l_value == CU_T( "Modulate"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACModulate);
	}
	else if (l_value == CU_T( "Replace"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACReplace);
	}
	else if (l_value == CU_T( "Add"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACAdd);
	}
	else if (l_value == CU_T( "Add Signed"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACAddSigned);
	}
	else if (l_value == CU_T( "Interpolate"))
	{
		m_selectedTextureUnit->GetEnvironment()->SetAlphaCombination( ACInterpolate);
	}
	wxGetApp().GetMainFrame()->ShowPanels();
}

void PassPanel :: _onDoubleFace( wxCommandEvent & event)
{
	bool l_double = m_doubleFace->IsChecked();
	m_pass->SetDoubleFace( l_double);
	wxGetApp().GetMainFrame()->ShowPanels();
}

void PassPanel :: _onHasShader( wxCommandEvent & event)
{
	ShaderDialog l_dialog( this, m_pass);

	l_dialog.ShowModal();
}

void PassPanel :: _onTextureMode( wxCommandEvent & event)
{
	m_selectedTextureUnit->SetTextureMapMode( (TextureUnit::eMAP_MODE)m_texMode->GetSelection());
}

void PassPanel :: _onAlpha( wxCommandEvent & event)
{
	m_pass->SetAlpha( real( event.GetInt()) / 255.0f);
}