#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/Module_GlRender.h"

using namespace Castor3D;
using namespace Castor::Resources;

String GlEnum::GlslStrings[] =
{
	CU_T( "[e00] GLSL is not available!"),
	CU_T( "[e01] Not a valid program object!"),
	CU_T( "[e02] Not a valid object!"),
	CU_T( "[e03] Out of memory!"),
	CU_T( "[e04] Unknown compiler error!"),
	CU_T( "[e05] Linker log is not available!"),
	CU_T( "[e06] Compiler log is not available!"),
	CU_T( "[Empty]")
};
String GlEnum::GlslErrors[] =
{
	CU_T( "[500] Invalid Enum !"),
	CU_T( "[501] Invalid Value !"),
	CU_T( "[502] Invalid Operation !"),
	CU_T( "[503] Stack Overflow !"),
	CU_T( "[504] Stack Underflow !"),
	CU_T( "[505] Out of memory !"),
	CU_T( "[506] Table too large"),
	CU_T( "[Empty] Unknown Error")
};
GLenum GlEnum::DrawTypes[eNbDrawTypes] =
{
	GL_TRIANGLES,
	GL_LINES,
	GL_POINTS,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_QUADS,
	GL_QUAD_STRIP
};
GLenum GlEnum::EnvironmentModes[TextureEnvironment::eNbEnvModes] =
{
	GL_REPLACE,
	GL_MODULATE,
	GL_BLEND,
	GL_ADD,
	GL_COMBINE
};
GLenum GlEnum::RgbCombinations[TextureEnvironment::eNbRgbCombinations] =
{
	GL_REPLACE,
	GL_REPLACE,
	GL_MODULATE,
	GL_ADD,
	GL_ADD_SIGNED,
	GL_DOT3_RGB,
	GL_DOT3_RGBA,
	GL_INTERPOLATE
};
GLenum GlEnum::RgbOperands[TextureEnvironment::eNbRgbOperands] =
{
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};
GLenum GlEnum::AlphaCombinations[TextureEnvironment::eNbAlphaCombinations] =
{
	GL_REPLACE,
	GL_REPLACE,
	GL_MODULATE,
	GL_ADD,
	GL_ADD_SIGNED,
	GL_INTERPOLATE
};
GLenum GlEnum::AlphaOperands[TextureEnvironment::eNbAlphaOperands] =
{
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};
GLenum GlEnum::CombinationSources[TextureEnvironment::eNbCombinationSources] =
{
	1,
	GL_TEXTURE,
	GL_CONSTANT,
	GL_PRIMARY_COLOR,
	GL_PREVIOUS
};
GLenum GlEnum::TextureDimensions[TextureUnit::eNbTextureDimensions] =
{
	GL_TEXTURE_1D,
	GL_TEXTURE_2D,
	GL_TEXTURE_3D
};
GLenum GlEnum::AlphaFuncs[TextureUnit::eNbAlphaFuncs] =
{
	GL_ALWAYS,
	GL_LESS,
	GL_LEQUAL,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_GREATER,
	GL_NEVER
};
GLenum GlEnum::TextureWrapMode[TextureUnit::eNbWrapModes] =
{
	GL_REPEAT,
	GL_CLAMP,
	GL_CLAMP_TO_BORDER,
	GL_CLAMP_TO_EDGE
};
GLenum GlEnum::TextureInterpolation[TextureUnit::eNbInterpolationModes] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR
};
GLenum GlEnum::LightIndexes[8] =
{
	GL_LIGHT0,
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3,
	GL_LIGHT4,
	GL_LIGHT5,
	GL_LIGHT6,
	GL_LIGHT7
};
GLenum GlEnum::SrcBlendFactors[Pass::eNbSrcBlendFactors] =
{
	GL_ZERO,
	GL_ZERO,
	GL_ONE,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE
};
GLenum GlEnum::DstBlendFactors[Pass::eNbDstBlendFactors] =
{
	GL_ZERO,
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA
};
GLenum GlEnum::Usages[eNbUsages] =
{
	GL_VERTEX_ARRAY,
	GL_NORMAL_ARRAY,
	GL_TANGENT_ARRAY_EXT,
	GL_TEXTURE_COORD_ARRAY,
	GL_TEXTURE_COORD_ARRAY,
	GL_TEXTURE_COORD_ARRAY,
	GL_TEXTURE_COORD_ARRAY,
	GL_COLOR_ARRAY
};
GLenum GlEnum::TextureArguments[TextureUnit::eNbTextureArguments] =
{
	GL_PRIMARY_COLOR,
	GL_TEXTURE,
	GL_PREVIOUS,
	GL_CONSTANT
};
GLenum GlEnum::TextureRgbModes[TextureUnit::eNbTextureRgbModes] =
{
	0,
	GL_REPLACE,
	GL_ADD,
	GL_MODULATE
};
GLenum GlEnum::TextureAlphaModes[TextureUnit::eNbTextureAlphaModes] =
{
	0,
	GL_REPLACE,
	GL_ADD,
	GL_MODULATE
};
GlEnum::GlPixelFmt GlEnum::PixelFormats[eNbPixelFormats] =
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

void GlEnum :: GlCheckError( const String & p_text, bool p_bThrows)
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
		String l_strError = p_text + CU_T( " - " ) + GlslErrors[l_errorCode];
#ifdef _WIN32
		l_strError += CU_T( " - ") + l_strWinError;
#endif
		Logger::LogError( l_strError, p_bThrows);
	}
}

void GlEnum :: CgCheckError( const String & p_strText, bool p_bThrows)
{
	CGerror l_errorCode = cgGetError();

	if (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + CU_T( " - ") + cgGetErrorString( l_errorCode), p_bThrows);
	}

	GlCheckError( p_strText, p_bThrows);
}

String GlEnum :: GetGlslErrorString( int p_index)
{
	return GlslStrings[p_index];
}

GLenum GlEnum :: Get( ePRIMITIVE_TYPE p_index)
{
	return DrawTypes[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eMODE p_index)
{
	return EnvironmentModes[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eRGB_COMBINATION p_index)
{
	return RgbCombinations[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eRGB_OPERAND p_index)
{
	return RgbOperands[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eALPHA_COMBINATION p_index)
{
	return AlphaCombinations[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eALPHA_OPERAND p_index)
{
	return AlphaOperands[p_index];
}

GLenum GlEnum :: Get( TextureEnvironment::eCOMBINATION_SOURCE p_index)
{
	return CombinationSources[p_index];
}

GLenum GlEnum :: Get( TextureUnit::eDIMENSION p_index)
{
	return TextureDimensions[p_index];
}

GLenum GlEnum :: Get( TextureUnit::eALPHA_FUNC p_eAlphaFunc)
{
	return AlphaFuncs[p_eAlphaFunc];
}

GLenum GlEnum :: Get( TextureUnit::eWRAP_MODE p_eWrapMode)
{
	return TextureWrapMode[p_eWrapMode];
}

GLenum GlEnum :: Get( TextureUnit::eINTERPOLATION_MODE p_eInterpolationMode)
{
	return TextureInterpolation[p_eInterpolationMode];
}

GLenum GlEnum :: Get( TextureUnit::eTEXTURE_ARGUMENT p_eArgument)
{
	return TextureArguments[p_eArgument];
}

GLenum GlEnum :: Get( TextureUnit::eTEXTURE_RGB_MODE p_eMode)
{
	return TextureRgbModes[p_eMode];
}

GLenum GlEnum :: Get( TextureUnit::eTEXTURE_ALPHA_MODE p_eMode)
{
	return TextureAlphaModes[p_eMode];
}

GLenum GlEnum :: Get( eLIGHT_INDEXES p_uiIndex)
{
	return LightIndexes[p_uiIndex];
}

GLenum GlEnum :: Get( Pass::eSRC_BLEND_FACTOR p_eBlendFactor)
{
	return SrcBlendFactors[p_eBlendFactor];
}

GLenum GlEnum :: Get( Pass::eDST_BLEND_FACTOR p_eBlendFactor)
{
	return DstBlendFactors[p_eBlendFactor];
}

GlEnum::GlPixelFmt GlEnum :: Get( ePIXEL_FORMAT p_pixelFormat)
{
	return PixelFormats[p_pixelFormat];
}

GLenum GlEnum :: Get( eELEMENT_USAGE p_eUsage)
{
	return Usages[p_eUsage];
}

GLenum GlEnum :: GetLockFlags( size_t p_uiFlags)
{
	GLenum l_ulLockFlags = GL_READ_WRITE_ARB;

	if (p_uiFlags & eLockReadOnly)
	{
		l_ulLockFlags = GL_READ_ONLY_ARB;
	}

	if (p_uiFlags & eLockWriteOnly)
	{
		l_ulLockFlags = GL_WRITE_ONLY_ARB;
	}

	return l_ulLockFlags;
}

GLenum GlEnum :: GetBufferFlags( size_t p_uiFlags)
{
    return (p_uiFlags & eBufferDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}
