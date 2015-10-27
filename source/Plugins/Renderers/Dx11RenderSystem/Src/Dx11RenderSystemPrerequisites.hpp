/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___DX11_RENDER_SYSTEM_PREREQUISITES_H___
#define ___DX11_RENDER_SYSTEM_PREREQUISITES_H___

#if !defined _MSC_VER
#   define __in
#   define __inout
#   define __out
#   define __in_opt
#   define __inout_opt
#   define __out_opt
#   define __deref_out_range(A,B)
#   define __in_bcount(a)
#   define __in_ecount(a)
#   define __in_bcount_opt(a)
#   define __in_ecount_opt(a)
#   define __out_bcount(a)
#   define __out_ecount(a)
#   define __out_bcount_opt(a)
#   define __out_ecount_opt(a)
#   define __out_ecount_part_opt(a,b)
#   define __deref_out
#   define __deref_out_opt
#   define __RPC__deref_out
#   define _COM_Outptr_
#   define _Out_
#   define _Out_opt_
#   define _In_
#   define _In_opt_
#   define _In_z_
#   define _In_opt_z_
#   define _In_reads_bytes_(x)
#   define _Out_writes_bytes_(x)
#   define _In_reads_(x)
#   define _Out_writes_(x)
#   define _Outptr_result_z_
#   define _Outptr_result_opt_z_
#   define _Outptr_
#   define _Outptr_opt_
#   define _Inout_
#   define _Inout_opt_
#   define _Outptr_result_maybenull_
#   define _Outptr_opt_result_maybenull_
#   define WINAPI_INLINE

typedef uint8_t UINT8;
// to prevent includes of DxGI.h
#	define __WIDL_DXGI_H
#endif

#include <Castor3DPrerequisites.hpp>

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4005 )	// macro redefinition
#endif

#include <Windows.h>
#include <initguid.h>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3dx11.h>
#include <DXGI.h>

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#include <cstdint>

#if !defined _MSC_VER

#	ifndef __uuidof
#		define __uuidof(x) IID_##x
#	endif

#   ifdef DECLARE_GUID
#       undef DECLARE_GUID
#   endif
#   define DECLARE_GUID( id, a, b, c, d, e, f, g, h, i, j, k ) extern const GUID id;

DECLARE_GUID( IID_ID3D11DeviceChild,			0x1841e5c8, 0x16b0, 0x489b, 0xbc, 0xc8, 0x44, 0xcf, 0xb0, 0xd5, 0xde, 0xae );
DECLARE_GUID( IID_ID3D11DepthStencilState,		0x03823efb, 0x8d8f, 0x4e1c, 0x9a, 0xa2, 0xf6, 0x4b, 0xb2, 0xcb, 0xfd, 0xf1 );
DECLARE_GUID( IID_ID3D11BlendState,				0x75b68faa, 0x347d, 0x4159, 0x8f, 0x45, 0xa0, 0x64, 0x0f, 0x01, 0xcd, 0x9a );
DECLARE_GUID( IID_ID3D11RasterizerState,		0x9bb4ab81, 0xab1a, 0x4d8f, 0xb5, 0x06, 0xfc, 0x04, 0x20, 0x0b, 0x6e, 0xe7 );
DECLARE_GUID( IID_ID3D11Resource,				0xdc8e63f3, 0xd12b, 0x4952, 0xb4, 0x7b, 0x5e, 0x45, 0x02, 0x6a, 0x86, 0x2d );
DECLARE_GUID( IID_ID3D11Buffer,					0x48570b85, 0xd1ee, 0x4fcd, 0xa2, 0x50, 0xeb, 0x35, 0x07, 0x22, 0xb0, 0x37 );
DECLARE_GUID( IID_ID3D11Texture1D,				0xf8fb5c27, 0xc6b3, 0x4f75, 0xa4, 0xc8, 0x43, 0x9a, 0xf2, 0xef, 0x56, 0x4c );
DECLARE_GUID( IID_ID3D11Texture2D,				0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c );
DECLARE_GUID( IID_ID3D11Texture3D,				0x037e866e, 0xf56d, 0x4357, 0xa8, 0xaf, 0x9d, 0xab, 0xbe, 0x6e, 0x25, 0x0e );
DECLARE_GUID( IID_ID3D11View,					0x839d1216, 0xbb2e, 0x412b, 0xb7, 0xf4, 0xa9, 0xdb, 0xeb, 0xe0, 0x8e, 0xd1 );
DECLARE_GUID( IID_ID3D11ShaderResourceView,		0xb0e06fe0, 0x8192, 0x4e1a, 0xb1, 0xca, 0x36, 0xd7, 0x41, 0x47, 0x10, 0xb2 );
DECLARE_GUID( IID_ID3D11RenderTargetView,		0xdfdba067, 0x0b8d, 0x4865, 0x87, 0x5b, 0xd7, 0xb4, 0x51, 0x6c, 0xc1, 0x64 );
DECLARE_GUID( IID_ID3D11DepthStencilView,		0x9fdac92a, 0x1876, 0x48c3, 0xaf, 0xad, 0x25, 0xb9, 0x4f, 0x84, 0xa9, 0xb6 );
DECLARE_GUID( IID_ID3D11UnorderedAccessView,	0x28acf509, 0x7f5c, 0x48f6, 0x86, 0x11, 0xf3, 0x16, 0x01, 0x0a, 0x63, 0x80 );
DECLARE_GUID( IID_ID3D11VertexShader,			0x3b301d64, 0xd678, 0x4289, 0x88, 0x97, 0x22, 0xf8, 0x92, 0x8b, 0x72, 0xf3 );
DECLARE_GUID( IID_ID3D11HullShader,				0x8e5c6061, 0x628a, 0x4c8e, 0x82, 0x64, 0xbb, 0xe4, 0x5c, 0xb3, 0xd5, 0xdd );
DECLARE_GUID( IID_ID3D11DomainShader,			0xf582c508, 0x0f36, 0x490c, 0x99, 0x77, 0x31, 0xee, 0xce, 0x26, 0x8c, 0xfa );
DECLARE_GUID( IID_ID3D11GeometryShader,			0x38325b96, 0xeffb, 0x4022, 0xba, 0x02, 0x2e, 0x79, 0x5b, 0x70, 0x27, 0x5c );
DECLARE_GUID( IID_ID3D11PixelShader,			0xea82e40d, 0x51dc, 0x4f33, 0x93, 0xd4, 0xdb, 0x7c, 0x91, 0x25, 0xae, 0x8c );
DECLARE_GUID( IID_ID3D11ComputeShader,			0x4f5b196e, 0xc2bd, 0x495e, 0xbd, 0x01, 0x1f, 0xde, 0xd3, 0x8e, 0x49, 0x69 );
DECLARE_GUID( IID_ID3D11InputLayout,			0xe4819ddc, 0x4cf0, 0x4025, 0xbd, 0x26, 0x5d, 0xe8, 0x2a, 0x3e, 0x07, 0xb7 );
DECLARE_GUID( IID_ID3D11SamplerState,			0xda6fea51, 0x564c, 0x4487, 0x98, 0x10, 0xf0, 0xd0, 0xf9, 0xb4, 0xe3, 0xa5 );
DECLARE_GUID( IID_ID3D11Asynchronous,			0x4b35d0cd, 0x1e15, 0x4258, 0x9c, 0x98, 0x1b, 0x13, 0x33, 0xf6, 0xdd, 0x3b );
DECLARE_GUID( IID_ID3D11Query,					0xd6c00747, 0x87b7, 0x425e, 0xb8, 0x4d, 0x44, 0xd1, 0x08, 0x56, 0x0a, 0xfd );
DECLARE_GUID( IID_ID3D11Predicate,				0x9eb576dd, 0x9f77, 0x4d86, 0x81, 0xaa, 0x8b, 0xab, 0x5f, 0xe4, 0x90, 0xe2 );
DECLARE_GUID( IID_ID3D11Counter,				0x6e8c49fb, 0xa371, 0x4770, 0xb4, 0x40, 0x29, 0x08, 0x60, 0x22, 0xb7, 0x41 );
DECLARE_GUID( IID_ID3D11ClassInstance,			0xa6cd7faa, 0xb0b7, 0x4a2f, 0x94, 0x36, 0x86, 0x62, 0xa6, 0x57, 0x97, 0xcb );
DECLARE_GUID( IID_ID3D11ClassLinkage,			0xddf57cba, 0x9543, 0x46e4, 0xa1, 0x2b, 0xf2, 0x07, 0xa0, 0xfe, 0x7f, 0xed );
DECLARE_GUID( IID_ID3D11CommandList,			0xa24bc4d1, 0x769e, 0x43f7, 0x80, 0x13, 0x98, 0xff, 0x56, 0x6c, 0x18, 0xe2 );
DECLARE_GUID( IID_ID3D11DeviceContext,			0xc0bfa96c, 0xe089, 0x44fb, 0x8e, 0xaf, 0x26, 0xf8, 0x79, 0x61, 0x90, 0xda );
DECLARE_GUID( IID_ID3D11Device,					0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 );
DECLARE_GUID( IID_IDXGIFactory,					0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69 );
#endif

#if !defined( NDEBUG )
#	if !defined( D3D_DEBUG_INFO )
#		define D3D_DEBUG_INFO
#	endif
#	define DX_DEBUG_RT 1
#else
#	define DX_DEBUG_RT 0
#endif

#ifdef _WIN32
#	ifdef Dx11RenderSystem_EXPORTS
#		define C3D_Dx11_API __declspec(dllexport)
#	else
#		define C3D_Dx11_API __declspec(dllimport)
#	endif
#else
#	define C3D_Dx11_API
#endif

#	if !defined( NDEBUG )
#		define dxCheckError( hr, txt ) Dx11Render::DirectX11::CheckError( hr, cuT( txt ) )
#		define dxTrack( rs, obj, type ) rs->Track( obj, #type, __FILE__, __LINE__ )
#		define dxUntrack( rs, obj ) rs->Untrack( obj )
#	else
#		define dxCheckError( hr, txt ) SUCCEEDED( hr )
#		define dxTrack( rs, obj, txt )
#		define dxUntrack( rs, obj )
#	endif

namespace Dx11Render
{
	using Castor::real;

	template< typename T, uint32_t Count > class DxVertexAttribs;
	template< typename T, class D3dBufferObject > class DxVertexBufferObject;
	template< typename T, uint32_t Count > class DxVertexAttribsBuffer;
	template< typename T > class DxOneFrameVariable;
	template< typename T, uint32_t Count > class DxPointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns > class DxMatrixFrameVariable;
	class DxVertexBuffer;
	class DxNormalsBuffer;
	class DxTextureBuffer;
	class DxRenderSystem;
	class DxPlugin;
	class DxRenderWindow;
	class DxOverlayRenderer;
	class DxShaderObject;
	class DxShaderProgram;
	class DxFrameVariableBase;
	class DxFrameVariableBuffer;
	class DxPipelineImpl;
	class DxContext;
	class DxRenderBuffer;
	class DxColourRenderBuffer;
	class DxDepthStencilRenderBuffer;
	class DxRenderBufferAttachment;
	class DxTextureAttachment;
	class DxFrameBuffer;
	class DxStaticTexture;
	class DxDynamicTexture;

	DECLARE_SMART_PTR( DxContext );
	DECLARE_SMART_PTR( DxRenderBuffer );
	DECLARE_SMART_PTR( DxColourRenderBuffer );
	DECLARE_SMART_PTR( DxDepthStencilRenderBuffer );
	DECLARE_SMART_PTR( DxRenderBufferAttachment );
	DECLARE_SMART_PTR( DxTextureAttachment );
	DECLARE_SMART_PTR( DxFrameBuffer );
	DECLARE_SMART_PTR( DxStaticTexture );
	DECLARE_SMART_PTR( DxDynamicTexture );
	DECLARE_SMART_PTR( DxShaderProgram );
	DECLARE_SMART_PTR( DxFrameVariableBuffer );

	DECLARE_VECTOR( DxTextureAttachment *, DxTextureAttachmentPtr );
	DECLARE_VECTOR( DxRenderBufferAttachment *, DxRenderBufferAttachmentPtr );

	typedef DxVertexAttribs< real, 3 > DxVertexAttribs3r;
	typedef DxVertexAttribs< real, 2 > DxVertexAttribs2r;
	typedef DxVertexAttribs< int, 3 > DxVertexAttribs3i;
	typedef DxVertexAttribs< int, 2 > DxVertexAttribs2i;
	typedef DxVertexAttribsBuffer< real, 3 > DxVertexAttribsBuffer3r;
	typedef DxVertexAttribsBuffer< real, 2 > DxVertexAttribsBuffer2r;
	typedef DxVertexAttribsBuffer< int, 3 > DxVertexAttribsBuffer3i;
	typedef DxVertexAttribsBuffer< int, 2 > DxVertexAttribsBuffer2i;

	typedef std::shared_ptr< DxVertexBuffer > DxVertexBufferPtr;
	typedef std::shared_ptr< DxNormalsBuffer > DxNormalsBufferPtr;
	typedef std::shared_ptr< DxTextureBuffer > DxTextureBufferPtr;
	typedef std::shared_ptr< DxVertexAttribs3r > DxVertexAttribs3rPtr;
	typedef std::shared_ptr< DxVertexAttribs2r > DxVertexAttribs2rPtr;
	typedef std::shared_ptr< DxVertexAttribs3i > DxVertexAttribs3iPtr;
	typedef std::shared_ptr< DxVertexAttribs2i > DxVertexAttribs2iPtr;
	typedef std::shared_ptr< DxVertexAttribsBuffer3r > DxVertexAttribsBuffer3rPtr;
	typedef std::shared_ptr< DxVertexAttribsBuffer2r > DxVertexAttribsBuffer2rPtr;
	typedef std::shared_ptr< DxVertexAttribsBuffer3i > DxVertexAttribsBuffer3iPtr;
	typedef std::shared_ptr< DxVertexAttribsBuffer2i > DxVertexAttribsBuffer2iPtr;
	typedef std::shared_ptr< DxShaderObject > DxShaderObjectPtr;
	typedef std::shared_ptr< DxShaderProgram > DxShaderProgramPtr;
	typedef std::shared_ptr< DxFrameVariableBase > DxFrameVariablePtr;

	typedef std::vector< DxShaderProgramPtr > DxShaderProgramPtrArray;
	typedef std::vector< DxShaderObjectPtr > DxShaderObjectPtrArray;
	typedef std::map< Castor::String, DxFrameVariablePtr > DxFrameVariablePtrStrMap;

	class DirectX11
	{
	private:
		static DWORD TextureDimensions[Castor3D::eTEXTURE_TYPE_COUNT];
		static DWORD TextureInterpolation[Castor3D::eINTERPOLATION_MODE_COUNT];
		static DWORD TextureArguments[Castor3D::eBLEND_SOURCE_COUNT];
		static DWORD LightIndexes[Castor3D::eLIGHT_INDEXES_COUNT];
		static DWORD Index[Castor3D::eATTACHMENT_POINT_COUNT];
		static DXGI_FORMAT PixelFormats[Castor::ePIXEL_FORMAT_COUNT];
		static DXGI_FORMAT BoundPixelFormats[Castor::ePIXEL_FORMAT_COUNT];
		static D3D11_TEXTURE_ADDRESS_MODE TextureWrapMode[Castor3D::eWRAP_MODE_COUNT];
		static D3D11_BLEND BlendFactors[Castor3D::eBLEND_COUNT];
		static D3D11_BLEND_OP BlendOps[Castor3D::eBLEND_OP_COUNT];
		static D3D11_PRIMITIVE_TOPOLOGY DrawTypes[Castor3D::eTOPOLOGY_COUNT];
		static D3D11_DEPTH_WRITE_MASK DepthMasks[Castor3D::eWRITING_MASK_COUNT];
		static D3D11_COMPARISON_FUNC DepthFuncs[Castor3D::eDEPTH_FUNC_COUNT];
		static D3D11_COMPARISON_FUNC StencilFuncs[Castor3D::eSTENCIL_FUNC_COUNT];
		static D3D11_STENCIL_OP StencilOps[Castor3D::eSTENCIL_OP_COUNT];
		static D3D11_FILL_MODE FillModes[3];
		static D3D11_CULL_MODE CullModes[3];
		static std::map< Castor3D::eBUFFER_ACCESS_TYPE, D3D11_USAGE > Usages;
		static std::map< Castor3D::eBUFFER_ACCESS_NATURE, D3D11_CPU_ACCESS_FLAG > Accesses;
		static std::map< HRESULT, Castor::String > MapErrors;

	public:
		DirectX11();
		~DirectX11();

		static void Initialise();
		static void Cleanup();

		// VBO
		static bool SetVertexLayout( ID3D11Device * pDevice, ID3D11InputLayout * pDecl );
		static bool SetStreamSource( ID3D11Device * pDevice, UINT StreamNumber, ID3D11Buffer * pStreamData, UINT OffsetInBytes, UINT Stride );
		static bool SetIndices( ID3D11Device * pDevice, ID3D11Buffer * pIndexData );
		static bool UnlockBuffer( ID3D11Buffer * p_pBuffer );
		static bool DrawIndexedPrimitives();
		template< typename T >
		static void LockBuffer( T *& p_pReturn, ID3D11Buffer * p_pBuffer, uint32_t CU_PARAM_UNUSED( p_uiOffset ), uint32_t CU_PARAM_UNUSED( p_uiSize ), uint32_t p_uiFlags )
		{
			ID3D11Device * l_pDevice;
			ID3D11DeviceContext * l_deviceContext;
			D3D11_MAPPED_SUBRESOURCE l_mappedResource;
			p_pBuffer->GetDevice( &l_pDevice );
			l_pDevice->GetImmediateContext( &l_deviceContext );
			HRESULT l_hr = l_deviceContext->Map( p_pBuffer, 0, D3D11_MAP( DirectX11::GetLockFlags( p_uiFlags ) ), 0, &l_mappedResource );
			l_deviceContext->Release();
			l_pDevice->Release();

			if ( dxCheckError( l_hr, "ID3D11Buffer::Lock" ) )
			{
				p_pReturn = static_cast< T * >( l_mappedResource.pData );
			}
		}

		static bool CheckError( HRESULT p_hResult, Castor::String const & p_strText );
		static	DWORD GetLockFlags( uint32_t p_uiFlags );
		static inline DWORD Get( Castor3D::eTEXTURE_TYPE p_index )
		{
			return TextureDimensions[p_index];
		}
		static inline DWORD Get( Castor3D::eLIGHT_INDEXES p_index )
		{
			return LightIndexes[p_index];
		}
		static inline DWORD Get( Castor3D::eINTERPOLATION_MODE p_eInterpolationMode )
		{
			return TextureInterpolation[p_eInterpolationMode];
		}
		static inline DWORD Get( Castor3D::eBLEND_SOURCE p_eArgument )
		{
			return TextureArguments[p_eArgument];
		}
		static inline DWORD Get( Castor3D::eATTACHMENT_POINT p_eAttach	)
		{
			return Index[p_eAttach];
		}
		static inline DXGI_FORMAT Get( Castor::ePIXEL_FORMAT p_pixelFormat )
		{
			return PixelFormats[p_pixelFormat];
		}
		static inline DXGI_FORMAT GetBound( Castor::ePIXEL_FORMAT p_pixelFormat )
		{
			return BoundPixelFormats[p_pixelFormat];
		}
		static inline D3D11_TEXTURE_ADDRESS_MODE Get( Castor3D::eWRAP_MODE p_eWrapMode )
		{
			return TextureWrapMode[p_eWrapMode];
		}
		static inline D3D11_BLEND Get( Castor3D::eBLEND p_eBlendFactor )
		{
			return BlendFactors[p_eBlendFactor];
		}
		static inline D3D11_BLEND_OP Get( Castor3D::eBLEND_OP p_eOp )
		{
			return BlendOps[p_eOp];
		}
		static inline D3D11_PRIMITIVE_TOPOLOGY Get( Castor3D::eTOPOLOGY p_index )
		{
			return DrawTypes[p_index];
		}
		static inline D3D11_USAGE Get( Castor3D::eBUFFER_ACCESS_TYPE p_type )
		{
			return Usages[p_type];
		}
		static inline D3D11_DEPTH_WRITE_MASK Get( Castor3D::eWRITING_MASK p_eMask )
		{
			return DepthMasks[p_eMask];
		}
		static inline D3D11_COMPARISON_FUNC Get( Castor3D::eDEPTH_FUNC p_eFunc )
		{
			return DepthFuncs[p_eFunc];
		}
		static inline D3D11_COMPARISON_FUNC Get( Castor3D::eSTENCIL_FUNC p_eFunc )
		{
			return StencilFuncs[p_eFunc];
		}
		static inline D3D11_STENCIL_OP Get( Castor3D::eSTENCIL_OP p_eOp )
		{
			return StencilOps[p_eOp];
		}
		static inline D3D11_FILL_MODE Get( Castor3D::eFILL_MODE p_mode )
		{
			return FillModes[p_mode];
		}
		static inline D3D11_CULL_MODE Get( Castor3D::eFACE p_eFace )
		{
			return CullModes[p_eFace];
		}
		static inline UINT GetCpuAccessFlags( DWORD dwAccess )
		{
			UINT l_uiReturn = 0;

			if ( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_STATIC )
			{
				l_uiReturn = 0;
			}
			else if ( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_STREAM )
			{
				if ( dwAccess & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_uiReturn = D3D11_CPU_ACCESS_WRITE;
				}
				else
				{
					l_uiReturn = 0;
				}
			}
			else if ( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_DYNAMIC )
			{
				if ( dwAccess & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_uiReturn = D3D11_CPU_ACCESS_WRITE;
				}
				else
				{
					l_uiReturn = 0;
				}
			}

			return l_uiReturn;
		}
		static inline D3D11_MAP GetBufferMapMode( Castor3D::eBUFFER_ACCESS_NATURE eNature )
		{
			D3D11_MAP l_eReturn;

			if ( eNature == Castor3D::eBUFFER_ACCESS_NATURE_READ )
			{
				l_eReturn = D3D11_MAP_READ;
			}
			else
			{
				l_eReturn = D3D11_MAP_WRITE;
			}

			return l_eReturn;
		}
	};

	template< typename T > void ReleaseTracked( Castor3D::RenderSystem * p_rs, T *& p_ptReleasable )
	{
		if ( p_ptReleasable )
		{
			dxUntrack( static_cast< DxRenderSystem * >( p_rs ), p_ptReleasable );
			p_ptReleasable->Release();
			p_ptReleasable = NULL;
		}
	}

	template< typename T > void SafeRelease( T *& p_ptReleasable )
	{
		if ( p_ptReleasable )
		{
			p_ptReleasable->Release();
			p_ptReleasable = NULL;
		}
	}
}

#endif
