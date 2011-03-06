#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/TextureEnvironment.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneManager.h"

using namespace Castor3D;

std::map<String, TextureUnit::eALPHA_FUNC> TextureUnit::MapAlphaFuncs;
const String TextureUnit::StringAlphaFuncs[eNbAlphaFuncs]	= { "always", "less", "less_or_equal", "equal", "not_equal", "greater_or_equal", "greater", "never" };

std::map<String, TextureUnit::eTEXTURE_ARGUMENT> TextureUnit::MapTextureArguments;
const String TextureUnit::StringTextureArguments[eNbTextureArguments]	= { "diffuse", "texture", "previous", "constant" };

std::map<String, TextureUnit::eTEXTURE_RGB_MODE> TextureUnit::MapTextureRgbModes;
const String TextureUnit::StringTextureRgbModes[eNbTextureRgbModes]	= { "none", "first_arg", "add", "modulate" };

std::map<String, TextureUnit::eTEXTURE_ALPHA_MODE> TextureUnit::MapTextureAlphaModes;
const String TextureUnit::StringTextureAlphaModes[eNbTextureAlphaModes]	= { "none", "first_arg", "add", "modulate" };

TextureUnit :: TextureUnit()
	:	m_uiIndex				( 0)
	,	m_bTextureInitialised	( false)
	,	m_bTextured				( false)
	,	m_eMode					( eNone)
	,	m_clrPrimaryColour		( 1.0f, 1.0f, 1.0f, 1.0f)
	,	m_eTextureType			( eTex2D)
	,	m_environment			( new TextureEnvironment)
	,	m_eAlphaFunc			( eAlphaAlways)
	,	m_fAlphaValue			( 0)
	,	m_eRgbMode				( eRgbModeNone)
	,	m_eAlpMode				( eAlphaModeNone)
{
	m_eRgbArguments[0] = eNbTextureArguments;
	m_eRgbArguments[1] = eNbTextureArguments;
	m_eAlpArguments[0] = eNbTextureArguments;
	m_eAlpArguments[1] = eNbTextureArguments;
	m_eWrapModes[0] = eWrapRepeat;
	m_eWrapModes[1] = eWrapRepeat;
	m_eWrapModes[2] = eWrapRepeat;
	m_eInterpolationModes[0] = eLinear;
	m_eInterpolationModes[1] = eLinear;

	_initialiseMaps();
}

TextureUnit :: TextureUnit( const TextureUnit & p_copy)
	:	m_uiIndex				( p_copy.m_uiIndex)
	,	m_pImage				( p_copy.m_pImage)
	,	m_environment			( p_copy.m_environment)
	,	m_eTextureType			( p_copy.m_eTextureType)
	,	m_bTextureInitialised	( p_copy.m_bTextureInitialised)
	,	m_bTextured				( p_copy.m_bTextured)
	,	m_clrPrimaryColour		( p_copy.m_clrPrimaryColour)
	,	m_eMode					( p_copy.m_eMode)
	,	m_eAlphaFunc			( p_copy.m_eAlphaFunc)
	,	m_fAlphaValue			( p_copy.m_fAlphaValue)
	,	m_eRgbMode				( p_copy.m_eRgbMode)
	,	m_eAlpMode				( p_copy.m_eAlpMode)
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
	m_environment			= p_copy.m_environment;
	m_eTextureType			= p_copy.m_eTextureType;
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

void TextureUnit :: SetTexture2D( ImagePtr p_pImage)
{
	if (p_pImage != NULL)
	{
		if (m_pImage != NULL)
		{
			m_pImage.reset();
		}

		m_pImage = p_pImage;
		m_bTextureInitialised = false;
		m_bTextured = true;
		m_eTextureType = eTex2D;
	}
}

void TextureUnit :: Initialise()
{
	if (m_bTextured)
	{
		if (m_eTextureType == eTex2D &&  ! m_bTextureInitialised)
		{
			m_pRenderer->Initialise();
		}

		m_bTextureInitialised = true;
	}
}

void TextureUnit :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->Render();
//		m_environment->Render( p_displayMode);
	}
}

void TextureUnit :: EndRender()
{
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->EndRender();
	}
}

const unsigned char * TextureUnit :: GetImagePixels()const
{
	const unsigned char * l_pReturn = NULL;

	if (m_bTextured && m_pImage != NULL)
	{
		l_pReturn = m_pImage->GetBuffer();
	}

	return NULL;
}

unsigned char * TextureUnit :: GetImagePixels()
{
	unsigned char * l_pReturn = NULL;

	if (m_bTextured && m_pImage != NULL)
	{
		l_pReturn = m_pImage->GetBuffer();
	}

	return l_pReturn;
}

unsigned int TextureUnit :: GetWidth()const
{
	unsigned int l_uiReturn = 0;

	if (m_bTextured && m_pImage != NULL)
	{
		l_uiReturn = m_pImage->GetWidth();
	}

	return l_uiReturn;
}

unsigned int TextureUnit :: GetHeight()const
{
	unsigned int l_uiReturn = 0;

	if (m_bTextured && m_pImage != NULL)
	{
		l_uiReturn = m_pImage->GetHeight();
	}

	return l_uiReturn;
}

String TextureUnit :: GetTexturePath()const
{
	String l_strReturn;

	if (m_bTextured && m_pImage != NULL)
	{
		l_strReturn = m_pImage->GetPath();
	}

	return l_strReturn;
}

ePIXEL_FORMAT TextureUnit :: GetPixelFormat()
{
	ePIXEL_FORMAT l_pfReturn;

	if (m_bTextured && m_pImage != NULL)
	{
		l_pfReturn = m_pImage->GetPixelFormat();
	}

	return l_pfReturn;
}

bool TextureUnit :: Write( File & p_file)const
{
	bool l_bReturn = true;

	if (m_bTextured)
	{
		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( "\t\ttexture_unit\n\t\t{\n") > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, "\t\t\tcolour %f %f %f %f\n", m_clrPrimaryColour[0], m_clrPrimaryColour[1], m_clrPrimaryColour[2], m_clrPrimaryColour[3]) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, "\t\t\tmap_type %i\n", m_eMode) > 0;
		}

		if (l_bReturn && m_pImage != NULL)
		{
			Path l_path = m_pImage->GetPath();
			l_bReturn = p_file.WriteLine( "\t\t\timage " + l_path + "\n") > 0;
		}

		if (l_bReturn)
		{
			String l_strLine;
			l_strLine << StringAlphaFuncs[m_eAlphaFunc] << " " << m_fAlphaValue;
			l_bReturn = p_file.WriteLine( "\t\t\talpha_func " + l_strLine + "\n") > 0;
		}

		if (l_bReturn && m_eRgbMode != eRgbModeNone)
		{
			String l_strLine;
			l_strLine << StringTextureRgbModes[m_eRgbMode] << " " << StringTextureArguments[m_eRgbArguments[0]] << " " << StringTextureArguments[m_eRgbArguments[1]];
			l_bReturn = p_file.WriteLine( "\t\t\trgb_blend " + l_strLine + "\n") > 0;
		}

		if (l_bReturn && m_eAlpMode != eAlphaModeNone)
		{
			String l_strLine;
			l_strLine << StringTextureAlphaModes[m_eAlpMode] << " " << StringTextureArguments[m_eAlpArguments[0]] << " " << StringTextureArguments[m_eAlpArguments[1]];
			l_bReturn = p_file.WriteLine( "\t\t\talpha_blend " + l_strLine + "\n") > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteLine( "\t\t}\n") > 0;
		}
	}

	return l_bReturn;
}

bool TextureUnit :: Save( File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_bTextured) == sizeof( bool));

	if (l_bReturn && m_bTextured)
	{
		l_bReturn = (p_file.Write( m_eTextureType) == sizeof( eDIMENSION));

		if (l_bReturn)
		{
			l_bReturn = m_clrPrimaryColour.Save( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Write( m_eMode) == sizeof( eMAP_MODE));
		}

		if (l_bReturn)
		{
			l_bReturn = m_environment->Save( p_file);
		}

		String l_strName;

		if (l_bReturn)
		{
			l_bReturn = p_file.Write( String( m_pImage->GetPath()));
		}
	}

	return l_bReturn;
}

bool TextureUnit :: Load( File & p_file)
{
	ImageManager * l_pManager = RenderSystem::GetSingletonPtr()->GetSceneManager()->GetImageManager();
	bool l_bReturn = (p_file.Read( m_bTextured) == sizeof( bool));

	if (l_bReturn && m_bTextured)
	{
		l_bReturn = (p_file.Read( m_eTextureType) == sizeof( eDIMENSION));

		if (l_bReturn)
		{
			l_bReturn = m_clrPrimaryColour.Load( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Read( m_eMode) == sizeof( eMAP_MODE));
		}

		if (l_bReturn)
		{
			l_bReturn = m_environment->Load( p_file);
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
			SetTexture2D( l_pManager->CreateImage( l_pathName.GetFileName(), l_pathName));
		}
	}

	return l_bReturn;
}

void TextureUnit :: _initialiseMaps()
{
	MapAlphaFuncs.clear();
	MapAlphaFuncs["always"]				= eAlphaAlways;
	MapAlphaFuncs["less"]				= eAlphaLess;
	MapAlphaFuncs["less_or_equal"]		= eAlphaLessOrEqual;
	MapAlphaFuncs["equal"]				= eAlphaEqual;
	MapAlphaFuncs["not_equal"]			= eAlphaNotEqual;
	MapAlphaFuncs["greater_or_equal"]	= eAlphaGreaterOrEqual;
	MapAlphaFuncs["greater"]			= eAlphaGreater;
	MapAlphaFuncs["never"]				= eAlphaNever;

	MapTextureArguments.clear();
	MapTextureArguments["diffuse"]	= eArgumentDiffuse;
	MapTextureArguments["texture"]	= eArgumentTexture;
	MapTextureArguments["previous"]	= eArgumentPrevious;
	MapTextureArguments["constant"]	= eArgumentConstant;

	MapTextureRgbModes.clear();
	MapTextureRgbModes["none"]		= eRgbModeNone;
	MapTextureRgbModes["first_arg"]	= eRgbModeFirstArg;
	MapTextureRgbModes["add"]		= eRgbModeAdd;
	MapTextureRgbModes["modulate"]	= eRgbModeModulate;

	MapTextureAlphaModes.clear();
	MapTextureAlphaModes["none"]		= eAlphaModeNone;
	MapTextureAlphaModes["first_arg"]	= eAlphaModeFirstArg;
	MapTextureAlphaModes["add"]			= eAlphaModeAdd;
	MapTextureAlphaModes["modulate"]	= eAlphaModeModulate;
}
