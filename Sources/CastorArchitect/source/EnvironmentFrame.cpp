#include "PrecompiledHeader.h"

#include "EnvironmentFrame.h"

using namespace Castor3D;
using namespace CastorArchitect;

EnvironmentFrame :: EnvironmentFrame( wxWindow * parent, const wxString & title,
										  PassPtr p_pass, TextureEnvironmentPtr p_env,
										  const wxPoint & pos, const wxSize & size)
	:	wxFrame( parent, wxID_ANY, "Environment mode", pos, size),
		m_environment( p_env),
		m_pass( p_pass)
{
	int l_currentTop = 5;
	RGBCombination l_RGBCombination = m_environment->GetRGBCombination();

	if (l_RGBCombination != CCNone)
	{
		m_RGBSources[0] = m_environment->GetRGBSource( 0, m_RGBTextures[0]);
		m_RGBSources[1] = m_environment->GetRGBSource( 1, m_RGBTextures[1]);
		m_RGBSources[2] = m_environment->GetRGBSource( 2, m_RGBTextures[2]);
		m_RGBOperands[0] = m_environment->GetRGBOperand( 0);
		m_RGBOperands[1] = m_environment->GetRGBOperand( 1);
		m_RGBOperands[2] = m_environment->GetRGBOperand( 2);

		_createRGBSourceComboBox( 0, l_currentTop);
		l_currentTop += 20;
		_createRGBOperandComboBox( 0, l_currentTop);
		l_currentTop += 25;

		if (l_RGBCombination != CCReplace)
		{
			_createRGBSourceComboBox( 1, l_currentTop);
			l_currentTop += 20;
			_createRGBOperandComboBox( 1, l_currentTop);
			l_currentTop += 25;

			if (l_RGBCombination == CCInterpolate)
			{
				_createRGBSourceComboBox( 2, l_currentTop);
				l_currentTop += 20;
				_createRGBOperandComboBox( 2, l_currentTop);
				l_currentTop += 25;
			}
		}
		l_currentTop += 5;
	}
	AlphaCombination l_AlphaCombination = m_environment->GetAlphaCombination();

	if (l_AlphaCombination != ACNone)
	{
		m_AlphaSources[0] = m_environment->GetAlphaSource( 0, m_AlphaTextures[0]);
		m_AlphaSources[1] = m_environment->GetAlphaSource( 1, m_AlphaTextures[1]);
		m_AlphaSources[2] = m_environment->GetAlphaSource( 2, m_AlphaTextures[2]);
		m_AlphaOperands[0] = m_environment->GetAlphaOperand( 0);
		m_AlphaOperands[1] = m_environment->GetAlphaOperand( 1);
		m_AlphaOperands[2] = m_environment->GetAlphaOperand( 2);

		_createAlphaSourceComboBox( 0, l_currentTop);
		l_currentTop += 20;
		_createAlphaOperandComboBox( 0, l_currentTop);
		l_currentTop += 25;

		if (l_AlphaCombination != ACReplace)
		{
			_createAlphaSourceComboBox( 1, l_currentTop);
			l_currentTop += 20;
			_createAlphaOperandComboBox( 1, l_currentTop);
			l_currentTop += 25;

			if (l_AlphaCombination == ACInterpolate)
			{
				_createAlphaSourceComboBox( 2, l_currentTop);
				l_currentTop += 20;
				_createAlphaOperandComboBox( 2, l_currentTop);
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
	m_environment = NULL;
	m_pass = NULL;
}

void EnvironmentFrame :: _createRGBSourceComboBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_environment.null() || m_pass.null())
	{
		return;
	}
	int l_textureIndex = 0;
	CombinationSource l_source = m_environment->GetRGBSource( p_index, l_textureIndex);
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
		case CSCurrentTexture : l_value = CU_T( "Current Texture");break;
		case CSConstant : l_value = CU_T( "Constant");break;
		case CSPrevious : l_value = CU_T( "Previous");break;
		case CSPrimaryColour : l_value = CU_T( "Primary Colour");break;
		case CSTexture :
			l_value = CU_T( "Texture ");
			l_value << l_textureIndex;
		break;
	}
	std::cout << "RGB source " << p_index << " : " << l_value.char_str() << " (" << l_source << ")\n";
	wxString l_text = CU_T( "RGB Source ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 80, 20));
	m_RGBSourcesCB[p_index] = new wxComboBox( this, eRGBSrc0 + p_index, l_value,
											  wxPoint( 90, p_currentTop - 3), wxSize( 105, 20),
											  l_nbChoices, l_choices,
											  wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _createRGBOperandComboBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_environment.null())
	{
		return;
	}
	RGBOperand l_operand = m_environment->GetRGBOperand( p_index);
	unsigned int l_nbChoices = 4;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Source Colour");
	l_choices[1] = CU_T( "One Minus Source Colour");
	l_choices[2] = CU_T( "Source Alpha");
	l_choices[3] = CU_T( "One Minus Source Alpha");
	wxString l_value;
	switch (l_operand)
	{
		case COSrcColour : l_value = CU_T( "Source Colour");break;
		case COOneMinusSrcColour : l_value = CU_T( "One Minus Source Colour");break;
		case COSrcAlpha : l_value = CU_T( "Source Alpha");break;
		case COOneMinusSrcAlpha : l_value = CU_T( "One Minus Source Alpha");break;
		break;
	}
	std::cout << "RGB operand " << p_index << " : " << l_value.char_str() << " (" << l_operand << ")\n";
	wxString l_text = CU_T( "RGB Operand ");
	l_text << p_index;
	new wxStaticText( this, wxID_ANY, l_text, wxPoint( 5, p_currentTop), wxSize( 80, 20));
	m_RGBOperandsCB[p_index] = new wxComboBox( this, eRGBOperand0 + p_index, l_value,
											   wxPoint( 90, p_currentTop - 3), wxSize( 105, 20),
											   l_nbChoices, l_choices,
											   wxBORDER_SIMPLE | wxCB_READONLY);
	delete [] l_choices;
}

void EnvironmentFrame :: _createAlphaSourceComboBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3)
	{
		return;
	}
	int l_textureIndex = 0;
	CombinationSource l_source = m_environment->GetAlphaSource( p_index, l_textureIndex);
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
		case CSCurrentTexture : l_value = CU_T( "Current Texture");break;
		case CSConstant : l_value = CU_T( "Constant");break;
		case CSPrevious : l_value = CU_T( "Previous");break;
		case CSPrimaryColour : l_value = CU_T( "Primary Colour");break;
		case CSTexture :
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

void EnvironmentFrame :: _createAlphaOperandComboBox( unsigned int p_index, int p_currentTop)
{
	if (p_index >= 3 || m_environment.null())
	{
		return;
	}
	AlphaOperand l_operand = m_environment->GetAlphaOperand( p_index);
	unsigned int l_nbChoices = 2;
	wxString * l_choices = new wxString[l_nbChoices];
	l_choices[0] = CU_T( "Source Alpha");
	l_choices[1] = CU_T( "One Minus Source Alpha");
	wxString l_value;
	switch (l_operand)
	{
		case AOSrcAlpha : l_value = CU_T( "Source Alpha");break;
		case AOOneMinusSrcAlpha : l_value = CU_T( "One Minus Source Alpha");break;
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
	RGBCombination l_RGBCombination = m_environment->GetRGBCombination();
	if (l_RGBCombination != CCNone)
	{
		m_environment->SetRGBSource( 0, m_RGBSources[0], m_RGBTextures[0]);
		m_environment->SetRGBOperand( 0, m_RGBOperands[0]);
		if (l_RGBCombination != CCReplace)
		{
			m_environment->SetRGBSource( 1, m_RGBSources[1], m_RGBTextures[1]);
			m_environment->SetRGBOperand( 1, m_RGBOperands[1]);
			if (l_RGBCombination == CCInterpolate)
			{
				m_environment->SetRGBSource( 2, m_RGBSources[2], m_RGBTextures[2]);
				m_environment->SetRGBOperand( 2, m_RGBOperands[2]);
			}
		}
	}
	AlphaCombination l_AlphaCombination = m_environment->GetAlphaCombination();
	if (l_AlphaCombination != ACNone)
	{
		m_environment->SetAlphaSource( 0, m_AlphaSources[0], m_AlphaTextures[0]);
		m_environment->SetAlphaOperand( 0, m_AlphaOperands[0]);
		if (l_AlphaCombination != ACReplace)
		{
			m_environment->SetAlphaSource( 1, m_AlphaSources[1], m_AlphaTextures[1]);
			m_environment->SetAlphaOperand( 1, m_AlphaOperands[1]);
			if (l_AlphaCombination == ACInterpolate)
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
	wxString l_value = m_RGBSourcesCB[0]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetRGBSource( 0, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetRGBSource( 0, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetRGBSource( 0, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetRGBSource( 0, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetRGBSource( 0, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onRGBSource1Select( wxCommandEvent & event)
{
	wxString l_value = m_RGBSourcesCB[1]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetRGBSource( 1, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetRGBSource( 1, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetRGBSource( 1, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetRGBSource( 1, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetRGBSource( 1, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onRGBSource2Select( wxCommandEvent & event)
{
	wxString l_value = m_RGBSourcesCB[2]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetRGBSource( 2, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetRGBSource( 2, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetRGBSource( 2, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetRGBSource( 2, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetRGBSource( 2, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onRGBOperand0Select( wxCommandEvent & event)
{
	wxString l_value = m_RGBOperandsCB[0]->GetValue();
	if  (l_value == CU_T( "Source Colour"))
	{
		m_environment->SetRGBOperand( 0, COSrcColour);
	}
	else if (l_value == CU_T( "One Minus Source Colour"))
	{
		m_environment->SetRGBOperand( 0, COOneMinusSrcColour);
	}
	else if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetRGBOperand( 0, COSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetRGBOperand( 0, COOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onRGBOperand1Select( wxCommandEvent & event)
{
	wxString l_value = m_RGBOperandsCB[1]->GetValue();
	if  (l_value == CU_T( "Source Colour"))
	{
		m_environment->SetRGBOperand( 1, COSrcColour);
	}
	else if (l_value == CU_T( "One Minus Source Colour"))
	{
		m_environment->SetRGBOperand( 1, COOneMinusSrcColour);
	}
	else if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetRGBOperand( 1, COSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetRGBOperand( 1, COOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onRGBOperand2Select( wxCommandEvent & event)
{
	wxString l_value = m_RGBOperandsCB[2]->GetValue();
	if  (l_value == CU_T( "Source Colour"))
	{
		m_environment->SetRGBOperand( 2, COSrcColour);
	}
	else if (l_value == CU_T( "One Minus Source Colour"))
	{
		m_environment->SetRGBOperand( 2, COOneMinusSrcColour);
	}
	else if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetRGBOperand( 2, COSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetRGBOperand( 2, COOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onAlphaSource0Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaSourcesCB[0]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetAlphaSource( 0, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetAlphaSource( 0, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetAlphaSource( 0, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetAlphaSource( 0, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetAlphaSource( 0, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onAlphaSource1Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaSourcesCB[1]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetAlphaSource( 1, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetAlphaSource( 1, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetAlphaSource( 1, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetAlphaSource( 1, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetAlphaSource( 1, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onAlphaSource2Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaSourcesCB[2]->GetValue();
	if  (l_value == CU_T( "Current Texture"))
	{
		m_environment->SetAlphaSource( 2, CSCurrentTexture);
	}
	else if (l_value == CU_T( "Constant"))
	{
		m_environment->SetAlphaSource( 2, CSConstant);
	}
	else if (l_value == CU_T( "Previous"))
	{
		m_environment->SetAlphaSource( 2, CSPrevious);
	}
	else if (l_value == CU_T( "Primary Colour"))
	{
		m_environment->SetAlphaSource( 2, CSPrimaryColour);
	}
	else
	{
		l_value = l_value.substr( 0, l_value.find_last_of( ' '));
		l_value.Trim();
		int l_index = atoi( l_value.char_str());
		m_environment->SetAlphaSource( 2, CSTexture, l_index);
	}
}

void EnvironmentFrame :: _onAlphaOperand0Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaOperandsCB[0]->GetValue();
	if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetAlphaOperand( 0, AOSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetAlphaOperand( 0, AOOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onAlphaOperand1Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaOperandsCB[1]->GetValue();
	if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetAlphaOperand( 1, AOSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetAlphaOperand( 1, AOOneMinusSrcAlpha);
	}
}

void EnvironmentFrame :: _onAlphaOperand2Select( wxCommandEvent & event)
{
	wxString l_value = m_AlphaOperandsCB[2]->GetValue();
	if (l_value == CU_T( "Source Alpha"))
	{
		m_environment->SetAlphaOperand( 2, AOSrcAlpha);
	}
	else if (l_value == CU_T( "One Minus Source Alpha"))
	{
		m_environment->SetAlphaOperand( 2, AOOneMinusSrcAlpha);
	}
}