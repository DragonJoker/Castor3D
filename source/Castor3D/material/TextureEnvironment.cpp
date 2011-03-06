#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/TextureEnvironment.h"

using namespace Castor3D;

const String TextureEnvironment::StringMode[eNbEnvModes]							= { "replace", "modulate", "blend", "add", "combine" };
const String TextureEnvironment::StringRgbCombinations[eNbRgbCombinations]			= { "none", "replace", "modulate", "add", "add_signed", "dot3_rgb", "dot3_alpha", "interpolate" };
const String TextureEnvironment::StringRgbOperands[eNbRgbOperands]					= { "src_colour", "one_minus_src_colour", "src_alpha", "one_minus_src_alpha" };
const String TextureEnvironment::StringAlphaCombinations[eNbAlphaCombinations]		= { "none", "replace", "modulate", "add", "add_signed", "interpolate" };
const String TextureEnvironment::StringAlphaOperands[eNbAlphaOperands]				= { "src_alpha", "one_minus_src_alpha" };
const String TextureEnvironment::StringCombinationSources[eNbCombinationSources]	= { "texture", "current_texture", "constant", "primary_colour", "previous" };

std::map <String, TextureEnvironment::eMODE>				TextureEnvironment::MapModes;
std::map <String, TextureEnvironment::eRGB_COMBINATION>		TextureEnvironment::MapRgbCombinations;
std::map <String, TextureEnvironment::eRGB_OPERAND>			TextureEnvironment::MapRgbOperands;
std::map <String, TextureEnvironment::eALPHA_COMBINATION>	TextureEnvironment::MapAlphaCombinations;
std::map <String, TextureEnvironment::eALPHA_OPERAND>		TextureEnvironment::MapAlphaOperands;
std::map <String, TextureEnvironment::eCOMBINATION_SOURCE>	TextureEnvironment::MapCombinationSources;

TextureEnvironment :: TextureEnvironment()
	:	m_eMode						( eModeReplace)
	,	m_eRgbCombination			( eRgbCombiNone)
	,	m_eAlphaCombination			( eAlphaCombiNone)
	,	m_iRgbTextureSourceIndex	( 0)
	,	m_iAlphaTextureSourceIndex	( 0)
	,	m_rRgbScale					( 1.0)
	,	m_rAlphaScale				( 1.0)
	,	m_ptRgbCombinationSources	( eCombiSrcCurrentTexture, eCombiSrcPrevious, eCombiSrcConstant)
	,	m_ptAlphaCombinationSources	( eCombiSrcCurrentTexture, eCombiSrcPrevious, eCombiSrcConstant)
	,	m_ptRgbOperands				( eRgbOpeSrcColour, eRgbOpeSrcColour, eRgbOpeSrcAlpha)
	,	m_ptAlphaOperands			( eAlphaOpeSrcAlpha, eAlphaOpeSrcAlpha, eAlphaOpeSrcAlpha)
{
	_initialiseMaps();
}

TextureEnvironment :: TextureEnvironment( const TextureEnvironment & p_copy)
	:	m_eMode						( p_copy.m_eMode)
	,	m_eRgbCombination			( p_copy.m_eRgbCombination)
	,	m_eAlphaCombination			( p_copy.m_eAlphaCombination)
	,	m_iRgbTextureSourceIndex	( p_copy.m_iRgbTextureSourceIndex)
	,	m_iAlphaTextureSourceIndex	( p_copy.m_iAlphaTextureSourceIndex)
	,	m_rRgbScale					( p_copy.m_rRgbScale)
	,	m_rAlphaScale				( p_copy.m_rAlphaScale)
	,	m_ptRgbCombinationSources	( p_copy.m_ptRgbCombinationSources)
	,	m_ptAlphaCombinationSources	( p_copy.m_ptAlphaCombinationSources)
	,	m_ptRgbOperands				( p_copy.m_ptRgbOperands)
	,	m_ptAlphaOperands			( p_copy.m_ptAlphaOperands)
{
}

TextureEnvironment & TextureEnvironment :: operator =( const TextureEnvironment & p_copy)
{
	m_eMode						= p_copy.m_eMode;
	m_eRgbCombination			= p_copy.m_eRgbCombination;
	m_eAlphaCombination			= p_copy.m_eAlphaCombination;
	m_iRgbTextureSourceIndex	= p_copy.m_iRgbTextureSourceIndex;
	m_iAlphaTextureSourceIndex	= p_copy.m_iAlphaTextureSourceIndex;
	m_rRgbScale					= p_copy.m_rRgbScale;
	m_rAlphaScale				= p_copy.m_rAlphaScale;
	m_ptRgbCombinationSources	= p_copy.m_ptRgbCombinationSources;
	m_ptAlphaCombinationSources	= p_copy.m_ptAlphaCombinationSources;
	m_ptRgbOperands				= p_copy.m_ptRgbOperands;
	m_ptAlphaOperands			= p_copy.m_ptAlphaOperands;

	return * this;
}

TextureEnvironment :: ~TextureEnvironment()
{
}

void TextureEnvironment :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	m_pRenderer->Render();
}

void TextureEnvironment :: SetRgbSource( unsigned int p_uiIndex, 
										 eCOMBINATION_SOURCE p_source, 
										 int p_textureIndex)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_ptRgbCombinationSources[p_uiIndex] = p_source;
	m_iRgbTextureSourceIndex = p_textureIndex;
}

void TextureEnvironment :: SetAlphaSource( unsigned int p_uiIndex, 
										   eCOMBINATION_SOURCE p_source, 
										   int p_textureIndex)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_ptAlphaCombinationSources[p_uiIndex] = p_source;
	m_iAlphaTextureSourceIndex = p_textureIndex;
}

void TextureEnvironment :: SetRgbOperand( unsigned int p_uiIndex, 
										  eRGB_OPERAND p_operand)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_ptRgbOperands[p_uiIndex] = p_operand;
}

void TextureEnvironment :: SetAlphaOperand( unsigned int p_uiIndex, 
										    eALPHA_OPERAND p_operand)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_ptAlphaOperands[p_uiIndex] = p_operand;
}

TextureEnvironment::eCOMBINATION_SOURCE TextureEnvironment :: GetRgbSource( unsigned int p_uiIndex, 
														int & p_textureIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eCOMBINATION_SOURCE( m_ptRgbCombinationSources[p_uiIndex]);
}

TextureEnvironment::eCOMBINATION_SOURCE TextureEnvironment :: GetAlphaSource( unsigned int p_uiIndex, 
													    int & p_textureIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eCOMBINATION_SOURCE( m_ptAlphaCombinationSources[p_uiIndex]);
}

TextureEnvironment::eRGB_OPERAND TextureEnvironment :: GetRgbOperand( unsigned int p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eRGB_OPERAND( m_ptRgbOperands[p_uiIndex]);
}


TextureEnvironment::eALPHA_OPERAND TextureEnvironment :: GetAlphaOperand( unsigned int p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eALPHA_OPERAND( m_ptAlphaOperands[p_uiIndex]);
}

bool TextureEnvironment :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( "\t\t\tblend_mode " + StringMode[m_eMode]) > 0;

	if (l_bReturn)
	{
		if (m_bCombineRgb && m_eMode == eModeCombine && m_eRgbCombination != eRgbCombiNone)
		{
			String l_strLine = "\t\t\trgb_combine " + StringRgbCombinations[m_eRgbCombination] + " " + StringRgbOperands[m_ptRgbOperands[0]] + " " + StringCombinationSources[m_ptRgbCombinationSources[0]];

			if (m_ptRgbCombinationSources[0] == eCombiSrcTexture)
			{
				l_strLine << " " << m_iRgbTextureSourceIndex;
			}

			if (m_eRgbCombination != eRgbCombiReplace)
			{
				l_strLine << " " << StringRgbOperands[m_ptRgbOperands[1]] + " " + StringCombinationSources[m_ptRgbCombinationSources[1]];

				if (m_ptRgbCombinationSources[1] == eCombiSrcTexture)
				{
					l_strLine << " " << m_iRgbTextureSourceIndex;
				}

				if (m_eRgbCombination == eRgbCombiInterpolate)
				{
					l_strLine << " " << StringRgbOperands[m_ptRgbOperands[2]] + " " + StringCombinationSources[m_ptRgbCombinationSources[2]];

					if (m_ptRgbCombinationSources[1] == eCombiSrcTexture)
					{
						l_strLine << " " << m_iRgbTextureSourceIndex;
					}
				}
			}

			l_bReturn = p_file.WriteLine( l_strLine + "\n") > 0;
		}
	}

	if (l_bReturn)
	{
		if (m_bCombineAlpha && m_eMode == eModeCombine && m_eAlphaCombination != eAlphaCombiNone)
		{
			String l_strLine = "\t\t\talpha_combine " + StringAlphaCombinations[m_eAlphaCombination] + " " + StringAlphaOperands[m_ptAlphaOperands[0]] + " " + StringCombinationSources[m_ptAlphaCombinationSources[0]];

			if (m_ptAlphaCombinationSources[0] == eCombiSrcTexture)
			{
				l_strLine << " " << m_iAlphaTextureSourceIndex;
			}

			if (m_eAlphaCombination != eAlphaCombiReplace)
			{
				l_strLine << " " << StringAlphaOperands[m_ptAlphaOperands[1]] + " " + StringCombinationSources[m_ptAlphaCombinationSources[1]];

				if (m_ptAlphaCombinationSources[1] == eCombiSrcTexture)
				{
					l_strLine << " " << m_iAlphaTextureSourceIndex;
				}

				if (m_eAlphaCombination == eAlphaCombiInterpolate)
				{
					l_strLine << " " << StringAlphaOperands[m_ptAlphaOperands[2]] + " " + StringCombinationSources[m_ptAlphaCombinationSources[2]];

					if (m_ptAlphaCombinationSources[1] == eCombiSrcTexture)
					{
						l_strLine << " " << m_iAlphaTextureSourceIndex;
					}
				}
			}

			l_bReturn = p_file.WriteLine( l_strLine + "\n") > 0;
		}
	}

	return l_bReturn;
}

bool TextureEnvironment :: Save( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_eMode) == sizeof( eMODE));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_eRgbCombination) == sizeof( eRGB_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_eAlphaCombination) == sizeof( eALPHA_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptRgbCombinationSources.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_iRgbTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptAlphaCombinationSources.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_iAlphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptRgbOperands.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptAlphaOperands.Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_rRgbScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_rAlphaScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_bCombineRgb) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_bCombineAlpha) == sizeof( bool));
	}

	return l_bReturn;
}

bool TextureEnvironment :: Load( Castor::Utils::File & p_file)
{
	bool l_bReturn = (p_file.Read( m_eMode) == sizeof( eMODE));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_eRgbCombination) == sizeof( eRGB_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_eAlphaCombination) == sizeof( eALPHA_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptRgbCombinationSources.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_iRgbTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptAlphaCombinationSources.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_iAlphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptRgbOperands.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_ptAlphaOperands.Load( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_rRgbScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_rAlphaScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_bCombineRgb) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_bCombineAlpha) == sizeof( bool));
	}

	return l_bReturn;
}

void TextureEnvironment :: _initialiseMaps()
{
	MapModes.clear();
	MapRgbCombinations.clear();
	MapRgbOperands.clear();
	MapAlphaCombinations.clear();
	MapAlphaOperands.clear();
	MapCombinationSources.clear();

	MapModes["replace"]							= eModeReplace;
	MapModes["modulate"]						= eModeModulate;
	MapModes["blend"]							= eModeBlend;
	MapModes["add"]								= eModeAdd;
	MapModes["combine"]							= eModeCombine;

	MapRgbCombinations["none"]					= eRgbCombiNone;
	MapRgbCombinations["replace"]				= eRgbCombiReplace;
	MapRgbCombinations["modulate"]				= eRgbCombiModulate;
	MapRgbCombinations["add"]					= eRgbCombiAdd;
	MapRgbCombinations["add_signed"]			= eRgbCombiAddSigned;
	MapRgbCombinations["dot3_rgb"]				= eRgbCombiDot3Rgb;
	MapRgbCombinations["dot3_alpha"]			= eRgbCombiDot3Rgba;
	MapRgbCombinations["interpolate"]			= eRgbCombiInterpolate;

	MapRgbOperands["src_colour"]				= eRgbOpeSrcColour;
	MapRgbOperands["one_minus_src_colour"]		= eRgbOpeOneMinusSrcColour;
	MapRgbOperands["src_alpha"]					= eRgbOpeSrcAlpha;
	MapRgbOperands["one_minus_src_alpha"]		= eRgbOpeOneMinusSrcAlpha;

	MapAlphaCombinations["none"]				= eAlphaCombiNone;
	MapAlphaCombinations["replace"]				= eAlphaCombiReplace;
	MapAlphaCombinations["modulate"]			= eAlphaCombiModulate;
	MapAlphaCombinations["add"]					= eAlphaCombiAdd;
	MapAlphaCombinations["add_signed"]			= eAlphaCombiAddSigned;
	MapAlphaCombinations["interpolate"]			= eAlphaCombiInterpolate;

	MapAlphaOperands["src_alpha"]				= eAlphaOpeSrcAlpha;
	MapAlphaOperands["one_minus_src_alpha"]		= eAlphaOpeOneMinusSrcAlpha;

	MapCombinationSources["texture"]			= eCombiSrcTexture;
	MapCombinationSources["current_texture"]	= eCombiSrcCurrentTexture;
	MapCombinationSources["constant"]			= eCombiSrcConstant;
	MapCombinationSources["primary_colour"]		= eCombiSrcPrimaryColour;
	MapCombinationSources["previous"]			= eCombiSrcPrevious;
}