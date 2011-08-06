#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;

//*********************************************************************************************

bool Loader<TextureUnit> :: Load( TextureUnit & p_unit, File & p_file)
{
	p_unit.Cleanup();
	bool l_bTmp;
	float l_fTmp;
	bool l_bReturn = (p_file.Read( l_bTmp) == sizeof( bool));

	if (l_bReturn && l_bTmp)
	{
		eTEXTURE_TYPE l_eDimension;
		l_bReturn = (p_file.Read( l_eDimension) == sizeof( eTEXTURE_TYPE));

		if (l_bReturn)
		{
			l_bReturn = Loader<Colour>::Load( p_unit.GetPrimaryColour(), p_file);
		}

		if (l_bReturn)
		{
			TextureUnit::eMAP_MODE l_eMapMode;
			l_bReturn = (p_file.Read( l_eMapMode) == sizeof( TextureUnit::eMAP_MODE));
			p_unit.SetTextureMapMode( l_eMapMode);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Read( l_fTmp) == sizeof( float));
			p_unit.SetAlphaValue( l_fTmp);
		}

		if (l_bReturn)
		{
			TextureUnit::eALPHA_FUNC l_eAlphaFunc;
			l_bReturn = (p_file.Read( l_eAlphaFunc) == sizeof( TextureUnit::eALPHA_FUNC));
			p_unit.SetAlphaFunc( l_eAlphaFunc);
		}

		if (l_bReturn)
		{
			TextureUnit::eWRAP_MODE l_eWrapModes[TextureUnit::eUVW_COUNT];
			l_bReturn = (p_file.ReadArray( l_eWrapModes, TextureUnit::eUVW_COUNT) == sizeof( l_eWrapModes));

			for (int i = TextureUnit::eUVW_U ; i < TextureUnit::eUVW_COUNT ; i++)
			{
				p_unit.SetWrapMode( TextureUnit::eUVW( i), l_eWrapModes[i]);
			}
		}

		if (l_bReturn)
		{
			TextureUnit::eINTERPOLATION_MODE l_eInterpolationModes[TextureUnit::eINTERPOLATION_FILTER_COUNT];
			l_bReturn = (p_file.ReadArray( l_eInterpolationModes, TextureUnit::eINTERPOLATION_FILTER_COUNT) == sizeof( l_eInterpolationModes));

			for (int i = TextureUnit::eINTERPOLATION_FILTER_MIN ; i < TextureUnit::eINTERPOLATION_FILTER_COUNT ; i++)
			{
				p_unit.SetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER( i), l_eInterpolationModes[i]);
			}
		}

		if (l_bReturn)
		{
			TextureUnit::eBLEND_SOURCE l_eRgbArguments[TextureUnit::eBLEND_SRC_INDEX_COUNT];
			l_bReturn = (p_file.ReadArray( l_eRgbArguments, TextureUnit::eBLEND_SRC_INDEX_COUNT) == sizeof( l_eRgbArguments));

			for (int i = TextureUnit::eBLEND_SRC_INDEX_0 ; i < TextureUnit::eBLEND_SRC_INDEX_COUNT ; i++)
			{
				p_unit.SetRgbArgument( TextureUnit::eBLEND_SRC_INDEX( i), l_eRgbArguments[i]);
			}
		}

		if (l_bReturn)
		{
			TextureUnit::eBLEND_SOURCE l_eAlpArguments[TextureUnit::eBLEND_SRC_INDEX_COUNT];
			l_bReturn = (p_file.ReadArray( l_eAlpArguments, TextureUnit::eBLEND_SRC_INDEX_COUNT) == sizeof( l_eAlpArguments));

			for (int i = TextureUnit::eBLEND_SRC_INDEX_0 ; i < TextureUnit::eBLEND_SRC_INDEX_COUNT ; i++)
			{
				p_unit.SetAlpArgument( TextureUnit::eBLEND_SRC_INDEX( i), l_eAlpArguments[i]);
			}
		}

		if (l_bReturn)
		{
			TextureUnit::eRGB_BLEND_FUNC l_eMode;
			l_bReturn = (p_file.Read( l_eMode) == sizeof( TextureUnit::eRGB_BLEND_FUNC));
			p_unit.SetRgbMode( l_eMode);
		}

		if (l_bReturn)
		{
			TextureUnit::eALPHA_BLEND_FUNC l_eMode;
			l_bReturn = (p_file.Read( l_eMode) == sizeof( TextureUnit::eRGB_BLEND_FUNC));
			p_unit.SetAlpMode( l_eMode);
		}

		Path l_pathName;

		if (l_bReturn)
		{
			String l_strPath( l_pathName);
			l_bReturn = p_file.Read( l_strPath);
			l_pathName = l_strPath;
		}

		if (l_bReturn)
		{
			Collection<Image, String> l_imgCollection;
			ImagePtr l_pImage = l_imgCollection.GetElement( l_pathName);
			if ( ! l_pImage)
			{
				l_pImage = ImagePtr( new Image( l_pathName));
				l_imgCollection.AddElement( l_pathName, l_pImage);
			}
			l_bReturn = Loader<Image>::Load( * l_pImage, l_pathName);
			p_unit.SetTexture( l_eDimension, l_pImage);
		}
	}

	return l_bReturn;
}

bool Loader<TextureUnit> :: Save( const TextureUnit & p_unit, File & p_file)
{
	bool l_bReturn = (p_file.Write( p_unit.IsTextured()) == sizeof( bool));

	if (l_bReturn && p_unit.IsTextured())
	{
		l_bReturn = (p_file.Write( p_unit.GetDimension()) == sizeof( eTEXTURE_TYPE));

		if (l_bReturn)
		{
			l_bReturn = Loader<Colour>::Save( p_unit.GetPrimaryColour(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( p_unit.GetTextureMapMode()) == sizeof( TextureUnit::eMAP_MODE));
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( p_unit.GetAlphaValue()) == sizeof( float));
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( p_unit.GetAlphaFunc()) == sizeof( TextureUnit::eALPHA_FUNC));
		}

		if (l_bReturn)
		{
			for (int i = TextureUnit::eUVW_U ; i < TextureUnit::eUVW_COUNT && l_bReturn ; i++)
			{
				l_bReturn = (p_file.Write( p_unit.GetWrapMode( TextureUnit::eUVW( i))) == sizeof( TextureUnit::eWRAP_MODE));
			}
		}

		if (l_bReturn)
		{
			for (int i = TextureUnit::eINTERPOLATION_FILTER_MIN ; i < TextureUnit::eINTERPOLATION_FILTER_COUNT && l_bReturn ; i++)
			{
				l_bReturn = (p_file.Write( p_unit.GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER( i))) == sizeof( TextureUnit::eINTERPOLATION_MODE));
			}
		}

		if (l_bReturn)
		{
			for (int i = TextureUnit::eBLEND_SRC_INDEX_0 ; i < TextureUnit::eBLEND_SRC_INDEX_COUNT && l_bReturn ; i++)
			{
				l_bReturn = (p_file.Write( p_unit.GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX( i))) == sizeof( TextureUnit::eBLEND_SOURCE));
			}
		}

		if (l_bReturn)
		{
			for (int i = TextureUnit::eBLEND_SRC_INDEX_0 ; i < TextureUnit::eBLEND_SRC_INDEX_COUNT && l_bReturn ; i++)
			{
				l_bReturn = (p_file.Write( p_unit.GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX( i))) == sizeof( TextureUnit::eBLEND_SOURCE));
			}
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( p_unit.GetRgbFunction()) == sizeof( TextureUnit::eRGB_BLEND_FUNC));
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( p_unit.GetAlpFunction()) == sizeof( TextureUnit::eRGB_BLEND_FUNC));
		}

		String l_strName;

		if (l_bReturn)
		{
			l_bReturn = p_file.Write( String( p_unit.GetImage()->GetPath()));
		}
	}

	return l_bReturn;
}

bool Loader<TextureUnit> :: Write( const TextureUnit & p_unit, File & p_file)
{
	bool l_bReturn = true;

	if (p_unit.IsTextured())
	{
		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( cuT( "\t\ttexture_unit\n\t\t{\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\t\tcolour ")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = Loader<Colour>::Write( p_unit.GetPrimaryColour(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\n\t\t\tmap_type %i\n"), p_unit.GetTextureMapMode()) > 0;
		}

		if (l_bReturn && p_unit.GetImage())
		{
			Path l_path = p_unit.GetImage()->GetPath();
			l_bReturn = p_file.WriteLine( cuT( "\t\t\timage ") + l_path + cuT( "\n")) > 0;
		}

		if (l_bReturn)
		{
			String l_strLine;
			l_strLine << TextureUnit::StringAlphaFuncs[p_unit.GetAlphaFunc()] << cuT( " ") << p_unit.GetAlphaValue();
			l_bReturn = p_file.WriteLine( cuT( "\t\t\talpha_func ") + l_strLine + cuT( "\n")) > 0;
		}

		if (l_bReturn && p_unit.GetRgbFunction() != TextureUnit::eRGB_BLEND_FUNC_NONE)
		{
			String l_strLine;
			l_strLine << TextureUnit::StringTextureRgbFunctions[p_unit.GetRgbFunction()] << cuT( " ") << TextureUnit::StringTextureArguments[p_unit.GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_0)] << cuT( " ") << TextureUnit::StringTextureArguments[p_unit.GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_1)];
			l_bReturn = p_file.WriteLine( cuT( "\t\t\trgb_blend ") + l_strLine + cuT( "\n")) > 0;
		}

		if (l_bReturn && p_unit.GetAlpFunction() != TextureUnit::eALPHA_BLEND_FUNC_NONE)
		{
			String l_strLine;
			l_strLine << TextureUnit::StringTextureAlphaFunctions[p_unit.GetAlpFunction()] << cuT( " ") << TextureUnit::StringTextureArguments[p_unit.GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_0)] << cuT( " ") << TextureUnit::StringTextureArguments[p_unit.GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_1)];
			l_bReturn = p_file.WriteLine( cuT( "\t\t\talpha_blend ") + l_strLine + cuT( "\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( cuT( "\t\t}\n")) > 0;
		}
	}

	return l_bReturn;
}

//*********************************************************************************************

std::map<String, eTEXTURE_TYPE> TextureUnit::MapTypes;
const String TextureUnit::StringTypes[eTEXTURE_TYPE_COUNT]								= { cuT( "1d"),			cuT( "2d"),			cuT( "3d") };

std::map<String, TextureUnit::eMAP_MODE> TextureUnit::MapMapModes;
const String TextureUnit::StringMapModes[eMAP_MODE_COUNT]						= { cuT( "none"),		cuT( "reflexion"),	cuT( "sphere") };

std::map<String, TextureUnit::eALPHA_FUNC> TextureUnit::MapAlphaFuncs;
const String TextureUnit::StringAlphaFuncs[eALPHA_FUNC_COUNT]					= { cuT( "always"),		cuT( "less"),		cuT( "less_or_equal"),	cuT( "equal"),		cuT( "not_equal"),	cuT( "greater_or_equal"),	cuT( "greater"),	cuT( "never") };

std::map<String, TextureUnit::eBLEND_SOURCE> TextureUnit::MapTextureArguments;
const String TextureUnit::StringTextureArguments[eBLEND_SOURCE_COUNT]			= { cuT( "diffuse"),	cuT( "texture"),	cuT( "previous"),		cuT( "constant") };

std::map<String, TextureUnit::eRGB_BLEND_FUNC> TextureUnit::MapTextureRgbFunctions;
const String TextureUnit::StringTextureRgbFunctions[eRGB_BLEND_FUNC_COUNT]		= { cuT( "none"),		cuT( "first_arg"),	cuT( "add"),			cuT( "add_signed"),	cuT( "modulate"),	cuT( "interpolate"),		cuT( "substract"),	cuT( "dot3_rgb"),	cuT( "dot3_rgba") };

std::map<String, TextureUnit::eALPHA_BLEND_FUNC> TextureUnit::MapTextureAlphaFunctions;
const String TextureUnit::StringTextureAlphaFunctions[eALPHA_BLEND_FUNC_COUNT]	= { cuT( "none"),		cuT( "first_arg"),	cuT( "add"),			cuT( "add_signed"),	cuT( "modulate"),	cuT( "interpolate"),		cuT( "substract") };

TextureUnit :: TextureUnit()
	:	m_uiIndex				( 0)
	,	m_bTextureInitialised	( false)
	,	m_bTextured				( false)
	,	m_eMode					( eMAP_MODE_NONE)
	,	m_clrPrimaryColour		( Colour::FromRGBA( 0xFFFFFFFF))
	,	m_eDimension			( eTEXTURE_TYPE_2D)
	,	m_eAlphaFunc			( eALPHA_FUNC_ALWAYS)
	,	m_fAlphaValue			( 0)
	,	m_eRgbFunction			( eRGB_BLEND_FUNC_NONE)
	,	m_eAlpFunction			( eALPHA_BLEND_FUNC_NONE)
{
	m_eRgbArguments[0] = eBLEND_SOURCE_COUNT;
	m_eRgbArguments[1] = eBLEND_SOURCE_COUNT;
	m_eAlpArguments[0] = eBLEND_SOURCE_COUNT;
	m_eAlpArguments[1] = eBLEND_SOURCE_COUNT;
	m_eWrapModes[0] = eWRAP_MODE_REPEAT;
	m_eWrapModes[1] = eWRAP_MODE_REPEAT;
	m_eWrapModes[2] = eWRAP_MODE_REPEAT;
	m_eInterpolationModes[0] = eINTERPOLATION_MODE_ANISOTROPIC;
	m_eInterpolationModes[1] = eINTERPOLATION_MODE_ANISOTROPIC;

	_initialiseMaps();
}

TextureUnit :: TextureUnit( const TextureUnit & p_copy)
	:	m_uiIndex				( p_copy.m_uiIndex)
	,	m_pImage				( p_copy.m_pImage)
	,	m_eDimension			( p_copy.m_eDimension)
	,	m_bTextureInitialised	( p_copy.m_bTextureInitialised)
	,	m_bTextured				( p_copy.m_bTextured)
	,	m_clrPrimaryColour		( p_copy.m_clrPrimaryColour)
	,	m_eMode					( p_copy.m_eMode)
	,	m_eAlphaFunc			( p_copy.m_eAlphaFunc)
	,	m_fAlphaValue			( p_copy.m_fAlphaValue)
	,	m_eRgbFunction			( p_copy.m_eRgbFunction)
	,	m_eAlpFunction			( p_copy.m_eAlpFunction)
{
	m_eRgbArguments[0] = m_eRgbArguments[0];
	m_eRgbArguments[1] = m_eRgbArguments[1];
	m_eAlpArguments[0] = m_eAlpArguments[0];
	m_eAlpArguments[1] = m_eAlpArguments[1];
	m_eWrapModes[0] = p_copy.m_eWrapModes[0];
	m_eWrapModes[1] = p_copy.m_eWrapModes[1];
	m_eWrapModes[2] = p_copy.m_eWrapModes[2];
	_initialiseMaps();
}

TextureUnit & TextureUnit :: operator =( const TextureUnit & p_copy)
{
	m_uiIndex				= p_copy.m_uiIndex;
	m_pImage				= p_copy.m_pImage;
	m_eDimension			= p_copy.m_eDimension;
	m_bTextureInitialised	= p_copy.m_bTextureInitialised;
	m_bTextured				= p_copy.m_bTextured;
	m_clrPrimaryColour		= p_copy.m_clrPrimaryColour;
	m_eMode					= p_copy.m_eMode;
	m_eAlphaFunc			= p_copy.m_eAlphaFunc;
	m_fAlphaValue			= p_copy.m_fAlphaValue;
	m_eWrapModes[0] 		= p_copy.m_eWrapModes[0];
	m_eWrapModes[1] 		= p_copy.m_eWrapModes[1];
	m_eWrapModes[2] 		= p_copy.m_eWrapModes[2];

	return * this;
}

TextureUnit :: ~TextureUnit()
{
}

void TextureUnit :: SetTexture( eTEXTURE_TYPE p_eDimension, ImagePtr p_pImage)
{
	if (p_pImage)
	{
		if (m_pImage)
		{
			m_pImage.reset();
		}

		m_pImage = p_pImage;
		m_bTextureInitialised = false;
		m_bTextured = true;
		m_eDimension = p_eDimension;
	}
}

void TextureUnit :: Initialise()
{
	CASTOR_TRACK;
	if (m_bTextured)
	{
		if (m_eDimension == eTEXTURE_TYPE_2D &&  ! m_bTextureInitialised)
		{
			m_pRenderer->Initialise();
		}

		m_bTextureInitialised = true;
	}
}

void TextureUnit :: Cleanup()
{
	CASTOR_TRACK;
	m_pImage.reset();
	m_bTextureInitialised = false;
	m_bTextured = false;
	m_clrPrimaryColour = Colour();
	m_fAlphaValue = 1.0f;
}

void TextureUnit :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->Render();
//		m_environment->Render( p_displayMode);
	}
}

void TextureUnit :: EndRender()
{
	CASTOR_TRACK;
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->EndRender();
	}
}

unsigned char const * TextureUnit :: GetImagePixels()const
{
	unsigned char const * l_pReturn = nullptr;

	if (m_bTextured && m_pImage)
	{
		l_pReturn = m_pImage->GetBuffer();
	}

	return nullptr;
}

unsigned char * TextureUnit :: GetImagePixels()
{
	unsigned char * l_pReturn = nullptr;

	if (m_bTextured && m_pImage)
	{
		l_pReturn = m_pImage->GetBuffer();
	}

	return l_pReturn;
}

size_t TextureUnit :: GetImageSize()const
{
	size_t l_uiReturn = 0;

	if (m_bTextured && m_pImage)
	{
		l_uiReturn = m_pImage->GetSize();
	}

	return l_uiReturn;
}

unsigned int TextureUnit :: GetWidth()const
{
	unsigned int l_uiReturn = 0;

	if (m_bTextured && m_pImage)
	{
		l_uiReturn = m_pImage->GetWidth();
	}

	return l_uiReturn;
}

unsigned int TextureUnit :: GetHeight()const
{
	unsigned int l_uiReturn = 0;

	if (m_bTextured && m_pImage)
	{
		l_uiReturn = m_pImage->GetHeight();
	}

	return l_uiReturn;
}

String TextureUnit :: GetTexturePath()const
{
	String l_strReturn;

	if (m_bTextured && m_pImage)
	{
		l_strReturn = m_pImage->GetPath();
	}

	return l_strReturn;
}

ePIXEL_FORMAT TextureUnit :: GetPixelFormat()
{
	ePIXEL_FORMAT l_pfReturn;

	if (m_bTextured && m_pImage)
	{
		l_pfReturn = m_pImage->GetPixelFormat();
	}

	return l_pfReturn;
}

void TextureUnit :: _initialiseMaps()
{
	MapTypes.clear();
	MapTypes["1d"]							= eTEXTURE_TYPE_1D;
	MapTypes["2d"]							= eTEXTURE_TYPE_2D;
	MapTypes["3d"]							= eTEXTURE_TYPE_3D;

	MapMapModes.clear();
	MapMapModes["none"]						= eMAP_MODE_NONE;
	MapMapModes["reflexion"]				= eMAP_MODE_REFLEXION;
	MapMapModes["sphere"]					= eMAP_MODE_SPHERE;

	MapAlphaFuncs.clear();
	MapAlphaFuncs["always"]					= eALPHA_FUNC_ALWAYS;
	MapAlphaFuncs["less"]					= eALPHA_FUNC_LESS;
	MapAlphaFuncs["less_or_equal"]			= eALPHA_FUNC_LESS_OR_EQUAL;
	MapAlphaFuncs["equal"]					= eALPHA_FUNC_EQUAL;
	MapAlphaFuncs["not_equal"]				= eALPHA_FUNC_NOT_EQUAL;
	MapAlphaFuncs["greater_or_equal"]		= eALPHA_FUNC_GREATER_OR_EQUAL;
	MapAlphaFuncs["greater"]				= eALPHA_FUNC_GREATER;
	MapAlphaFuncs["never"]					= eALPHA_FUNC_NEVER;

	MapTextureArguments.clear();
	MapTextureArguments["texture"]			= eBLEND_SOURCE_TEXTURE;
	MapTextureArguments["texture0"]			= eBLEND_SOURCE_TEXTURE0;
	MapTextureArguments["texture1"]			= eBLEND_SOURCE_TEXTURE1;
	MapTextureArguments["texture2"]			= eBLEND_SOURCE_TEXTURE2;
	MapTextureArguments["texture3"]			= eBLEND_SOURCE_TEXTURE3;
	MapTextureArguments["constant"]			= eBLEND_SOURCE_CONSTANT;
	MapTextureArguments["diffuse"]			= eBLEND_SOURCE_DIFFUSE;
	MapTextureArguments["previous"]			= eBLEND_SOURCE_PREVIOUS;

	MapTextureRgbFunctions.clear();
	MapTextureRgbFunctions["none"]			= eRGB_BLEND_FUNC_NONE;
	MapTextureRgbFunctions["first_arg"]		= eRGB_BLEND_FUNC_FIRST_ARG;
	MapTextureRgbFunctions["add"]			= eRGB_BLEND_FUNC_ADD;
	MapTextureRgbFunctions["add_signed"]	= eRGB_BLEND_FUNC_ADD_SIGNED;
	MapTextureRgbFunctions["modulate"]		= eRGB_BLEND_FUNC_MODULATE;
	MapTextureRgbFunctions["interpolate"]	= eRGB_BLEND_FUNC_INTERPOLATE;
	MapTextureRgbFunctions["substract"]		= eRGB_BLEND_FUNC_SUBSTRACT;
	MapTextureRgbFunctions["dot3_rgb"]		= eRGB_BLEND_FUNC_DOT3_RGB;
	MapTextureRgbFunctions["dot3_rgba"]		= eRGB_BLEND_FUNC_DOT3_RGBA;

	MapTextureAlphaFunctions.clear();
	MapTextureAlphaFunctions["none"]		= eALPHA_BLEND_FUNC_NONE;
	MapTextureAlphaFunctions["first_arg"]	= eALPHA_BLEND_FUNC_FIRST_ARG;
	MapTextureAlphaFunctions["add"]			= eALPHA_BLEND_FUNC_ADD;
	MapTextureAlphaFunctions["add_signed"]	= eALPHA_BLEND_FUNC_ADD_SIGNED;
	MapTextureAlphaFunctions["modulate"]	= eALPHA_BLEND_FUNC_MODULATE;
	MapTextureAlphaFunctions["interpolate"]	= eALPHA_BLEND_FUNC_INTERPOLATE;
	MapTextureAlphaFunctions["substract"]	= eALPHA_BLEND_FUNC_SUBSTRACT;
}

//*********************************************************************************************
