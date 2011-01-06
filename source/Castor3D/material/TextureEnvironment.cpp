#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/TextureEnvironment.h"

using namespace Castor3D;

TextureEnvironment :: TextureEnvironment()
	:	m_mode						( eMModulate)
	,	m_RGBCombination			( eCCNone)
	,	m_alphaCombination			( eACNone)
	,	m_RGBTextureSourceIndex		( 0)
	,	m_alphaTextureSourceIndex	( 0)
	,	m_RGBScale					( 1.0)
	,	m_alphaScale				( 1.0)
	,	m_RGBCombinationSources		( eSrcCurrentTexture, eSrcPrevious, eSrcConstant)
	,	m_alphaCombinationSources	( eSrcCurrentTexture, eSrcPrevious, eSrcConstant)
	,	m_RGBOperands				( eCOSrcColour, eCOSrcColour, eCOSrcAlpha)
	,	m_alphaOperands				( eAOSrcAlpha, eAOSrcAlpha, eAOSrcAlpha)
{
}

TextureEnvironment :: TextureEnvironment( const TextureEnvironment & p_copy)
	:	m_mode						( p_copy.m_mode)
	,	m_RGBCombination			( p_copy.m_RGBCombination)
	,	m_alphaCombination			( p_copy.m_alphaCombination)
	,	m_RGBTextureSourceIndex		( p_copy.m_RGBTextureSourceIndex)
	,	m_alphaTextureSourceIndex	( p_copy.m_alphaTextureSourceIndex)
	,	m_RGBScale					( p_copy.m_RGBScale)
	,	m_alphaScale				( p_copy.m_alphaScale)
	,	m_RGBCombinationSources		( p_copy.m_RGBCombinationSources)
	,	m_alphaCombinationSources	( p_copy.m_alphaCombinationSources)
	,	m_RGBOperands				( p_copy.m_RGBOperands)
	,	m_alphaOperands				( p_copy.m_alphaOperands)
{
}

TextureEnvironment & TextureEnvironment :: operator =( const TextureEnvironment & p_copy)
{
	m_mode						= p_copy.m_mode;
	m_RGBCombination			= p_copy.m_RGBCombination;
	m_alphaCombination			= p_copy.m_alphaCombination;
	m_RGBTextureSourceIndex		= p_copy.m_RGBTextureSourceIndex;
	m_alphaTextureSourceIndex	= p_copy.m_alphaTextureSourceIndex;
	m_RGBScale					= p_copy.m_RGBScale;
	m_alphaScale				= p_copy.m_alphaScale;
	m_RGBCombinationSources		= p_copy.m_RGBCombinationSources;
	m_alphaCombinationSources	= p_copy.m_alphaCombinationSources;
	m_RGBOperands				= p_copy.m_RGBOperands;
	m_alphaOperands				= p_copy.m_alphaOperands;

	return * this;
}

TextureEnvironment :: ~TextureEnvironment()
{
}

void TextureEnvironment :: Render( eDRAW_TYPE p_displayMode)
{
	m_pRenderer->Render();
}

void TextureEnvironment :: SetRGBSource( unsigned int p_uiIndex, 
										 eCOMBINATION_SOURCE p_source, 
										 int p_textureIndex)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_RGBCombinationSources[p_uiIndex] = p_source;
	m_RGBTextureSourceIndex = p_textureIndex;
}

void TextureEnvironment :: SetAlphaSource( unsigned int p_uiIndex, 
										   eCOMBINATION_SOURCE p_source, 
										   int p_textureIndex)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_alphaCombinationSources[p_uiIndex] = p_source;
	m_alphaTextureSourceIndex = p_textureIndex;
}

void TextureEnvironment :: SetRGBOperand( unsigned int p_uiIndex, 
										  eRGB_OPERAND p_operand)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_RGBOperands[p_uiIndex] = p_operand;
}

void TextureEnvironment :: SetAlphaOperand( unsigned int p_uiIndex, 
										    eALPHA_OPERAND p_operand)
{
	CASTOR_ASSERT( p_uiIndex < 3);
	m_alphaOperands[p_uiIndex] = p_operand;
}

TextureEnvironment::eCOMBINATION_SOURCE TextureEnvironment :: GetRGBSource( unsigned int p_uiIndex, 
														int & p_textureIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eCOMBINATION_SOURCE( m_RGBCombinationSources[p_uiIndex]);
}

TextureEnvironment::eCOMBINATION_SOURCE TextureEnvironment :: GetAlphaSource( unsigned int p_uiIndex, 
													    int & p_textureIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eCOMBINATION_SOURCE( m_alphaCombinationSources[p_uiIndex]);
}

TextureEnvironment::eRGB_OPERAND TextureEnvironment :: GetRGBOperand( unsigned int p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eRGB_OPERAND( m_RGBOperands[p_uiIndex]);
}


TextureEnvironment::eALPHA_OPERAND TextureEnvironment :: GetAlphaOperand( unsigned int p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < 3);
	return eALPHA_OPERAND( m_alphaOperands[p_uiIndex]);
}

bool TextureEnvironment :: Write( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_mode) == sizeof( eMODE));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_RGBCombination) == sizeof( eRGB_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_alphaCombination) == sizeof( eALPHA_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = m_RGBCombinationSources.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_RGBTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_alphaCombinationSources.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_alphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_RGBOperands.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_alphaOperands.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_RGBScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_alphaScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_combineRGB) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_combineAlpha) == sizeof( bool));
	}

	return l_bReturn;
}

bool TextureEnvironment :: Read( Castor::Utils::File & p_file)
{
	bool l_bReturn = (p_file.Read( m_mode) == sizeof( eMODE));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_RGBCombination) == sizeof( eRGB_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_alphaCombination) == sizeof( eALPHA_COMBINATION));
	}

	if (l_bReturn)
	{
		l_bReturn = m_RGBCombinationSources.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_RGBTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_alphaCombinationSources.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_alphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = m_RGBOperands.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_alphaOperands.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_RGBScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_alphaScale) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_combineRGB) == sizeof( bool));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_combineAlpha) == sizeof( bool));
	}

	return l_bReturn;
}