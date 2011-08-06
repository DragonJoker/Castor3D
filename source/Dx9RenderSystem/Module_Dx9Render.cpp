#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Module_Dx9Render.hpp"

using namespace Castor3D;
using namespace Castor::Resources;

String D3dEnum::HlslStrings[] =
{	cuT( "[e00] GLSL is not available!")
,	cuT( "[e01] Not a valid program object!")
,	cuT( "[e02] Not a valid object!")
,	cuT( "[e03] Out of memory!")
,	cuT( "[e04] Unknown compiler error!")
,	cuT( "[e05] Linker log is not available!")
,	cuT( "[e06] Compiler log is not available!")
,	cuT( "[Empty]")
};
String D3dEnum::HlslErrors[] =
{	cuT( "[500] Invalid Enum !")
,	cuT( "[501] Invalid Value !")
,	cuT( "[502] Invalid Operation !")
,	cuT( "[503] Stack Overflow !")
,	cuT( "[504] Stack Underflow !")
,	cuT( "[505] Out of memory !")
,	cuT( "[506] Table too large")
,	cuT( "[Empty] Unknown Error")
};
D3DPRIMITIVETYPE D3dEnum::DrawTypes[ePRIMITIVE_TYPE_COUNT] =
{	D3DPT_TRIANGLELIST
,	D3DPT_LINELIST
,	D3DPT_POINTLIST
,	D3DPT_TRIANGLESTRIP
,	D3DPT_TRIANGLEFAN
,	D3DPT_POINTLIST
,	D3DPT_POINTLIST
};
DWORD D3dEnum::TextureDimensions[eTEXTURE_TYPE_COUNT] =
{	0
,	0
,	0
};
D3DCMPFUNC D3dEnum::AlphaFuncs[TextureUnit::eALPHA_FUNC_COUNT] =
{	D3DCMP_ALWAYS
,	D3DCMP_LESS
,	D3DCMP_LESSEQUAL
,	D3DCMP_EQUAL
,	D3DCMP_NOTEQUAL
,	D3DCMP_GREATEREQUAL
,	D3DCMP_GREATER
,	D3DCMP_NEVER
};
D3DTEXTUREADDRESS D3dEnum::TextureWrapMode[TextureUnit::eWRAP_MODE_COUNT] =
{	D3DTADDRESS_WRAP
,	D3DTADDRESS_CLAMP
,	D3DTADDRESS_BORDER
,	D3DTADDRESS_BORDER
};
DWORD D3dEnum::TextureInterpolation[TextureUnit::eINTERPOLATION_MODE_COUNT] =
{	D3DTEXF_POINT
,	D3DTEXF_LINEAR
,	D3DTEXF_GAUSSIANQUAD//D3DTEXF_ANISOTROPIC
};
D3DTEXTUREOP D3dEnum::RgbBlendFuncs[TextureUnit::eRGB_BLEND_FUNC_COUNT] =
{	D3DTOP_MODULATE
,	D3DTOP_SELECTARG1
,	D3DTOP_ADD
,	D3DTOP_ADDSIGNED
,	D3DTOP_MODULATE
,	D3DTOP_ADDSMOOTH
,	D3DTOP_SUBTRACT
,	D3DTOP_DOTPRODUCT3
,	D3DTOP_DOTPRODUCT3
};
D3DTEXTUREOP D3dEnum::AlphaBlendFuncs[TextureUnit::eALPHA_BLEND_FUNC_COUNT] =
{	D3DTOP_MODULATE
,	D3DTOP_SELECTARG1
,	D3DTOP_ADD
,	D3DTOP_ADDSIGNED
,	D3DTOP_MODULATE
,	D3DTOP_ADDSMOOTH
,	D3DTOP_SUBTRACT
};
DWORD D3dEnum::TextureArguments[TextureUnit::eBLEND_SOURCE_COUNT] =
{	D3DTA_TEXTURE
,	D3DTA_TEXTURE
,	D3DTA_TEXTURE
,	D3DTA_TEXTURE
,	D3DTA_TEXTURE
,	D3DTA_CONSTANT
,	D3DTA_DIFFUSE
,	D3DTA_TFACTOR
};
DWORD D3dEnum::LightIndexes[eLIGHT_INDEXES_COUNT] =
{	0
,	1
,	2
,	3
,	4
,	5
,	6
,	7
};
D3DLIGHTTYPE D3dEnum::LightTypes[eLIGHT_TYPE_COUNT] =
{	D3DLIGHT_DIRECTIONAL
,	D3DLIGHT_POINT
,	D3DLIGHT_SPOT
};
D3DBLEND D3dEnum::SrcBlendFactors[Pass::eSRC_BLEND_COUNT] =
{	D3DBLEND_ZERO
,	D3DBLEND_ZERO
,	D3DBLEND_ONE
,	D3DBLEND_DESTCOLOR
,	D3DBLEND_INVDESTCOLOR
,	D3DBLEND_SRCALPHA
,	D3DBLEND_INVSRCALPHA
,	D3DBLEND_DESTALPHA
,	D3DBLEND_INVDESTALPHA
,	D3DBLEND_SRCALPHASAT
};
D3DBLEND D3dEnum::DstBlendFactors[Pass::eDST_BLEND_COUNT] =
{	D3DBLEND_ZERO
,	D3DBLEND_ZERO
,	D3DBLEND_ONE
,	D3DBLEND_SRCCOLOR
,	D3DBLEND_INVSRCCOLOR
,	D3DBLEND_SRCALPHA
,	D3DBLEND_INVSRCALPHA
,	D3DBLEND_DESTALPHA
,	D3DBLEND_INVDESTALPHA
};
D3DFORMAT D3dEnum::PixelFormats[ePIXEL_FORMAT_COUNT] =
{	D3DFMT_A4L4
,	D3DFMT_A4L4
,	D3DFMT_L8
,	D3DFMT_A8L8
,	D3DFMT_A1R5G5B5
,	D3DFMT_A4R4G4B4
,	D3DFMT_R8G8B8
,	D3DFMT_A8R8G8B8
,	D3DFMT_DXT1
,	D3DFMT_DXT3
,	D3DFMT_DXT5
};

std::map <HRESULT, String> D3dEnum::MapErrors;

bool D3dEnum :: D3dCheckError( HRESULT p_hResult, String const & p_text, bool p_bThrow)
{
	if (MapErrors.size() == 0)
	{
		MapErrors[D3DERR_WRONGTEXTUREFORMAT       ] = cuT( "D3D : Wrong texture format");
		MapErrors[D3DERR_UNSUPPORTEDCOLOROPERATION] = cuT( "D3D : Unsupported color operation");
		MapErrors[D3DERR_UNSUPPORTEDCOLORARG      ] = cuT( "D3D : Unsupported color arg");
		MapErrors[D3DERR_UNSUPPORTEDALPHAOPERATION] = cuT( "D3D : Unsupported alpha operation");
		MapErrors[D3DERR_UNSUPPORTEDALPHAARG      ] = cuT( "D3D : Unsupported alpha arg");
		MapErrors[D3DERR_TOOMANYOPERATIONS        ] = cuT( "D3D : Too many operations");
		MapErrors[D3DERR_CONFLICTINGTEXTUREFILTER ] = cuT( "D3D : Conflicting texture filter");
		MapErrors[D3DERR_UNSUPPORTEDFACTORVALUE   ] = cuT( "D3D : Unsupported factor value");
		MapErrors[D3DERR_CONFLICTINGRENDERSTATE   ] = cuT( "D3D : Conflicting render state");
		MapErrors[D3DERR_UNSUPPORTEDTEXTUREFILTER ] = cuT( "D3D : Unsupported texture filter");
		MapErrors[D3DERR_CONFLICTINGTEXTUREPALETTE] = cuT( "D3D : Conflicting texture palette");
		MapErrors[D3DERR_DRIVERINTERNALERROR      ] = cuT( "D3D : Driver internal error");

		MapErrors[D3DERR_NOTFOUND                 ] = cuT( "D3D : Not found");
		MapErrors[D3DERR_MOREDATA                 ] = cuT( "D3D : More data");
		MapErrors[D3DERR_DEVICELOST               ] = cuT( "D3D : Device lost");
		MapErrors[D3DERR_DEVICENOTRESET           ] = cuT( "D3D : Device not reset");
		MapErrors[D3DERR_NOTAVAILABLE             ] = cuT( "D3D : Not available");
		MapErrors[D3DERR_OUTOFVIDEOMEMORY         ] = cuT( "D3D : Out of video memory");
		MapErrors[D3DERR_INVALIDDEVICE            ] = cuT( "D3D : Invalid device");
		MapErrors[D3DERR_INVALIDCALL              ] = cuT( "D3D : Invalid call");
		MapErrors[D3DERR_DRIVERINVALIDCALL        ] = cuT( "D3D : Driver invalid call");
		MapErrors[D3DERR_WASSTILLDRAWING          ] = cuT( "D3D : Was still drawing");
		MapErrors[D3DOK_NOAUTOGEN                 ] = cuT( "D3D : No autogen");
/*
		MapErrors[D3DERR_DEVICEREMOVED              ] = cuT( "D3D : Device removed");
		MapErrors[S_NOT_RESIDENT                    ] = cuT( "D3D : Not resident");
		MapErrors[S_RESIDENT_IN_SHARED_MEMORY       ] = cuT( "D3D : Resident in shared memory");
		MapErrors[S_PRESENT_MODE_CHANGED            ] = cuT( "D3D : Present mode changed");
		MapErrors[S_PRESENT_OCCLUDED                ] = cuT( "D3D : Present occluded");
		MapErrors[D3DERR_DEVICEHUNG                 ] = cuT( "D3D : Device hung");
		MapErrors[D3DERR_UNSUPPORTEDOVERLAY         ] = cuT( "D3D : Unsupported overlay");
		MapErrors[D3DERR_UNSUPPORTEDOVERLAYFORMAT   ] = cuT( "D3D : Unsupported overlay format");
		MapErrors[D3DERR_CANNOTPROTECTCONTENT       ] = cuT( "D3D : Cannot protect content");
		MapErrors[D3DERR_UNSUPPORTEDCRYPTO          ] = cuT( "D3D : Unsupported crypto");
		MapErrors[D3DERR_PRESENT_STATISTICS_DISJOINT] = cuT( "D3D : Present statistics disjoint");
*/
	}
	
	if (p_hResult != D3D_OK)
	{
		String l_strWinError = p_text + cuT( " :");
		LPTSTR l_szError = 0;
		std::map <HRESULT, String>::iterator l_it = MapErrors.find( p_hResult);

		if (l_it != MapErrors.end())
		{
			l_strWinError += " " + l_it->second;
		}

		if (::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), 0, (LPTSTR)& l_szError, 0, nullptr) != 0)
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

void D3dEnum :: CgCheckError( String const & p_strText)
{
	CGerror l_errorCode = cgGetError();

	if (l_errorCode != CG_NO_ERROR)
	{
		Logger::LogError( p_strText + cuT( " - ") + cgGetErrorString( l_errorCode), false);
	}
}

DWORD D3dEnum :: GetLockFlags( size_t p_uiFlags)
{
	DWORD l_ulLockFlags = 0;

	if (p_uiFlags & eLOCK_FLAG_READ_ONLY)
	{
		l_ulLockFlags = D3DLOCK_READONLY;
	}

	if (p_uiFlags & eLOCK_FLAG_WRITE_ONLY)
	{
		l_ulLockFlags = D3DLOCK_DISCARD;
	}

	return l_ulLockFlags;
}