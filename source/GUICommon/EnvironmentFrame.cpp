#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/EnvironmentFrame.h"

using namespace Castor3D;
using namespace GuiCommon;

EnvironmentFrame :: EnvironmentFrame( wxWindow * parent, const wxString & title,
									  PassPtr p_pass, TextureEnvironmentPtr p_env,
									  const wxPoint & pos, const wxSize & size)
	:	wxFrame( parent, wxID_ANY, CU_T( "Environment mode"), pos, size),
		m_environment( p_env),
		m_pass( p_pass)
{
	int l_currentTop = 5;
	TextureEnvironment::eRGB_COMBINATION l_RGBCombination = m_environment->GetRgbCombination();

	if (l_RGBCombination != TextureEnvironment::eRgbCombiNone)
	{
		m_RGBSources[0] = m_environment->GetRgbSource( 0, m_RGBTextures[0]);
		m_RGBSources[1] = m_environment->GetRgbSource( 1, m_RGBTextures[1]);
		m_RGBSources[2] = m_environment->GetRgbSource( 2, m_RGBTextures[2]);
		m_RGBOperands[0] = m_environment->GetRgbOperand( 0);
		m_RGBOperands[1] = m_environment->GetRgbOperand( 1);
		m_RGBOperands[2] = m_environment->GetRgbOperand( 2);

		_createRGBSourceCubeBox( 0, l_currentTop);
		l_currentTop += 20;
		_createRGBOperandCubeBox( 0, l_currentTop);
		l_currentTop += 25;

		if (l_RGBCombination != TextureEnvironment::eRgbCombiReplace)
		{
			_createRGBSourceCubeBox( 1, l_currentTop);
			l_currentTop += 20;
			_createRGBOperandCubeBox( 1, l_currentTop);
			l_currentTop += 25;

			if (l_RGBCombination == TextureEnvironment::eRgbCombiInterpolate)
			{
				_createRGBSourceCubeBox( 2, l_currentTop);
				l_currentTop += 20;
				_createRGBOperandCubeBox( 2, l_currentTop);
				l_currentTop += 25;
			}
		}
		l_currentTop += 5;
	}

	TextureEnvironment::eALPHA_COMBINATION l_AlphaCombination = m_environment->GetAlphaCombination();

	if (l_AlphaCombination != TextureEnvironment::eAlphaCombiNone)
	{
		m_AlphaSources[0] = m_environment->GetAlphaSource( 0, m_AlphaTextures[0]);
		m_AlphaSources[1] = m_environment->GetAlphaSource( 1, m_AlphaTextures[1]);
		m_AlphaSources[2] = m_environment->GetAlphaSource( 2, m_AlphaTextures[2]);
		m_AlphaOperands[0] = m_environment->GetAlphaOperand( 0);
		m_AlphaOperands[1] = m_environment->GetAlphaOperand( 1);
		m_AlphaOperands[2] = m_environment->GetAlphaOperand( 2);

		_createAlphaSourceCubeBox( 0, l_currentTop);
		l_currentTop += 20;
		_createAlphaOperandCubeBox( 0, l_currentTop);
		l_currentTop += 25;

		if (l_AlphaCombination != TextureEnvironment::eAlphaCombiReplace)
		{
			_createAlphaSourceCubeBox( 1, l_currentTop);
			l_currentTop += 20;
			_createAlphaOperandCubeBox( 1, l_currentTop);
			l_currentTop += 25;

			if (l_AlphaCombination == TextureEnvironment::eAlphaCombiInterpolate)
			{
				_createAlphaSourceCubeBox( 2, l_currentTop);
				l_currentTop += 20;
				_createAlphaOperandCubeBox( 2, l_currentTop);
				l_currentTop += 25;
			}
		}
		l_currentTop += 5;
	}

	m_OKButton = new wxButton( this, eOK, CU_T( "OK"), wxPoint( 30, l_currentTop), wxSize( 60, 20), wxBORDER_SIMPLE);
	m_cancelButton = new wxButton( this, eCancel, CU_T( "Cancel"), wxPoint( 130, l_currentTop), wxSize( 60, 20), wxBORDER_SIMPLE);

	SetSize( 220, l_currentTop + 50);
}

EnvironmentFrame :: ~EnvironmentFrame()
{
}

void EnvironmentFrame :: _createRGBSourceCubeBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_pass == NULL)
	{
		return;
	}
	int l_textureIndex = 0;
	TextureEnvironment::eCOMBINATION_SOURCE l_source = m_environment->GetRgbSource( p_index, l_textureIndex);
	unsigned int l_nbUnits = m_pass->GetNbTexUnits();
	unsigned int l_nbChoices = 4 + l_nbUnits;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Current Texture");
	l_choices[1] = CU_T( "Constant");
	l_choices[2] = CU_T( "Primary Colour");
	l_choices[3] = CU_T( "Previous");
	wxString l_value = wxEmptyString;
	for( unsigned int i = 0 ; i < l_nbUnits ; i++)
	{
		l_value = CU_T( "Texture ");
		l_value << i;
		l_choices[4 + i] = l_value;
	}
	switch (l_source)
	{
		case TextureEnvironment::eCombiSrcCurrentTexture : l_value = CU_T( "Current Texture");break;
		case TextureEnvironment::eCombiSrcConstant : l_value = CU_T( "Constant");break;
		case TextureEnvironment::eCombiSrcPrevious : l_value = CU_T( "Previous");break;
		case TextureEnvironment::eCombiSrcPrimaryColour : l_value = CU_T( "Primary Colour");break;
		case TextureEnvironment::eCombiSrcTexture :
			l_value = CU_T( "Texture ");
			l_value << l_textureIndex;
		break;
	}
//	std::cout << "RGB source " << p_index << " : " << l_value.char_str() << " (" << l_source << ")\n";
	wxString l_text = CU_T( "RGB Source ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 80, 20));
	m_RGBSourcesCB[p_index] = new wxComboBox( this, eRGBSrc0 + p_index, l_value,
											  wxPoint( 90, p_currentTop - 3), wxSize( 105, 20),
											  l_nbChoices, l_choices,
											  wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _createRGBOperandCubeBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_pass == NULL)
	{
		return;
	}
	TextureEnvironment::eRGB_OPERAND l_operand = m_environment->GetRgbOperand( p_index);
	unsigned int l_nbChoices = 4;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Source Colour");
	l_choices[1] = CU_T( "One Minus Source Colour");
	l_choices[2] = CU_T( "Source Alpha");
	l_choices[3] = CU_T( "One Minus Source Alpha");
	wxString l_value;
	switch (l_operand)
	{
		case TextureEnvironment::eRgbOpeSrcColour : l_value = CU_T( "Source Colour");break;
		case TextureEnvironment::eRgbOpeOneMinusSrcColour : l_value = CU_T( "One Minus Source Colour");break;
		case TextureEnvironment::eRgbOpeSrcAlpha : l_value = CU_T( "Source Alpha");break;
		case TextureEnvironment::eRgbOpeOneMinusSrcAlpha : l_value = CU_T( "One Minus Source Alpha");break;
		break;
	}
//	std::cout << "RGB operand " << p_index << " : " << l_value.char_str() << " (" << l_operand << ")\n";
	wxString l_text = CU_T( "RGB Operand ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 80, 20));
	m_RGBOperandsCB[p_index] = new wxComboBox( this, eRGBOperand0 + p_index, l_value,
											   wxPoint( 90, p_currentTop - 3), wxSize( 105, 20),
											   l_nbChoices, l_choices,
											   wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _createAlphaSourceCubeBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_pass == NULL)
	{
		return;
	}
	int l_textureIndex = 0;
	TextureEnvironment::eCOMBINATION_SOURCE l_source = m_environment->GetAlphaSource( p_index, l_textureIndex);
	unsigned int l_nbUnits = m_pass->GetNbTexUnits();
	unsigned int l_nbChoices = 4 + l_nbUnits;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Current Texture");
	l_choices[1] = CU_T( "Constant");
	l_choices[2] = CU_T( "Primary Colour");
	l_choices[3] = CU_T( "Previous");
	wxString l_value = wxEmptyString;
	for( unsigned int i = 0 ; i < l_nbUnits ; i++)
	{
		l_value = CU_T( "Texture ");
		l_value << i;
		l_choices[4 + i] = l_value;
	}
	switch (l_source)
	{
		case TextureEnvironment::eCombiSrcCurrentTexture : l_value = CU_T( "Current Texture");break;
		case TextureEnvironment::eCombiSrcConstant : l_value = CU_T( "Constant");break;
		case TextureEnvironment::eCombiSrcPrevious : l_value = CU_T( "Previous");break;
		case TextureEnvironment::eCombiSrcPrimaryColour : l_value = CU_T( "Primary Colour");break;
		case TextureEnvironment::eCombiSrcTexture :
			l_value = CU_T( "Texture ");
			l_value << l_textureIndex;
		break;
	}
	wxString l_text = CU_T( "Alpha Source ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 90, 20));
	m_AlphaSourcesCB[p_index] = new wxComboBox( this, eAlphaSrc0 + p_index, l_value,
												wxPoint( 100, p_currentTop - 3), wxSize( 105, 20),
												l_nbChoices, l_choices,
												wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _createAlphaOperandCubeBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_pass == NULL)
	{
		return;
	}
	TextureEnvironment::eALPHA_OPERAND l_operand = m_environment->GetAlphaOperand( p_index);
	unsigned int l_nbChoices = 2;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Source Alpha");
	l_choices[1] = CU_T( "One Minus Source Alpha");
	wxString l_value;
	switch (l_operand)
	{
		case TextureEnvironment::eAlphaOpeSrcAlpha : l_value = CU_T( "Source Alpha");break;
		case TextureEnvironment::eAlphaOpeOneMinusSrcAlpha : l_value = CU_T( "One Minus Source Alpha");break;
		break;
	}
	wxString l_text = CU_T( "Alpha Operand ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 90, 20));
	m_AlphaOperandsCB[p_index] = new wxComboBox( this, eAlphaOperand0 + p_index, l_value,
											   wxPoint( 100, p_currentTop - 3), wxSize( 105, 20),
											   l_nbChoices, l_choices,
											   wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _onRGBSourceSelect( size_t p_uiIndex)
{
	wxString l_value = m_RGBSourcesCB[p_uiIndex]->GetValue();

	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetRgbSource( p_uiIndex, TextureEnvironment::eCombiSrcCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetRgbSource( p_uiIndex, TextureEnvironment::eCombiSrcConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetRgbSource( p_uiIndex, TextureEnvironment::eCombiSrcPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetRgbSource( p_uiIndex, TextureEnvironment::eCombiSrcPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.c_str());
		m_environment->SetRgbSource( p_uiIndex, TextureEnvironment::eCombiSrcTexture, l_index);
	}
}

void EnvironmentFrame :: _onRGBOperandSelect( size_t p_uiIndex)
{
	wxString l_value = m_RGBOperandsCB[p_uiIndex]->GetValue();

	if  (l_value == CU_T( "Source Colour"))
	{
		m_environment->SetRgbOperand( p_uiIndex, TextureEnvironment::eRgbOpeSrcColour);
	}
	else if (l_value == CU_T( "One Minus Source Colour"))
	{
		m_environment->SetRgbOperand( p_uiIndex, TextureEnvironment::eRgbOpeOneMinusSrcColour);
	}
	else if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetRgbOperand( p_uiIndex, TextureEnvironment::eRgbOpeSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetRgbOperand( p_uiIndex, TextureEnvironment::eRgbOpeOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onAlphaSourceSelect( size_t p_uiIndex)
{
	wxString l_value = m_AlphaSourcesCB[p_uiIndex]->GetValue();

	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetAlphaSource( p_uiIndex, TextureEnvironment::eCombiSrcCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetAlphaSource( p_uiIndex, TextureEnvironment::eCombiSrcConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetAlphaSource( p_uiIndex, TextureEnvironment::eCombiSrcPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetAlphaSource( p_uiIndex, TextureEnvironment::eCombiSrcPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.c_str());
		m_environment->SetAlphaSource( p_uiIndex, TextureEnvironment::eCombiSrcTexture, l_index);
	}
}

void EnvironmentFrame :: _onAlphaOperandSelect( size_t p_uiIndex)
{
	wxString l_value = m_AlphaOperandsCB[p_uiIndex]->GetValue();

	if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetAlphaOperand( p_uiIndex, TextureEnvironment::eAlphaOpeSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetAlphaOperand( p_uiIndex, TextureEnvironment::eAlphaOpeOneMinusSrcAlpha);
	}
}

BEGIN_EVENT_TABLE( EnvironmentFrame, wxFrame)
	EVT_BUTTON(		eOK,				EnvironmentFrame::_onOK)
	EVT_BUTTON(		eCancel,			EnvironmentFrame::_onCancel)
	EVT_COMBOBOX(	eRGBSrc0,			EnvironmentFrame::_onRGBSource0Select)
	EVT_COMBOBOX(	eRGBSrc1,			EnvironmentFrame::_onRGBSource1Select)
	EVT_COMBOBOX(	eRGBSrc2,			EnvironmentFrame::_onRGBSource2Select)
	EVT_COMBOBOX(	eRGBOperand0,		EnvironmentFrame::_onRGBOperand0Select)
	EVT_COMBOBOX(	eRGBOperand1,		EnvironmentFrame::_onRGBOperand1Select)
	EVT_COMBOBOX(	eRGBOperand2,		EnvironmentFrame::_onRGBOperand2Select)
	EVT_COMBOBOX(	eAlphaSrc0,			EnvironmentFrame::_onAlphaSource0Select)
	EVT_COMBOBOX(	eAlphaSrc1,			EnvironmentFrame::_onAlphaSource1Select)
	EVT_COMBOBOX(	eAlphaSrc2,			EnvironmentFrame::_onAlphaSource2Select)
	EVT_COMBOBOX(	eAlphaOperand0,		EnvironmentFrame::_onAlphaOperand0Select)
	EVT_COMBOBOX(	eAlphaOperand1,		EnvironmentFrame::_onAlphaOperand1Select)
	EVT_COMBOBOX(	eAlphaOperand2,		EnvironmentFrame::_onAlphaOperand2Select)
END_EVENT_TABLE()

void EnvironmentFrame :: _onOK( wxCommandEvent & event)
{
	Close();
}

void EnvironmentFrame :: _onCancel( wxCommandEvent & event)
{
	TextureEnvironment::eRGB_COMBINATION l_RGBCombination = m_environment->GetRgbCombination();

	if (l_RGBCombination != TextureEnvironment::eRgbCombiNone)
	{
		m_environment->SetRgbSource( 0, m_RGBSources[0], m_RGBTextures[0]);
		m_environment->SetRgbOperand( 0, m_RGBOperands[0]);

		if (l_RGBCombination != TextureEnvironment::eRgbCombiReplace)
		{
			m_environment->SetRgbSource( 1, m_RGBSources[1], m_RGBTextures[1]);
			m_environment->SetRgbOperand( 1, m_RGBOperands[1]);

			if (l_RGBCombination == TextureEnvironment::eRgbCombiInterpolate)
			{
				m_environment->SetRgbSource( 2, m_RGBSources[2], m_RGBTextures[2]);
				m_environment->SetRgbOperand( 2, m_RGBOperands[2]);
			}
		}
	}

	TextureEnvironment::eALPHA_COMBINATION l_AlphaCombination = m_environment->GetAlphaCombination();

	if (l_AlphaCombination != TextureEnvironment::eAlphaCombiNone)
	{
		m_environment->SetAlphaSource( 0, m_AlphaSources[0], m_AlphaTextures[0]);
		m_environment->SetAlphaOperand( 0, m_AlphaOperands[0]);

		if (l_AlphaCombination != TextureEnvironment::eAlphaCombiReplace)
		{
			m_environment->SetAlphaSource( 1, m_AlphaSources[1], m_AlphaTextures[1]);
			m_environment->SetAlphaOperand( 1, m_AlphaOperands[1]);

			if (l_AlphaCombination == TextureEnvironment::eAlphaCombiInterpolate)
			{
				m_environment->SetAlphaSource( 2, m_AlphaSources[2], m_AlphaTextures[2]);
				m_environment->SetAlphaOperand( 2, m_AlphaOperands[2]);
			}
		}
	}
	Close();
}

void EnvironmentFrame :: _onRGBSource0Select( wxCommandEvent & event)
{
	_onRGBSourceSelect( 0);
}

void EnvironmentFrame :: _onRGBSource1Select( wxCommandEvent & event)
{
	_onRGBSourceSelect( 1);
}

void EnvironmentFrame :: _onRGBSource2Select( wxCommandEvent & event)
{
	_onRGBSourceSelect( 2);
}

void EnvironmentFrame :: _onRGBOperand0Select( wxCommandEvent & event)
{
	_onRGBOperandSelect( 0);
}

void EnvironmentFrame :: _onRGBOperand1Select( wxCommandEvent & event)
{
	_onRGBOperandSelect( 1);
}

void EnvironmentFrame :: _onRGBOperand2Select( wxCommandEvent & event)
{
	_onRGBOperandSelect( 2);
}

void EnvironmentFrame :: _onAlphaSource0Select( wxCommandEvent & event)
{
	_onAlphaSourceSelect( 0);
}

void EnvironmentFrame :: _onAlphaSource1Select( wxCommandEvent & event)
{
	_onAlphaSourceSelect( 1);
}

void EnvironmentFrame :: _onAlphaSource2Select( wxCommandEvent & event)
{
	_onAlphaSourceSelect( 2);
}

void EnvironmentFrame :: _onAlphaOperand0Select( wxCommandEvent & event)
{
	_onAlphaOperandSelect( 0);
}

void EnvironmentFrame :: _onAlphaOperand1Select( wxCommandEvent & event)
{
	_onAlphaOperandSelect( 1);
}

void EnvironmentFrame :: _onAlphaOperand2Select( wxCommandEvent & event)
{
	_onAlphaOperandSelect( 2);
}
