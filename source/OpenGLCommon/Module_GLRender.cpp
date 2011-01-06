#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/Module_GLRender.h"

using namespace Castor::Resources;

Char *	g_GLSLStrings[]			= { CU_T( "[e00] GLSL is not available!"), CU_T( "[e01] Not a valid program object!"), CU_T( "[e02] Not a valid object!"), CU_T( "[e03] Out of memory!"), CU_T( "[e04] Unknown compiler error!"), CU_T( "[e05] Linker log is not available!"), CU_T( "[e06] Compiler log is not available!"), CU_T( "[Empty]") };
Char *	g_GLSLErrors[]			= { CU_T( "[500] Invalid Enum !"), CU_T( "[501] Invalid Value !"), CU_T( "[502] Invalid Operation !"), CU_T( "[503] Stack Overflow !"), CU_T( "[504] Stack Underflow !"), CU_T( "[505] Out of memory !"), CU_T( "[506] Table too large"), CU_T( "[Empty] Unknown Error") };
int		g_drawTypes[7]			= { GL_TRIANGLES, GL_LINES, GL_POINTS, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP };
int		g_environmentModes[5]	= { GL_REPLACE, GL_MODULATE, GL_BLEND, GL_ADD, GL_COMBINE };
int 	g_RGBCombinations[8]	= { 0, GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_DOT3_RGB, GL_DOT3_RGBA, GL_INTERPOLATE };
int 	g_RGBOperands[4]		= { GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
int 	g_alphaCombinations[6]	= { 0, GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_INTERPOLATE };
int 	g_alphaOperands[2]		= { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
int 	g_combinationSources[5]	= { 1, GL_TEXTURE, GL_CONSTANT, GL_PRIMARY_COLOR, GL_PREVIOUS };
int 	g_textureDimensions[3]	= { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D };
int 	g_lightIndexes[8]		= { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

void Castor3D :: GLCheckError( const String & p_text)
{
#ifdef _WIN32
	String l_strWinError;
	DWORD l_dwError = GetLastError();
	LPTSTR l_szError = 0;
	if (::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, l_dwError, 0, (LPTSTR)& l_szError, 0, NULL) != 0)
	{
		l_strWinError = l_szError;
		LocalFree( l_szError);
	}
#endif
	GLenum l_errorCode = glGetError();

	if (l_errorCode != GL_NO_ERROR)
	{
		l_errorCode -= GL_INVALID_ENUM;
		String l_strError = p_text + CU_T( " - " ) + g_GLSLErrors[l_errorCode];
#ifdef _WIN32
		l_strError += CU_T( " - ") + l_strWinError;
#endif
		Logger::LogError( l_strError);
	}
}

void Castor3D :: CgCheckError( const String & p_strText)
{
	CGerror l_errorCode = cgGetError();

	if (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + CU_T( " - ") + cgGetErrorString( l_errorCode));
	}

	GLCheckError( p_strText);
}

Char * Castor3D :: GetGLSLErrorString( int p_index)
{
	return g_GLSLStrings[p_index];
}

int Castor3D :: GetDrawType( eDRAW_TYPE p_index)
{
	return g_drawTypes[p_index];
}

int Castor3D :: GetEnvironmentMode( TextureEnvironment::eMODE p_index)
{
	return g_environmentModes[p_index];
}

int Castor3D :: GetRGBCombination( TextureEnvironment::eRGB_COMBINATION p_index)
{
	return g_RGBCombinations[p_index];
}

int Castor3D :: GetRGBOperand( TextureEnvironment::eRGB_OPERAND p_index)
{
	return g_RGBOperands[p_index];
}

int Castor3D :: GetAlphaCombination( TextureEnvironment::eALPHA_COMBINATION p_index)
{
	return g_alphaCombinations[p_index];
}

int Castor3D :: GetAlphaOperand( TextureEnvironment::eALPHA_OPERAND p_index)
{
	return g_alphaOperands[p_index];
}

int Castor3D :: GetCombinationSource( TextureEnvironment::eCOMBINATION_SOURCE p_index)
{
	return g_combinationSources[p_index];
}

int Castor3D :: GetTextureDimension( TextureUnit::eDIMENSION p_index)
{
	return g_textureDimensions[p_index];
}

int Castor3D :: GetLightIndex( eLIGHT_INDEXES p_uiIndex)
{
	return g_lightIndexes[p_uiIndex];
}

int Castor3D :: GetPixelFormatBits( PixelFormat p_pixelFormat)
{
	int l_iReturn;

	switch (p_pixelFormat)
	{
	case pxfL8:
	case pxfDXTC1:
		l_iReturn = 8;
		break;

	case pxfL8A8:
	case pxfR5G5B5A1:
	case pxfR4G4B4A4:
	case pxfDXTC3:
	case pxfDXTC5:
		l_iReturn = 16;
		break;

	case pxfR8G8B8:
		l_iReturn = 24;
		break;

	case pxfR8G8B8A8:
		l_iReturn = 32;
		break;

	default:
		l_iReturn = 1;
		break;
	}

	return l_iReturn;
}