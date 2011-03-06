#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Module_Dx9Render.h"

using namespace Castor3D;
using namespace Castor::Resources;

String D3dEnum::HlslStrings[] =
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
String D3dEnum::HlslErrors[] =
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
D3DPRIMITIVETYPE D3dEnum::DrawTypes[eNbDrawTypes] =
{
	D3DPT_TRIANGLELIST,
	D3DPT_LINELIST,
	D3DPT_POINTLIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	D3DPT_POINTLIST,
	D3DPT_POINTLIST
};
DWORD D3dEnum::EnvironmentModes[TextureEnvironment::eNbEnvModes] =
{
	0,
	0,
	0,
	0,
	0
};
D3DTEXTUREOP D3dEnum::RgbCombinations[TextureEnvironment::eNbRgbCombinations] =
{
	D3DTOP_SELECTARG1,
	D3DTOP_SELECTARG1,
	D3DTOP_MODULATE,
	D3DTOP_ADD,
	D3DTOP_ADDSIGNED,
	D3DTOP_DOTPRODUCT3,
	D3DTOP_DOTPRODUCT3,
	D3DTOP_DOTPRODUCT3
};
DWORD D3dEnum::RgbOperands[TextureEnvironment::eNbRgbOperands] =
{
	0,
	0,
	0,
	0
};
D3DTEXTUREOP D3dEnum::AlphaCombinations[TextureEnvironment::eNbAlphaCombinations] =
{
	D3DTOP_SELECTARG1,
	D3DTOP_SELECTARG1,
	D3DTOP_MODULATE,
	D3DTOP_ADD,
	D3DTOP_ADDSIGNED,
	D3DTOP_ADDSIGNED
};
DWORD D3dEnum::AlphaOperands[TextureEnvironment::eNbAlphaOperands] =
{
	0,
	0
};
DWORD D3dEnum::CombinationSources[TextureEnvironment::eNbCombinationSources] =
{
	D3DTA_TFACTOR,
	D3DTA_TEXTURE,
	D3DTA_CONSTANT,
	D3DTA_DIFFUSE,
	D3DTA_TEMP
};
DWORD D3dEnum::TextureDimensions[TextureUnit::eNbTextureDimensions] =
{
	0,
	0,
	0
};
D3DCMPFUNC D3dEnum::AlphaFuncs[TextureUnit::eNbAlphaFuncs] =
{
	D3DCMP_ALWAYS,
	D3DCMP_LESS,
	D3DCMP_LESSEQUAL,
	D3DCMP_EQUAL,
	D3DCMP_NOTEQUAL,
	D3DCMP_GREATEREQUAL,
	D3DCMP_GREATER,
	D3DCMP_NEVER
};
D3DTEXTUREADDRESS D3dEnum::TextureWrapMode[TextureUnit::eNbWrapModes] =
{
	D3DTADDRESS_WRAP,
	D3DTADDRESS_CLAMP,
	D3DTADDRESS_BORDER,
	D3DTADDRESS_BORDER
};
DWORD D3dEnum::TextureInterpolation[TextureUnit::eNbInterpolationModes] =
{
	D3DTEXF_POINT,
	D3DTEXF_LINEAR,
	D3DTEXF_ANISOTROPIC
};
D3DTEXTUREOP D3dEnum::TextureRgbModes[TextureUnit::eNbTextureRgbModes] =
{
	D3DTOP_SELECTARG1,
	D3DTOP_SELECTARG1,
	D3DTOP_ADD,
	D3DTOP_MODULATE
};
D3DTEXTUREOP D3dEnum::TextureAlphaModes[TextureUnit::eNbTextureAlphaModes] =
{
	D3DTOP_SELECTARG1,
	D3DTOP_SELECTARG1,
	D3DTOP_ADD,
	D3DTOP_MODULATE
};
DWORD D3dEnum::TextureArguments[TextureUnit::eNbTextureArguments] =
{
	D3DTA_DIFFUSE,
	D3DTA_TEXTURE,
	D3DTA_CURRENT,
	D3DTA_TFACTOR
};
DWORD D3dEnum::LightIndexes[8] =
{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7
};
D3DLIGHTTYPE D3dEnum::LightTypes[Light::eNbLightTypes] =
{
	D3DLIGHT_DIRECTIONAL,
	D3DLIGHT_POINT,
	D3DLIGHT_SPOT
};
D3DBLEND D3dEnum::SrcBlendFactors[Pass::eNbSrcBlendFactors] =
{
	D3DBLEND_ZERO,
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA,
	D3DBLEND_SRCALPHASAT
};
D3DBLEND D3dEnum::DstBlendFactors[Pass::eNbDstBlendFactors] =
{
	D3DBLEND_ZERO,
	D3DBLEND_ZERO,
	D3DBLEND_ONE,
	D3DBLEND_SRCCOLOR,
	D3DBLEND_INVSRCCOLOR,
	D3DBLEND_SRCALPHA,
	D3DBLEND_INVSRCALPHA,
	D3DBLEND_DESTALPHA,
	D3DBLEND_INVDESTALPHA
};
D3DFORMAT D3dEnum::PixelFormats[eNbPixelFormats] =
{
	D3DFMT_A4L4,
	D3DFMT_A4L4,
	D3DFMT_L8,
	D3DFMT_A8L8,
	D3DFMT_A1R5G5B5,
	D3DFMT_A4R4G4B4,
	D3DFMT_R8G8B8,
	D3DFMT_A8R8G8B8,
	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5
};

std::map <HRESULT, String> D3dEnum::MapErrors;

bool D3dEnum :: D3dCheckError( HRESULT p_hResult, const String & p_text, bool p_bThrow)
{
	if (MapErrors.size() == 0)
	{
		MapErrors[D3DERR_WRONGTEXTUREFORMAT       ] = CU_T( "D3D : Wrong texture format");
		MapErrors[D3DERR_UNSUPPORTEDCOLOROPERATION] = CU_T( "D3D : Unsupported color operation");
		MapErrors[D3DERR_UNSUPPORTEDCOLORARG      ] = CU_T( "D3D : Unsupported color arg");
		MapErrors[D3DERR_UNSUPPORTEDALPHAOPERATION] = CU_T( "D3D : Unsupported alpha operation");
		MapErrors[D3DERR_UNSUPPORTEDALPHAARG      ] = CU_T( "D3D : Unsupported alpha arg");
		MapErrors[D3DERR_TOOMANYOPERATIONS        ] = CU_T( "D3D : Too many operations");
		MapErrors[D3DERR_CONFLICTINGTEXTUREFILTER ] = CU_T( "D3D : Conflicting texture filter");
		MapErrors[D3DERR_UNSUPPORTEDFACTORVALUE   ] = CU_T( "D3D : Unsupported factor value");
		MapErrors[D3DERR_CONFLICTINGRENDERSTATE   ] = CU_T( "D3D : Conflicting render state");
		MapErrors[D3DERR_UNSUPPORTEDTEXTUREFILTER ] = CU_T( "D3D : Unsupported texture filter");
		MapErrors[D3DERR_CONFLICTINGTEXTUREPALETTE] = CU_T( "D3D : Conflicting texture palette");
		MapErrors[D3DERR_DRIVERINTERNALERROR      ] = CU_T( "D3D : Driver internal error");

		MapErrors[D3DERR_NOTFOUND                 ] = CU_T( "D3D : Not found");
		MapErrors[D3DERR_MOREDATA                 ] = CU_T( "D3D : More data");
		MapErrors[D3DERR_DEVICELOST               ] = CU_T( "D3D : Device lost");
		MapErrors[D3DERR_DEVICENOTRESET           ] = CU_T( "D3D : Device not reset");
		MapErrors[D3DERR_NOTAVAILABLE             ] = CU_T( "D3D : Not available");
		MapErrors[D3DERR_OUTOFVIDEOMEMORY         ] = CU_T( "D3D : Out of video memory");
		MapErrors[D3DERR_INVALIDDEVICE            ] = CU_T( "D3D : Invalid device");
		MapErrors[D3DERR_INVALIDCALL              ] = CU_T( "D3D : Invalid call");
		MapErrors[D3DERR_DRIVERINVALIDCALL        ] = CU_T( "D3D : Driver invalid call");
		MapErrors[D3DERR_WASSTILLDRAWING          ] = CU_T( "D3D : Was still drawing");
		MapErrors[D3DOK_NOAUTOGEN                 ] = CU_T( "D3D : No autogen");
/*
		MapErrors[D3DERR_DEVICEREMOVED              ] = CU_T( "D3D : Device removed");
		MapErrors[S_NOT_RESIDENT                    ] = CU_T( "D3D : Not resident");
		MapErrors[S_RESIDENT_IN_SHARED_MEMORY       ] = CU_T( "D3D : Resident in shared memory");
		MapErrors[S_PRESENT_MODE_CHANGED            ] = CU_T( "D3D : Present mode changed");
		MapErrors[S_PRESENT_OCCLUDED                ] = CU_T( "D3D : Present occluded");
		MapErrors[D3DERR_DEVICEHUNG                 ] = CU_T( "D3D : Device hung");
		MapErrors[D3DERR_UNSUPPORTEDOVERLAY         ] = CU_T( "D3D : Unsupported overlay");
		MapErrors[D3DERR_UNSUPPORTEDOVERLAYFORMAT   ] = CU_T( "D3D : Unsupported overlay format");
		MapErrors[D3DERR_CANNOTPROTECTCONTENT       ] = CU_T( "D3D : Cannot protect content");
		MapErrors[D3DERR_UNSUPPORTEDCRYPTO          ] = CU_T( "D3D : Unsupported crypto");
		MapErrors[D3DERR_PRESENT_STATISTICS_DISJOINT] = CU_T( "D3D : Present statistics disjoint");
*/
	}
	
	if (p_hResult != D3D_OK)
	{
		String l_strWinError = p_text + CU_T( " :");
		LPTSTR l_szError = 0;
		std::map <HRESULT, String>::iterator l_it = MapErrors.find( p_hResult);

		if (l_it != MapErrors.end())
		{
			l_strWinError += " " + l_it->second;
		}

		if (::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, (LPTSTR)& l_szError, 0, NULL) != 0)
		{
			l_strWinError += String( " - Windows : ") + l_szError;
			LocalFree( l_szError);
			l_strWinError.replace( "\r", "");
			l_strWinError.replace( "\n", "");
		}

		Logger::LogError( l_strWinError, p_bThrow);

		return false;
	}

	return true;
}

void D3dEnum :: CgCheckError( const String & p_strText)
{
	CGerror l_errorCode = cgGetError();

	if (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + CU_T( " - ") + cgGetErrorString( l_errorCode), false);
	}
}

String D3dEnum :: GetHlslErrorString( int p_index)
{
	return HlslStrings[p_index];
}

D3DPRIMITIVETYPE D3dEnum :: Get( ePRIMITIVE_TYPE p_index)
{
	return DrawTypes[p_index];
}

DWORD D3dEnum :: Get( TextureEnvironment::eMODE p_index)
{
	return EnvironmentModes[p_index];
}

D3DTEXTUREOP D3dEnum :: Get( TextureEnvironment::eRGB_COMBINATION p_index)
{
	return RgbCombinations[p_index];
}

DWORD D3dEnum :: Get( TextureEnvironment::eRGB_OPERAND p_index)
{
	return RgbOperands[p_index];
}

D3DTEXTUREOP D3dEnum :: Get( TextureEnvironment::eALPHA_COMBINATION p_index)
{
	return AlphaCombinations[p_index];
}

DWORD D3dEnum :: Get( TextureEnvironment::eALPHA_OPERAND p_index)
{
	return AlphaOperands[p_index];
}

DWORD D3dEnum :: Get( TextureEnvironment::eCOMBINATION_SOURCE p_index)
{
	return CombinationSources[p_index];
}

DWORD D3dEnum :: Get( TextureUnit::eDIMENSION p_index)
{
	return TextureDimensions[p_index];
}

D3DCMPFUNC D3dEnum :: Get( TextureUnit::eALPHA_FUNC p_eAlphaFunc)
{
	return AlphaFuncs[p_eAlphaFunc];
}

D3DTEXTUREADDRESS D3dEnum :: Get( TextureUnit::eWRAP_MODE p_eWrapMode)
{
	return TextureWrapMode[p_eWrapMode];
}

DWORD D3dEnum :: Get( TextureUnit::eINTERPOLATION_MODE p_eInterpolationMode)
{
	return TextureInterpolation[p_eInterpolationMode];
}

D3DTEXTUREOP D3dEnum :: Get( TextureUnit::eTEXTURE_RGB_MODE p_eMode)
{
	return TextureRgbModes[p_eMode];
}

D3DTEXTUREOP D3dEnum :: Get( TextureUnit::eTEXTURE_ALPHA_MODE p_eMode)
{
	return TextureAlphaModes[p_eMode];
}

DWORD D3dEnum :: Get( TextureUnit::eTEXTURE_ARGUMENT p_eArgument)
{
	return TextureArguments[p_eArgument];
}

DWORD D3dEnum :: Get( eLIGHT_INDEXES p_uiIndex)
{
	return LightIndexes[p_uiIndex];
}

D3DBLEND D3dEnum :: Get( Pass::eSRC_BLEND_FACTOR p_eBlendFactor)
{
	return SrcBlendFactors[p_eBlendFactor];
}

D3DBLEND D3dEnum :: Get( Pass::eDST_BLEND_FACTOR p_eBlendFactor)
{
	return DstBlendFactors[p_eBlendFactor];
}

D3DFORMAT D3dEnum :: Get( ePIXEL_FORMAT p_pixelFormat)
{
	return PixelFormats[p_pixelFormat];
}

D3DLIGHTTYPE D3dEnum :: Get( Light::eLIGHT_TYPE p_eLightType)
{
	return LightTypes[p_eLightType];
}

DWORD D3dEnum :: GetLockFlags( size_t p_uiFlags)
{
	DWORD l_ulLockFlags = 0;

	if (p_uiFlags & eLockReadOnly)
	{
		l_ulLockFlags = D3DLOCK_READONLY;
	}

	if (p_uiFlags & eLockWriteOnly)
	{
		l_ulLockFlags = D3DLOCK_DISCARD;
	}

	return l_ulLockFlags;
}