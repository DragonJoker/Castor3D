#include "PrecompiledHeader.h"
#include "material/TextureEnvironment.h"

using namespace Castor3D;

TextureEnvironment :: TextureEnvironment()
	:	m_mode( EMModulate),
		m_RGBCombination( CCNone),
		m_alphaCombination( ACNone),
		m_RGBTextureSourceIndex( 0),
		m_alphaTextureSourceIndex( 0),
		m_RGBScale( 1.0),
		m_alphaScale( 1.0)
{
	m_RGBCombinationSources[0] = CSCurrentTexture;
	m_alphaCombinationSources[0] = CSCurrentTexture;
	m_RGBOperands[0] = COSrcColour;
	m_alphaOperands[0] = AOSrcAlpha;

	m_RGBCombinationSources[1] = CSPrevious;
	m_alphaCombinationSources[1] = CSPrevious;
	m_RGBOperands[1] = COSrcColour;
	m_alphaOperands[1] = AOSrcAlpha;

	m_RGBCombinationSources[2] = CSConstant;
	m_alphaCombinationSources[2] = CSConstant;
	m_RGBOperands[2] = COSrcAlpha;
	m_alphaOperands[2] = AOSrcAlpha;
}

TextureEnvironment :: ~TextureEnvironment()
{
}

void TextureEnvironment :: Render( eDRAW_TYPE p_displayMode)
{
	m_pRenderer->Render();
}

void TextureEnvironment :: SetRGBSource( unsigned int p_index, 
										 CombinationSource p_source, 
										 int p_textureIndex)
{
	if (p_index <= 2)
	{
		m_RGBCombinationSources[p_index] = p_source;
		m_RGBTextureSourceIndex = p_textureIndex;
	}
}

void TextureEnvironment :: SetAlphaSource( unsigned int p_index, 
										   CombinationSource p_source, 
										   int p_textureIndex)
{
	if (p_index <= 2)
	{
		m_alphaCombinationSources[p_index] = p_source;
		m_alphaTextureSourceIndex = p_textureIndex;
	}
}

void TextureEnvironment :: SetRGBOperand( unsigned int p_index, 
										  RGBOperand p_operand)
{
	if (p_index <= 2)
	{
		m_RGBOperands[p_index] = p_operand;
	}
}

void TextureEnvironment :: SetAlphaOperand( unsigned int p_index, 
										    AlphaOperand p_operand)
{
	if (p_index <= 2)
	{
		m_alphaOperands[p_index] = p_operand;
	}
}

CombinationSource TextureEnvironment :: GetRGBSource( unsigned int p_index, 
													  int & p_textureIndex)const
{
	if (p_index >= 2)
	{
		p_textureIndex = 0;
		return CSCurrentTexture;
	}

	return m_RGBCombinationSources[p_index];
}

CombinationSource TextureEnvironment :: GetAlphaSource( unsigned int p_index, 
													    int & p_textureIndex)const
{
	if (p_index >= 2)
	{
		p_textureIndex = 0;
		return CSCurrentTexture;
	}

	return m_alphaCombinationSources[p_index];
}

RGBOperand TextureEnvironment :: GetRGBOperand( unsigned int p_index)const
{
	if (p_index >= 2)
	{
		return COSrcColour;
	}

	return m_RGBOperands[p_index];
}


AlphaOperand TextureEnvironment :: GetAlphaOperand( unsigned int p_index)const
{
	if (p_index >= 2)
	{
		return AOSrcAlpha;
	}

	return m_alphaOperands[p_index];
}

bool TextureEnvironment :: Write( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_mode) == sizeof( EnvironmentMode));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_RGBCombination) == sizeof( RGBCombination));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_alphaCombination) == sizeof( AlphaCombination));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray( m_RGBCombinationSources, 3) == sizeof( CombinationSource) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_RGBTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray( m_alphaCombinationSources, 3) == sizeof( CombinationSource) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( m_alphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray( m_RGBOperands, 3) == sizeof( RGBOperand) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray( m_alphaOperands, 3) == sizeof( AlphaOperand) * 3);
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
	bool l_bReturn = (p_file.Read( m_mode) == sizeof( EnvironmentMode));

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_RGBCombination) == sizeof( RGBCombination));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_alphaCombination) == sizeof( AlphaCombination));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray( m_RGBCombinationSources, 3) == sizeof( CombinationSource) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_RGBTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray( m_alphaCombinationSources, 3) == sizeof( CombinationSource) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_alphaTextureSourceIndex) == sizeof( int));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray( m_RGBOperands, 3) == sizeof( RGBOperand) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray( m_alphaOperands, 3) == sizeof( AlphaOperand) * 3);
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