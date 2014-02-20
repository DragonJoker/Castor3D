#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/Module_DxRender.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

#if defined __GNUG__
#   ifdef DEFINE_GUID
#       undef DEFINE_GUID
#   endif
#   define DEFINE_GUID( id, a, b, c, d, e, f, g, h, i, j, k ) const GUID id = { a, b, c, { d, e, f, g, h, i, j, k } }

	DEFINE_GUID( IID_ID3D11DeviceChild,			0x1841e5c8, 0x16b0, 0x489b, 0xbc, 0xc8, 0x44, 0xcf, 0xb0, 0xd5, 0xde, 0xae );
	DEFINE_GUID( IID_ID3D11DepthStencilState,	0x03823efb, 0x8d8f, 0x4e1c, 0x9a, 0xa2, 0xf6, 0x4b, 0xb2, 0xcb, 0xfd, 0xf1 );
	DEFINE_GUID( IID_ID3D11BlendState,			0x75b68faa, 0x347d, 0x4159, 0x8f, 0x45, 0xa0, 0x64, 0x0f, 0x01, 0xcd, 0x9a );
	DEFINE_GUID( IID_ID3D11RasterizerState,		0x9bb4ab81, 0xab1a, 0x4d8f, 0xb5, 0x06, 0xfc, 0x04, 0x20, 0x0b, 0x6e, 0xe7 );
	DEFINE_GUID( IID_ID3D11Resource,			0xdc8e63f3, 0xd12b, 0x4952, 0xb4, 0x7b, 0x5e, 0x45, 0x02, 0x6a, 0x86, 0x2d );
	DEFINE_GUID( IID_ID3D11Buffer,				0x48570b85, 0xd1ee, 0x4fcd, 0xa2, 0x50, 0xeb, 0x35, 0x07, 0x22, 0xb0, 0x37 );
	DEFINE_GUID( IID_ID3D11Texture1D,			0xf8fb5c27, 0xc6b3, 0x4f75, 0xa4, 0xc8, 0x43, 0x9a, 0xf2, 0xef, 0x56, 0x4c );
	DEFINE_GUID( IID_ID3D11Texture2D,			0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c );
	DEFINE_GUID( IID_ID3D11Texture3D,			0x037e866e, 0xf56d, 0x4357, 0xa8, 0xaf, 0x9d, 0xab, 0xbe, 0x6e, 0x25, 0x0e );
	DEFINE_GUID( IID_ID3D11View,				0x839d1216, 0xbb2e, 0x412b, 0xb7, 0xf4, 0xa9, 0xdb, 0xeb, 0xe0, 0x8e, 0xd1 );
	DEFINE_GUID( IID_ID3D11ShaderResourceView,	0xb0e06fe0, 0x8192, 0x4e1a, 0xb1, 0xca, 0x36, 0xd7, 0x41, 0x47, 0x10, 0xb2 );
	DEFINE_GUID( IID_ID3D11RenderTargetView,	0xdfdba067, 0x0b8d, 0x4865, 0x87, 0x5b, 0xd7, 0xb4, 0x51, 0x6c, 0xc1, 0x64 );
	DEFINE_GUID( IID_ID3D11DepthStencilView,	0x9fdac92a, 0x1876, 0x48c3, 0xaf, 0xad, 0x25, 0xb9, 0x4f, 0x84, 0xa9, 0xb6 );
	DEFINE_GUID( IID_ID3D11UnorderedAccessView,	0x28acf509, 0x7f5c, 0x48f6, 0x86, 0x11, 0xf3, 0x16, 0x01, 0x0a, 0x63, 0x80 );
	DEFINE_GUID( IID_ID3D11VertexShader,		0x3b301d64, 0xd678, 0x4289, 0x88, 0x97, 0x22, 0xf8, 0x92, 0x8b, 0x72, 0xf3 );
	DEFINE_GUID( IID_ID3D11HullShader,			0x8e5c6061, 0x628a, 0x4c8e, 0x82, 0x64, 0xbb, 0xe4, 0x5c, 0xb3, 0xd5, 0xdd );
	DEFINE_GUID( IID_ID3D11DomainShader,		0xf582c508, 0x0f36, 0x490c, 0x99, 0x77, 0x31, 0xee, 0xce, 0x26, 0x8c, 0xfa );
	DEFINE_GUID( IID_ID3D11GeometryShader,		0x38325b96, 0xeffb, 0x4022, 0xba, 0x02, 0x2e, 0x79, 0x5b, 0x70, 0x27, 0x5c );
	DEFINE_GUID( IID_ID3D11PixelShader,			0xea82e40d, 0x51dc, 0x4f33, 0x93, 0xd4, 0xdb, 0x7c, 0x91, 0x25, 0xae, 0x8c );
	DEFINE_GUID( IID_ID3D11ComputeShader,		0x4f5b196e, 0xc2bd, 0x495e, 0xbd, 0x01, 0x1f, 0xde, 0xd3, 0x8e, 0x49, 0x69 );
	DEFINE_GUID( IID_ID3D11InputLayout,			0xe4819ddc, 0x4cf0, 0x4025, 0xbd, 0x26, 0x5d, 0xe8, 0x2a, 0x3e, 0x07, 0xb7 );
	DEFINE_GUID( IID_ID3D11SamplerState,		0xda6fea51, 0x564c, 0x4487, 0x98, 0x10, 0xf0, 0xd0, 0xf9, 0xb4, 0xe3, 0xa5 );
	DEFINE_GUID( IID_ID3D11Asynchronous,		0x4b35d0cd, 0x1e15, 0x4258, 0x9c, 0x98, 0x1b, 0x13, 0x33, 0xf6, 0xdd, 0x3b );
	DEFINE_GUID( IID_ID3D11Query,				0xd6c00747, 0x87b7, 0x425e, 0xb8, 0x4d, 0x44, 0xd1, 0x08, 0x56, 0x0a, 0xfd );
	DEFINE_GUID( IID_ID3D11Predicate,			0x9eb576dd, 0x9f77, 0x4d86, 0x81, 0xaa, 0x8b, 0xab, 0x5f, 0xe4, 0x90, 0xe2 );
	DEFINE_GUID( IID_ID3D11Counter,				0x6e8c49fb, 0xa371, 0x4770, 0xb4, 0x40, 0x29, 0x08, 0x60, 0x22, 0xb7, 0x41 );
	DEFINE_GUID( IID_ID3D11ClassInstance,		0xa6cd7faa, 0xb0b7, 0x4a2f, 0x94, 0x36, 0x86, 0x62, 0xa6, 0x57, 0x97, 0xcb );
	DEFINE_GUID( IID_ID3D11ClassLinkage,		0xddf57cba, 0x9543, 0x46e4, 0xa1, 0x2b, 0xf2, 0x07, 0xa0, 0xfe, 0x7f, 0xed );
	DEFINE_GUID( IID_ID3D11CommandList,			0xa24bc4d1, 0x769e, 0x43f7, 0x80, 0x13, 0x98, 0xff, 0x56, 0x6c, 0x18, 0xe2 );
	DEFINE_GUID( IID_ID3D11DeviceContext,		0xc0bfa96c, 0xe089, 0x44fb, 0x8e, 0xaf, 0x26, 0xf8, 0x79, 0x61, 0x90, 0xda );
	DEFINE_GUID( IID_ID3D11Device,				0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 );
	DEFINE_GUID( IID_IDXGIFactory,				0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69 );
#endif

String DirectX11::HlslStrings[] =
{	cuT( "[e00] GLSL is not available!")
,	cuT( "[e01] Not a valid program object!")
,	cuT( "[e02] Not a valid object!")
,	cuT( "[e03] Out of memory!")
,	cuT( "[e04] Unknown compiler error!")
,	cuT( "[e05] Linker log is not available!")
,	cuT( "[e06] Compiler log is not available!")
,	cuT( "[Empty]")
};

String DirectX11::HlslErrors[] =
{	cuT( "[500] Invalid Enum !")
,	cuT( "[501] Invalid Value !")
,	cuT( "[502] Invalid Operation !")
,	cuT( "[503] Stack Overflow !")
,	cuT( "[504] Stack Underflow !")
,	cuT( "[505] Out of memory !")
,	cuT( "[506] Table too large")
,	cuT( "[Empty] Unknown Error")
};

DWORD DirectX11::TextureDimensions[eTEXTURE_DIMENSION_COUNT] =
{	0
,	0
,	0
};

DWORD DirectX11::LightIndexes[eLIGHT_INDEXES_COUNT] =
{	0
,	1
,	2
,	3
,	4
,	5
,	6
,	7
};

DWORD DirectX11::Index[eATTACHMENT_POINT_COUNT] =
{	0xFFFFFFFF
,	0
,	1
,	2
,	3
,	4
,	5
,	6
,	7
,	8
,	9
,	10
,	11
,	12
,	13
,	14
,	15
,	0
,	0
};

DXGI_FORMAT DirectX11::PixelFormats[ePIXEL_FORMAT_COUNT] =
{	DXGI_FORMAT_R8_UNORM			//ePIXEL_FORMAT_L8
,	DXGI_FORMAT_R16_FLOAT			//ePIXEL_FORMAT_L16F32F
,	DXGI_FORMAT_R32_FLOAT			//ePIXEL_FORMAT_L32F
,	DXGI_FORMAT_R8G8_UNORM			//ePIXEL_FORMAT_A8L8
,	DXGI_FORMAT_R16G16_FLOAT		//ePIXEL_FORMAT_AL16F32F
,	DXGI_FORMAT_R32G32_FLOAT		//ePIXEL_FORMAT_AL32F
,	DXGI_FORMAT_B5G5R5A1_UNORM		//ePIXEL_FORMAT_A1R5G5B5
,	DXGI_FORMAT_UNKNOWN				//ePIXEL_FORMAT_A4R4G4B4
,	DXGI_FORMAT_B5G6R5_UNORM		//ePIXEL_FORMAT_R5G6B5
,	DXGI_FORMAT_UNKNOWN				//ePIXEL_FORMAT_R8G8B8
,	DXGI_FORMAT_R8G8B8A8_UNORM		//ePIXEL_FORMAT_A8R8G8B8
,	DXGI_FORMAT_UNKNOWN				//ePIXEL_FORMAT_RGB16F32F
,	DXGI_FORMAT_R16G16B16A16_FLOAT	//ePIXEL_FORMAT_ARGB16F32F
,	DXGI_FORMAT_R32G32B32_FLOAT		//ePIXEL_FORMAT_RGB32F
,	DXGI_FORMAT_R32G32B32A32_FLOAT	//ePIXEL_FORMAT_ARGB32F
,	DXGI_FORMAT_BC1_UNORM			//ePIXEL_FORMAT_DXTC1
,	DXGI_FORMAT_BC2_UNORM			//ePIXEL_FORMAT_DXTC3
,	DXGI_FORMAT_BC3_UNORM			//ePIXEL_FORMAT_DXTC5
,	DXGI_FORMAT_R8G8_B8G8_UNORM		//ePIXEL_FORMAT_YUY2
,	DXGI_FORMAT_D16_UNORM			//ePIXEL_FORMAT_DEPTH16
,	DXGI_FORMAT_D24_UNORM_S8_UINT	//ePIXEL_FORMAT_DEPTH24
,	DXGI_FORMAT_D24_UNORM_S8_UINT	//ePIXEL_FORMAT_DEPTH24S8
,	DXGI_FORMAT_D32_FLOAT			//ePIXEL_FORMAT_DEPTH32
,	DXGI_FORMAT_UNKNOWN				//ePIXEL_FORMAT_STENCIL1
,	DXGI_FORMAT_D24_UNORM_S8_UINT	//ePIXEL_FORMAT_STENCIL8
};

D3D11_TEXTURE_ADDRESS_MODE DirectX11::TextureWrapMode[eWRAP_MODE_COUNT] =
{	D3D11_TEXTURE_ADDRESS_WRAP
,	D3D11_TEXTURE_ADDRESS_CLAMP
,	D3D11_TEXTURE_ADDRESS_BORDER
,	D3D11_TEXTURE_ADDRESS_BORDER
};

DWORD DirectX11::TextureInterpolation[eINTERPOLATION_MODE_COUNT] =
{	D3D11_FILTER_TYPE_POINT
,	D3D11_FILTER_TYPE_POINT
,	D3D11_FILTER_TYPE_LINEAR
,	D3D11_FILTER_TYPE_LINEAR
};

D3D11_BLEND DirectX11::BlendFactors[eBLEND_COUNT] =
{	D3D11_BLEND_ZERO
,	D3D11_BLEND_ONE
,	D3D11_BLEND_SRC_COLOR
,	D3D11_BLEND_INV_SRC_COLOR
,	D3D11_BLEND_DEST_COLOR
,	D3D11_BLEND_INV_DEST_COLOR
,	D3D11_BLEND_SRC_ALPHA
,	D3D11_BLEND_INV_SRC_ALPHA
,	D3D11_BLEND_DEST_ALPHA
,	D3D11_BLEND_INV_DEST_ALPHA
,	D3D11_BLEND_BLEND_FACTOR
,	D3D11_BLEND_INV_BLEND_FACTOR
,	D3D11_BLEND_SRC_ALPHA_SAT
,	D3D11_BLEND_SRC1_COLOR
,	D3D11_BLEND_INV_SRC1_COLOR
,	D3D11_BLEND_SRC1_ALPHA
,	D3D11_BLEND_INV_SRC1_ALPHA
};

D3D11_BLEND_OP DirectX11::BlendOps[eBLEND_OP_COUNT] =
{	D3D11_BLEND_OP_ADD
,	D3D11_BLEND_OP_SUBTRACT
,	D3D11_BLEND_OP_REV_SUBTRACT
,	D3D11_BLEND_OP_MIN
,	D3D11_BLEND_OP_MAX
};

D3D11_PRIMITIVE_TOPOLOGY DirectX11::DrawTypes[eTOPOLOGY_COUNT] =
{	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST		// eTOPOLOGY_POINTS
,	D3D11_PRIMITIVE_TOPOLOGY_LINELIST		// eTOPOLOGY_LINES
,	D3D11_PRIMITIVE_TOPOLOGY_LINELIST		// eTOPOLOGY_LINE_LOOP
,	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP		// eTOPOLOGY_LINE_STRIP
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST	// eTOPOLOGY_TRIANGLES
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	// eTOPOLOGY_TRIANGLE_STRIPS
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST	// eTOPOLOGY_TRIANGLE_FAN
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST	// eTOPOLOGY_QUADS
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	// eTOPOLOGY_QUAD_STRIPS
,	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST	// eTOPOLOGY_POLYGON
};

D3D11_DEPTH_WRITE_MASK DirectX11::DepthMasks[eDEPTH_MASK_COUNT] =
{	D3D11_DEPTH_WRITE_MASK_ZERO
,	D3D11_DEPTH_WRITE_MASK_ALL
};

D3D11_COMPARISON_FUNC DirectX11::DepthFuncs[eDEPTH_FUNC_COUNT] =
{	D3D11_COMPARISON_NEVER			// eDEPTH_FUNC_NEVER
,	D3D11_COMPARISON_LESS			// eDEPTH_FUNC_LESS
,	D3D11_COMPARISON_EQUAL			// eDEPTH_FUNC_EQUAL
,	D3D11_COMPARISON_LESS_EQUAL		// eDEPTH_FUNC_LEQUAL
,	D3D11_COMPARISON_GREATER		// eDEPTH_FUNC_GREATER
,	D3D11_COMPARISON_NOT_EQUAL		// eDEPTH_FUNC_NOTEQUAL
,	D3D11_COMPARISON_GREATER_EQUAL	// eDEPTH_FUNC_GEQUAL
,	D3D11_COMPARISON_ALWAYS			// eDEPTH_FUNC_ALWAYS
};

D3D11_COMPARISON_FUNC DirectX11::StencilFuncs[eSTENCIL_FUNC_COUNT] =
{	D3D11_COMPARISON_NEVER			// eSTENCIL_FUNC_NEVER
,	D3D11_COMPARISON_LESS			// eSTENCIL_FUNC_LESS
,	D3D11_COMPARISON_EQUAL			// eSTENCIL_FUNC_EQUAL
,	D3D11_COMPARISON_LESS_EQUAL		// eSTENCIL_FUNC_LEQUAL
,	D3D11_COMPARISON_GREATER		// eSTENCIL_FUNC_GREATER
,	D3D11_COMPARISON_NOT_EQUAL		// eSTENCIL_FUNC_NOTEQUAL
,	D3D11_COMPARISON_GREATER_EQUAL	// eSTENCIL_FUNC_GEQUAL
,	D3D11_COMPARISON_ALWAYS			// eSTENCIL_FUNC_ALWAYS
};

D3D11_STENCIL_OP DirectX11::StencilOps[eSTENCIL_OP_COUNT] =
{	D3D11_STENCIL_OP_KEEP		// eSTENCIL_OP_KEEP
,	D3D11_STENCIL_OP_ZERO		// eSTENCIL_OP_ZERO
,	D3D11_STENCIL_OP_REPLACE	// eSTENCIL_OP_REPLACE
,	D3D11_STENCIL_OP_INCR		// eSTENCIL_OP_INCR
,	D3D11_STENCIL_OP_INCR_SAT	// eSTENCIL_OP_INCR_WRAP
,	D3D11_STENCIL_OP_DECR		// eSTENCIL_OP_DECR
,	D3D11_STENCIL_OP_DECR_SAT	// eSTENCIL_OP_DECR_WRAP
,	D3D11_STENCIL_OP_INVERT		// eSTENCIL_OP_INVERT
};

D3D11_FILL_MODE DirectX11::FillModes[3] =
{	D3D11_FILL_WIREFRAME
,	D3D11_FILL_WIREFRAME
,	D3D11_FILL_SOLID
};

D3D11_CULL_MODE DirectX11::CullModes[3] =
{	D3D11_CULL_NONE
,	D3D11_CULL_FRONT
,	D3D11_CULL_BACK
};

std::map< eBUFFER_ACCESS_TYPE,		D3D11_USAGE				> DirectX11::Usages;
std::map< eBUFFER_ACCESS_NATURE,	D3D11_CPU_ACCESS_FLAG	> DirectX11::Accesses;
std::map< HRESULT,					String					> DirectX11::MapErrors;

void DirectX11 :: Initialise()
{
	Usages[eBUFFER_ACCESS_TYPE_DYNAMIC] = D3D11_USAGE_DYNAMIC;
	Usages[eBUFFER_ACCESS_TYPE_STATIC] = D3D11_USAGE_IMMUTABLE;
	Usages[eBUFFER_ACCESS_TYPE_STREAM] = D3D11_USAGE_DYNAMIC;

	Accesses[eBUFFER_ACCESS_NATURE_READ] = D3D11_CPU_ACCESS_READ;
	Accesses[eBUFFER_ACCESS_NATURE_DRAW] = D3D11_CPU_ACCESS_WRITE;
	Accesses[eBUFFER_ACCESS_NATURE_COPY] = D3D11_CPU_ACCESS_FLAG( 0 );
}

void DirectX11 :: Cleanup()
{
	MapErrors.clear();
	Usages.clear();
}

bool DirectX11 :: CheckError( HRESULT p_hResult, String const & p_text, bool p_bThrow )
{
	if (p_hResult != S_OK)
	{
		String l_strWinError = p_text + cuT( " :");
		LPTSTR l_szError = 0;
		std::map <HRESULT, String>::iterator l_it = MapErrors.find( p_hResult);

		if (l_it != MapErrors.end())
		{
			l_strWinError += cuT( " " ) + l_it->second;
		}

		if (::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), 0, (LPTSTR)& l_szError, 0, nullptr) != 0)
		{
			l_strWinError += str_utils::from_str( " - Windows : " ) + l_szError;
			LocalFree( l_szError );
			str_utils::replace( l_strWinError, cuT( "\r" ), cuEmptyString );
			str_utils::replace( l_strWinError, cuT( "\n" ), cuEmptyString );
		}

		Logger::LogError( l_strWinError, p_bThrow);

		return false;
	}

	return true;
}

DWORD DirectX11 :: GetLockFlags( uint32_t p_uiFlags)
{
	DWORD l_ulLockFlags = 0;

	if (p_uiFlags & eLOCK_FLAG_READ_ONLY)
	{
		l_ulLockFlags |= D3D11_MAP_READ;
	}

	if (p_uiFlags & eLOCK_FLAG_WRITE_ONLY)
	{
		l_ulLockFlags |= D3D11_MAP_WRITE_DISCARD;
	}

	return l_ulLockFlags;
}

bool DirectX11 :: UnlockBuffer( ID3D11Buffer * p_pBuffer )
{
	ID3D11Device * l_pDevice;
	ID3D11DeviceContext * l_pDeviceContext;
	p_pBuffer->GetDevice( &l_pDevice );
	l_pDevice->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->Unmap( p_pBuffer, 0 );
	l_pDeviceContext->Release();
	l_pDevice->Release();
	return true;
}
