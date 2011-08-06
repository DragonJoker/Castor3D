#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/Module_GlRender.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor::Resources;

String GlEnum::GlslStrings[] =
{	cuT( "[e00] GLSL is not available!")
,	cuT( "[e01] Not a valid program object!")
,	cuT( "[e02] Not a valid object!")
,	cuT( "[e03] Out of memory!")
,	cuT( "[e04] Unknown compiler error!")
,	cuT( "[e05] Linker log is not available!")
,	cuT( "[e06] Compiler log is not available!")
,	cuT( "[Empty]")
};
String GlEnum::GlslErrors[] =
{	cuT( "[500] Invalid Enum !")
,	cuT( "[501] Invalid Value !")
,	cuT( "[502] Invalid Operation !")
,	cuT( "[503] Stack Overflow !")
,	cuT( "[504] Stack Underflow !")
,	cuT( "[505] Out of memory !")
,	cuT( "[506] Table too large")
,	cuT( "[Empty] Unknown Error")
};
GLenum GlEnum::DrawTypes[ePRIMITIVE_TYPE_COUNT] =
{	GL_TRIANGLES
,	GL_LINES
,	GL_POINTS
,	GL_TRIANGLE_STRIP
,	GL_TRIANGLE_FAN
,	GL_QUADS
,	GL_QUAD_STRIP
};
GLenum GlEnum::TextureDimensions[eTEXTURE_TYPE_COUNT] =
{	GL_TEXTURE_1D
,	GL_TEXTURE_2D
,	GL_TEXTURE_3D
};
GLenum GlEnum::AlphaFuncs[TextureUnit::eALPHA_FUNC_COUNT] =
{	GL_ALWAYS
,	GL_LESS
,	GL_LEQUAL
,	GL_EQUAL
,	GL_NOTEQUAL
,	GL_GEQUAL
,	GL_GREATER
,	GL_NEVER
};
GLenum GlEnum::TextureWrapMode[TextureUnit::eWRAP_MODE_COUNT] =
{	GL_REPEAT
,	GL_CLAMP
,	GL_CLAMP_TO_BORDER
,	GL_CLAMP_TO_EDGE
};
GLenum GlEnum::TextureInterpolation[TextureUnit::eINTERPOLATION_MODE_COUNT] =
{	GL_NEAREST
,	GL_LINEAR
,	GL_LINEAR
};
GLenum GlEnum::LightIndexes[eLIGHT_INDEXES_COUNT] =
{	GL_LIGHT0
,	GL_LIGHT1
,	GL_LIGHT2
,	GL_LIGHT3
,	GL_LIGHT4
,	GL_LIGHT5
,	GL_LIGHT6
,	GL_LIGHT7
};
GLenum GlEnum::SrcBlendFactors[Pass::eSRC_BLEND_COUNT] =
{	GL_ZERO
,	GL_ZERO
,	GL_ONE
,	GL_DST_COLOR
,	GL_ONE_MINUS_DST_COLOR
,	GL_SRC_ALPHA
,	GL_ONE_MINUS_SRC_ALPHA
,	GL_DST_ALPHA
,	GL_ONE_MINUS_DST_ALPHA
,	GL_SRC_ALPHA_SATURATE
};
GLenum GlEnum::DstBlendFactors[Pass::eDST_BLEND_COUNT] =
{	GL_ZERO
,	GL_ZERO
,	GL_ONE
,	GL_SRC_COLOR
,	GL_ONE_MINUS_SRC_COLOR
,	GL_SRC_ALPHA
,	GL_ONE_MINUS_SRC_ALPHA
,	GL_DST_ALPHA
,	GL_ONE_MINUS_DST_ALPHA
};
GLenum GlEnum::Usages[eELEMENT_USAGE_COUNT] =
{	GL_VERTEX_ARRAY
,	GL_NORMAL_ARRAY
,	GL_TANGENT_ARRAY_EXT
,	GL_TEXTURE_COORD_ARRAY
,	GL_TEXTURE_COORD_ARRAY
,	GL_TEXTURE_COORD_ARRAY
,	GL_TEXTURE_COORD_ARRAY
,	GL_COLOR_ARRAY
};
GLenum GlEnum::TextureArguments[TextureUnit::eBLEND_SOURCE_COUNT] =
{	GL_TEXTURE
,	GL_TEXTURE0
,	GL_TEXTURE1
,	GL_TEXTURE2
,	GL_TEXTURE3
,	GL_CONSTANT
,	GL_PRIMARY_COLOR
,	GL_PREVIOUS
};
GLenum GlEnum::RgbBlendFuncs[TextureUnit::eRGB_BLEND_FUNC_COUNT] =
{	GL_MODULATE
,	GL_REPLACE
,	GL_ADD
,	GL_ADD_SIGNED
,	GL_MODULATE
,	GL_INTERPOLATE
,	GL_SUBTRACT
,	GL_DOT3_RGB
,	GL_DOT3_RGBA
};
GLenum GlEnum::AlphaBlendFuncs[TextureUnit::eALPHA_BLEND_FUNC_COUNT] =
{	GL_MODULATE
,	GL_REPLACE
,	GL_ADD
,	GL_ADD_SIGNED
,	GL_MODULATE
,	GL_INTERPOLATE
,	GL_SUBTRACT
};
GlEnum::GlPixelFmt GlEnum::PixelFormats[ePIXEL_FORMAT_COUNT] =
{
	{GL_LUMINANCE,       GL_LUMINANCE4,                    GL_UNSIGNED_BYTE},
	{GL_LUMINANCE_ALPHA, GL_LUMINANCE4_ALPHA4,             GL_UNSIGNED_BYTE},
	{GL_LUMINANCE,       GL_LUMINANCE8,                    GL_UNSIGNED_BYTE},
	{GL_LUMINANCE_ALPHA, GL_LUMINANCE8_ALPHA8,             GL_UNSIGNED_BYTE},
	{GL_BGRA,            GL_RGB5_A1,                       GL_UNSIGNED_SHORT_1_5_5_5_REV},
	{GL_BGRA,            GL_RGBA4,                         GL_UNSIGNED_SHORT_4_4_4_4_REV},
	{GL_BGR,             GL_RGB8,                          GL_UNSIGNED_BYTE},
	{GL_BGRA,            GL_RGBA8,                         GL_UNSIGNED_BYTE},
	{GL_BGR,             GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0},
	{GL_BGRA,            GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0},
	{GL_BGRA,            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0}
};
GLenum GlEnum::ShaderTypes[eSHADER_TYPE_COUNT] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

bool GlEnum :: GlCheckError( String const & p_text, bool p_bThrows)
{
	bool l_bReturn = true;
#ifdef _WIN32
	String l_strWinError;
	DWORD l_dwError = GetLastError();
	LPTSTR l_szError = 0;

	if (l_dwError != ERROR_SUCCESS && ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, l_dwError, 0, (LPTSTR)& l_szError, 0, nullptr) != 0)
	{
		l_strWinError = l_szError;
		LocalFree( l_szError);
	}
#endif
	GLenum l_errorCode = OpenGl::GetError();

	if (l_errorCode != GL_NO_ERROR)
	{
		l_errorCode -= GL_INVALID_ENUM;
		String l_strError = p_text + cuT( " - " ) + GlslErrors[l_errorCode];
#ifdef _WIN32
		l_strError += cuT( " - ") + l_strWinError;
#endif
		Logger::LogError( l_strError, p_bThrows);
		l_bReturn = false;
	}

	return l_bReturn;
}

bool GlEnum :: CgCheckError( String const & p_strText, bool p_bThrows)
{
	bool l_bReturn = true;
	CGerror l_errorCode = cgGetError();

	while (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + cuT( " - ") + cgGetErrorString( l_errorCode), p_bThrows);
		GlCheckError( p_strText, p_bThrows);
		l_errorCode = cgGetError();
		l_bReturn = false;
	}

	return l_bReturn;
}

GLenum GlEnum :: GetLockFlags( size_t p_uiFlags)
{
	GLenum l_ulLockFlags = GL_READ_WRITE_ARB;

	if (p_uiFlags & eLOCK_FLAG_READ_ONLY)
	{
		l_ulLockFlags = GL_READ_ONLY_ARB;
	}

	if (p_uiFlags & eLOCK_FLAG_WRITE_ONLY)
	{
		l_ulLockFlags = GL_WRITE_ONLY_ARB;
	}

	return l_ulLockFlags;
}