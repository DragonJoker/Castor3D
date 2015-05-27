#include "Dx9RenderSystemPrerequisites.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	String DirectX9::HlslStrings[] =
	{
		cuT( "[e00] GLSL is not available!" ),
		cuT( "[e01] Not a valid program object!" ),
		cuT( "[e02] Not a valid object!" ),
		cuT( "[e03] Out of memory!" ),
		cuT( "[e04] Unknown compiler error!" ),
		cuT( "[e05] Linker log is not available!" ),
		cuT( "[e06] Compiler log is not available!" ),
		cuT( "[Empty]" ),
	};
	String DirectX9::HlslErrors[] =
	{
		cuT( "[500] Invalid Enum !" ),
		cuT( "[501] Invalid Value !" ),
		cuT( "[502] Invalid Operation !" ),
		cuT( "[503] Stack Overflow !" ),
		cuT( "[504] Stack Underflow !" ),
		cuT( "[505] Out of memory !" ),
		cuT( "[506] Table too large" ),
		cuT( "[Empty] Unknown Error" ),
	};
	D3DPRIMITIVETYPE DirectX9::DrawTypes[eTOPOLOGY_COUNT] =
	{
		D3DPT_POINTLIST,
		D3DPT_LINELIST,
		D3DPT_LINELIST,
		D3DPT_LINESTRIP,
		D3DPT_TRIANGLELIST,
		D3DPT_TRIANGLESTRIP,
		D3DPT_TRIANGLEFAN,
		D3DPT_LINELIST,
		D3DPT_LINELIST,
		D3DPT_LINELIST,
	};
	DWORD DirectX9::TextureDimensions[eTEXTURE_DIMENSION_COUNT] =
	{
		0,
		0,
		0,
	};
	D3DCMPFUNC DirectX9::AlphaFuncs[eALPHA_FUNC_COUNT] =
	{
		D3DCMP_ALWAYS,
		D3DCMP_LESS,
		D3DCMP_LESSEQUAL,
		D3DCMP_EQUAL,
		D3DCMP_NOTEQUAL,
		D3DCMP_GREATEREQUAL,
		D3DCMP_GREATER,
		D3DCMP_NEVER,
	};
	D3DTEXTUREADDRESS DirectX9::TextureWrapMode[eWRAP_MODE_COUNT] =
	{
		D3DTADDRESS_WRAP,
		D3DTADDRESS_CLAMP,
		D3DTADDRESS_BORDER,
		D3DTADDRESS_BORDER,
	};
	DWORD DirectX9::TextureInterpolation[eINTERPOLATION_MODE_COUNT] =
	{
		D3DTEXF_NONE,
		D3DTEXF_POINT,
		D3DTEXF_LINEAR,
		D3DTEXF_LINEAR,
	};
	D3DTEXTUREOP DirectX9::RgbBlendFuncs[eRGB_BLEND_FUNC_COUNT] =
	{
		D3DTOP_MODULATE,
		D3DTOP_SELECTARG1,
		D3DTOP_ADD,
		D3DTOP_ADDSIGNED,
		D3DTOP_MODULATE,
		D3DTOP_ADDSMOOTH,
		D3DTOP_SUBTRACT,
		D3DTOP_DOTPRODUCT3,
		D3DTOP_DOTPRODUCT3,
	};
	D3DTEXTUREOP DirectX9::AlphaBlendFuncs[eALPHA_BLEND_FUNC_COUNT] =
	{
		D3DTOP_MODULATE,
		D3DTOP_SELECTARG1,
		D3DTOP_ADD,
		D3DTOP_ADDSIGNED,
		D3DTOP_MODULATE,
		D3DTOP_ADDSMOOTH,
		D3DTOP_SUBTRACT,
	};
	DWORD DirectX9::TextureArguments[eBLEND_SOURCE_COUNT] =
	{
		D3DTA_TEXTURE,
		D3DTA_TEXTURE,
		D3DTA_TEXTURE,
		D3DTA_TEXTURE,
		D3DTA_TEXTURE,
		D3DTA_CONSTANT,
		D3DTA_DIFFUSE,
		D3DTA_TFACTOR,
	};
	DWORD DirectX9::LightIndexes[eLIGHT_INDEXES_COUNT] =
	{
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
	};
	D3DLIGHTTYPE DirectX9::LightTypes[eLIGHT_TYPE_COUNT] =
	{
		D3DLIGHT_DIRECTIONAL,
		D3DLIGHT_POINT,
		D3DLIGHT_SPOT,
	};
	D3DBLEND DirectX9::BlendFactors[eBLEND_COUNT] =
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
		D3DBLEND_SRCALPHASAT,
	};
	D3DBLENDOP DirectX9::BlendOps[eBLEND_OP_COUNT] =
	{
		D3DBLENDOP_ADD,
		D3DBLENDOP_SUBTRACT,
		D3DBLENDOP_REVSUBTRACT,
		D3DBLENDOP_MIN,
		D3DBLENDOP_MAX,
	};
	D3DFORMAT DirectX9::PixelFormats[ePIXEL_FORMAT_COUNT] =
	{
		D3DFMT_L8,				//ePIXEL_FORMAT_L8
		D3DFMT_R32F,			//ePIXEL_FORMAT_L16F32F
		D3DFMT_R32F,			//ePIXEL_FORMAT_L32F
		D3DFMT_A8L8,			//ePIXEL_FORMAT_A8L8
		D3DFMT_G32R32F,			//ePIXEL_FORMAT_AL16F32F
		D3DFMT_G32R32F,			//ePIXEL_FORMAT_AL32F
		D3DFMT_X1R5G5B5,		//ePIXEL_FORMAT_A1R5G5B5
		D3DFMT_X4R4G4B4,		//ePIXEL_FORMAT_A4R4G4B4
		D3DFMT_R5G6B5,			//ePIXEL_FORMAT_R5G6B5
		D3DFMT_UNKNOWN,			//ePIXEL_FORMAT_R8G8B8
		D3DFMT_X8R8G8B8,		//ePIXEL_FORMAT_A8R8G8B8
		D3DFMT_UNKNOWN,			//ePIXEL_FORMAT_RGB16F32F
		D3DFMT_A32B32G32R32F,	//ePIXEL_FORMAT_ARGB16F32F
		D3DFMT_UNKNOWN,			//ePIXEL_FORMAT_RGB32F
		D3DFMT_A32B32G32R32F,	//ePIXEL_FORMAT_ARGB32F
		D3DFMT_DXT1,			//ePIXEL_FORMAT_DXTC1
		D3DFMT_DXT3,			//ePIXEL_FORMAT_DXTC3
		D3DFMT_DXT5,			//ePIXEL_FORMAT_DXTC5
		D3DFMT_YUY2,			//ePIXEL_FORMAT_YUY2
		D3DFMT_D16,				//ePIXEL_FORMAT_DEPTH16
		D3DFMT_D24X8,			//ePIXEL_FORMAT_DEPTH24
		D3DFMT_D24X8,			//ePIXEL_FORMAT_DEPTH24S8
		D3DFMT_D32,				//ePIXEL_FORMAT_DEPTH32
		D3DFMT_D15S1,			//ePIXEL_FORMAT_STENCIL1
		D3DFMT_D24S8,			//ePIXEL_FORMAT_STENCIL8
	};
	DWORD DirectX9::Index[eATTACHMENT_POINT_COUNT] =
	{
		0xFFFFFFFF,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15,
		0,
		0,
	};
	D3DCULL DirectX9::CullModes[eFACE_COUNT] =
	{
		D3DCULL_NONE,
		D3DCULL_CCW,
		D3DCULL_CW,
		D3DCULL( 0 ),
	};
	D3DFILLMODE DirectX9::FillModes[3] =
	{
		D3DFILL_POINT,
		D3DFILL_WIREFRAME,
		D3DFILL_SOLID,
	};
	D3DCMPFUNC DirectX9::DepthFuncs[eDEPTH_FUNC_COUNT] =
	{
		D3DCMP_NEVER,			//!< eDEPTH_FUNC_NEVER
		D3DCMP_LESS,			//!< eDEPTH_FUNC_LESS
		D3DCMP_EQUAL,			//!< eDEPTH_FUNC_EQUAL
		D3DCMP_LESSEQUAL,		//!< eDEPTH_FUNC_LEQUAL
		D3DCMP_GREATER,			//!< eDEPTH_FUNC_GREATER
		D3DCMP_NOTEQUAL,		//!< eDEPTH_FUNC_NOTEQUAL
		D3DCMP_GREATEREQUAL,	//!< eDEPTH_FUNC_GEQUAL
		D3DCMP_ALWAYS,			//!< eDEPTH_FUNC_ALWAYS
	};
	D3DSTENCILOP DirectX9::StencilOps[eSTENCIL_OP_COUNT] =
	{
		D3DSTENCILOP_KEEP,		//!< eSTENCIL_OP_KEEP
		D3DSTENCILOP_ZERO,		//!< eSTENCIL_OP_ZERO
		D3DSTENCILOP_REPLACE,	//!< eSTENCIL_OP_REPLACE
		D3DSTENCILOP_INCR,		//!< eSTENCIL_OP_INCR
		D3DSTENCILOP_INCRSAT,	//!< eSTENCIL_OP_INCR_WRAP
		D3DSTENCILOP_DECR,		//!< eSTENCIL_OP_DECR
		D3DSTENCILOP_DECRSAT,	//!< eSTENCIL_OP_DECR_WRAP
		D3DSTENCILOP_INVERT,	//!< eSTENCIL_OP_INVERT
	};
	D3DCMPFUNC DirectX9::StencilFuncs[eSTENCIL_FUNC_COUNT] =
	{
		D3DCMP_NEVER,			//!< eSTENCIL_FUNC_NEVER
		D3DCMP_LESS,			//!< eSTENCIL_FUNC_LESS
		D3DCMP_EQUAL,			//!< eSTENCIL_FUNC_EQUAL
		D3DCMP_LESSEQUAL,		//!< eSTENCIL_FUNC_LEQUAL
		D3DCMP_GREATER,			//!< eSTENCIL_FUNC_GREATER
		D3DCMP_NOTEQUAL,		//!< eSTENCIL_FUNC_NOTEQUAL
		D3DCMP_GREATEREQUAL,	//!< eSTENCIL_FUNC_GEQUAL
		D3DCMP_ALWAYS,			//!< eSTENCIL_FUNC_ALWAYS
	};
	D3DRenderState DirectX9::Tweaks[eTWEAK_COUNT] =
	{
		{ D3DRS_ZENABLE,			TRUE,			FALSE			},// eTWEAK_DEPTH_TEST
		{ D3DRS_STENCILENABLE,		TRUE,			FALSE			},// eTWEAK_STENCIL_TEST
		{ D3DRS_ALPHABLENDENABLE,	TRUE,			FALSE			},// eTWEAK_BLEND
		{ D3DRS_LIGHTING,			TRUE,			FALSE			},// eTWEAK_LIGHTING
		{ D3DRS_ALPHATESTENABLE,	TRUE,			FALSE			},// eTWEAK_ALPHA_TEST
		{ D3DRS_CULLMODE,			D3DCULL_CCW,	D3DCULL_NONE	},// eTWEAK_CULLING
		{ D3DRS_DITHERENABLE,		TRUE,			FALSE			},// eTWEAK_DITHERING
		{ D3DRS_FOGENABLE,			TRUE,			FALSE			},// eTWEAK_FOG
		{ D3DRS_ZWRITEENABLE,		TRUE,			FALSE			},// eTWEAK_DEPTH_WRITE
	};

	std::map <HRESULT, String> DirectX9::MapErrors;

	bool DirectX9::CheckError( HRESULT p_hResult, String const & p_text )
	{
		if ( MapErrors.size() == 0 )
		{
			MapErrors[D3DERR_WRONGTEXTUREFORMAT				] = cuT( "D3D : Wrong texture format"	);
			MapErrors[D3DERR_UNSUPPORTEDCOLOROPERATION		] = cuT( "D3D : Unsupported color operation"	);
			MapErrors[D3DERR_UNSUPPORTEDCOLORARG			] = cuT( "D3D : Unsupported color arg"	);
			MapErrors[D3DERR_UNSUPPORTEDALPHAOPERATION		] = cuT( "D3D : Unsupported alpha operation"	);
			MapErrors[D3DERR_UNSUPPORTEDALPHAARG			] = cuT( "D3D : Unsupported alpha arg"	);
			MapErrors[D3DERR_TOOMANYOPERATIONS				] = cuT( "D3D : Too many operations"	);
			MapErrors[D3DERR_CONFLICTINGTEXTUREFILTER		] = cuT( "D3D : Conflicting texture filter"	);
			MapErrors[D3DERR_UNSUPPORTEDFACTORVALUE			] = cuT( "D3D : Unsupported factor value"	);
			MapErrors[D3DERR_CONFLICTINGRENDERSTATE			] = cuT( "D3D : Conflicting render state"	);
			MapErrors[D3DERR_UNSUPPORTEDTEXTUREFILTER		] = cuT( "D3D : Unsupported texture filter"	);
			MapErrors[D3DERR_CONFLICTINGTEXTUREPALETTE		] = cuT( "D3D : Conflicting texture palette"	);
			MapErrors[D3DERR_DRIVERINTERNALERROR			] = cuT( "D3D : Driver internal error"	);
			MapErrors[D3DERR_NOTFOUND						] = cuT( "D3D : Not found"	);
			MapErrors[D3DERR_MOREDATA						] = cuT( "D3D : More data"	);
			MapErrors[D3DERR_DEVICELOST						] = cuT( "D3D : Device lost"	);
			MapErrors[D3DERR_DEVICENOTRESET					] = cuT( "D3D : Device not reset"	);
			MapErrors[D3DERR_NOTAVAILABLE					] = cuT( "D3D : Not available"	);
			MapErrors[D3DERR_OUTOFVIDEOMEMORY				] = cuT( "D3D : Out of video memory"	);
			MapErrors[D3DERR_INVALIDDEVICE					] = cuT( "D3D : Invalid device"	);
			MapErrors[D3DERR_INVALIDCALL					] = cuT( "D3D : Invalid call"	);
			MapErrors[D3DERR_DRIVERINVALIDCALL				] = cuT( "D3D : Driver invalid call"	);
			MapErrors[D3DERR_WASSTILLDRAWING				] = cuT( "D3D : Was still drawing"	);
			MapErrors[D3DOK_NOAUTOGEN						] = cuT( "D3D : No autogen"	);
			//MapErrors[D3DERR_DEVICEREMOVED					] = cuT( "D3D : Device removed" );
			//MapErrors[S_NOT_RESIDENT						] = cuT( "D3D : Not resident" );
			//MapErrors[S_RESIDENT_IN_SHARED_MEMORY			] = cuT( "D3D : Resident in shared memory" );
			//MapErrors[S_PRESENT_MODE_CHANGED				] = cuT( "D3D : Present mode changed" );
			//MapErrors[S_PRESENT_OCCLUDED					] = cuT( "D3D : Present occluded" );
			//MapErrors[D3DERR_DEVICEHUNG						] = cuT( "D3D : Device hung" );
			//MapErrors[D3DERR_UNSUPPORTEDOVERLAY				] = cuT( "D3D : Unsupported overlay" );
			//MapErrors[D3DERR_UNSUPPORTEDOVERLAYFORMAT		] = cuT( "D3D : Unsupported overlay format" );
			//MapErrors[D3DERR_CANNOTPROTECTCONTENT			] = cuT( "D3D : Cannot protect content" );
			//MapErrors[D3DERR_UNSUPPORTEDCRYPTO				] = cuT( "D3D : Unsupported crypto" );
			//MapErrors[D3DERR_PRESENT_STATISTICS_DISJOINT	] = cuT( "D3D : Present statistics disjoint" );
		}

		if ( p_hResult != S_OK )
		{
			String l_strWinError = p_text + cuT( " :" );
			LPTSTR l_szError = 0;
			std::map <HRESULT, String>::iterator l_it = MapErrors.find( p_hResult );

			if ( l_it != MapErrors.end() )
			{
				l_strWinError += cuT( " " ) + l_it->second;
			}

			if ( ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), 0, ( LPTSTR )& l_szError, 0, nullptr ) != 0 )
			{
				l_strWinError += str_utils::from_str( " - Windows : " ) + l_szError;
				LocalFree( l_szError );
				str_utils::replace( l_strWinError, cuT( "\r" ), cuEmptyString );
				str_utils::replace( l_strWinError, cuT( "\n" ), cuEmptyString );
			}

			Logger::LogError( l_strWinError );
			return false;
		}

		return true;
	}

	DWORD DirectX9::GetLockFlags( uint32_t p_uiFlags )
	{
		DWORD l_ulLockFlags = 0;

		if ( p_uiFlags & eLOCK_FLAG_READ_ONLY )
		{
			l_ulLockFlags = D3DLOCK_READONLY;
		}

		if ( p_uiFlags & eLOCK_FLAG_WRITE_ONLY )
		{
			l_ulLockFlags = 0;//D3DLOCK_DISCARD;
		}

		return l_ulLockFlags;
	}
}
