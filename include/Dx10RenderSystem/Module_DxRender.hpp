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
#ifndef ___Dx10_ModuleRender___
#define ___Dx10_ModuleRender___

#include "Dx10.hpp"

#ifdef _WIN32
#	ifdef Dx10RenderSystem_EXPORTS
#		define C3D_Dx10_API __declspec(dllexport)
#	else
#		define C3D_Dx10_API __declspec(dllimport)
#	endif
#else
#	define C3D_Dx10_API
#endif

#   ifndef NDEBUG
#		undef dxCheckError
#	    define dxCheckError( hr, txt )			Dx10Render::DirectX10::CheckError( hr, cuT( txt ), false )
#		define dxDebugName( obj, type )\
			if( obj )\
			{\
				char l_szName[1024] = { 0 };\
				uint64_t l_ui64Address = (uint64_t)obj;\
				sprintf( l_szName, "%20s [0x%016X]", #type, l_ui64Address );\
				obj->SetPrivateData( WKPDID_D3DDebugObjectName, UINT( strlen( l_szName ) - 1 ), l_szName );\
				Castor::Logger::LogDebug( l_szName );\
			}
#   else
#	    define dxCheckError( hr, txt )			SUCCEEDED( hr )
#		define dxDebugName( obj, txt )
#   endif

namespace Dx10Render
{
	template< typename T, class D3dBufferObject				> class DxVertexBufferObject;
	template< typename T									> class DxOneFrameVariable;
	template< typename T, uint32_t Count					> class DxPointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns	> class DxMatrixFrameVariable;
	class DxVertexBuffer;
	class DxNormalsBuffer;
	class DxTextureBuffer;
	class DxRenderSystem;
	class DxPlugin;
	class DxSubmeshRenderer;
	class DxTextureRenderer;
	class DxMaterialRenderer;
	class DxLightRenderer;
	class DxCameraRenderer;
	class DxWindowRenderer;
	class DxOverlayRenderer;
	class DxShaderObject;
	class DxShaderProgram;
	class DxFrameVariableBase;
	class DxFrameVariableBuffer;
	class DxPipeline;
	class DxContext;
	class DxRenderBuffer;
	class DxColourRenderBuffer;
	class DxDepthStencilRenderBuffer;
	class DxRenderBufferAttachment;
	class DxTextureAttachment;
	class DxFrameBuffer;
	class DxStaticTexture;
	class DxDynamicTexture;

	DECLARE_SMART_PTR( DxContext					);
	DECLARE_SMART_PTR( DxRenderBuffer				);
	DECLARE_SMART_PTR( DxColourRenderBuffer			);
	DECLARE_SMART_PTR( DxDepthStencilRenderBuffer	);
	DECLARE_SMART_PTR( DxRenderBufferAttachment		);
	DECLARE_SMART_PTR( DxTextureAttachment			);
	DECLARE_SMART_PTR( DxFrameBuffer				);
	DECLARE_SMART_PTR( DxStaticTexture				);
	DECLARE_SMART_PTR( DxDynamicTexture				);
	DECLARE_SMART_PTR( DxShaderProgram				);
	DECLARE_SMART_PTR( DxFrameVariableBuffer		);

	DECLARE_VECTOR( DxTextureAttachment*,			DxTextureAttachmentPtr		);
	DECLARE_VECTOR( DxRenderBufferAttachment*,		DxRenderBufferAttachmentPtr	);

	typedef std::shared_ptr<	DxVertexBuffer			>	DxVertexBufferPtr;
	typedef std::shared_ptr<	DxTextureBuffer			>	DxTextureBufferPtr;
	typedef std::shared_ptr<	DxShaderObject			>	DxShaderObjectPtr;
	typedef std::shared_ptr<	DxShaderProgram			>	DxShaderProgramPtr;
	typedef std::shared_ptr<	DxFrameVariableBase		>	DxFrameVariablePtr;

	typedef std::vector<	DxShaderProgramPtr						>	DxShaderProgramPtrArray;
	typedef std::vector<	DxShaderObjectPtr						>	DxShaderObjectPtrArray;
	typedef std::map<		Castor::String,		DxFrameVariablePtr	>	DxFrameVariablePtrStrMap;

	class DirectX10
	{
	private:
		static Castor::String														HlslStrings				[];
		static Castor::String														HlslErrors				[];
		static DWORD																TextureDimensions		[Castor3D::eTEXTURE_DIMENSION_COUNT];
		static DWORD																TextureInterpolation	[Castor3D::eINTERPOLATION_MODE_COUNT];
		static DWORD 																TextureArguments		[Castor3D::eBLEND_SOURCE_COUNT];
		static DWORD 																LightIndexes			[Castor3D::eLIGHT_INDEXES_COUNT];
		static DWORD																Index					[Castor3D::eATTACHMENT_POINT_COUNT];
		static DXGI_FORMAT															PixelFormats			[Castor::ePIXEL_FORMAT_COUNT];
		static D3D10_TEXTURE_ADDRESS_MODE											TextureWrapMode			[Castor3D::eWRAP_MODE_COUNT];
		static D3D10_BLEND 															BlendFactors			[Castor3D::eBLEND_COUNT];
		static D3D10_BLEND_OP														BlendOps				[Castor3D::eBLEND_OP_COUNT];
		static D3D10_PRIMITIVE_TOPOLOGY												DrawTypes				[Castor3D::eTOPOLOGY_COUNT];
		static D3D10_DEPTH_WRITE_MASK												DepthMasks				[Castor3D::eDEPTH_MASK_COUNT];
		static D3D10_COMPARISON_FUNC												DepthFuncs				[Castor3D::eDEPTH_FUNC_COUNT];
		static D3D10_COMPARISON_FUNC												StencilFuncs			[Castor3D::eSTENCIL_FUNC_COUNT];
		static D3D10_STENCIL_OP														StencilOps				[Castor3D::eSTENCIL_OP_COUNT];
		static D3D10_FILL_MODE														FillModes				[3];
		static D3D10_CULL_MODE														CullModes				[3];
		static std::map< Castor3D::eBUFFER_ACCESS_TYPE, D3D10_USAGE >				Usages;
		static std::map< Castor3D::eBUFFER_ACCESS_NATURE, D3D10_CPU_ACCESS_FLAG >	Accesses;
		static std::map< HRESULT, Castor::String >									MapErrors;

	public:
		DirectX10();
		~DirectX10();

		static void Initialise();
		static void Cleanup();

		// VBO
		static bool			SetVertexLayout			( ID3D10Device * pDevice, ID3D10InputLayout * pDecl );
		static bool			SetStreamSource			( ID3D10Device * pDevice, UINT StreamNumber, ID3D10Buffer * pStreamData, UINT OffsetInBytes, UINT Stride );
		static bool			SetIndices				( ID3D10Device * pDevice, ID3D10Buffer * pIndexData );
		static bool			UnlockBuffer			( ID3D10Buffer * p_pBuffer );
		static bool			DrawIndexedPrimitives	();
		template< typename T >
		static void LockBuffer( T *& p_pReturn, ID3D10Buffer * p_pBuffer, uint32_t CU_PARAM_UNUSED( p_uiOffset ), uint32_t CU_PARAM_UNUSED( p_uiSize ), uint32_t p_uiFlags )
		{
			HRESULT l_hr = p_pBuffer->Map( D3D10_MAP( DirectX10::GetLockFlags( p_uiFlags ) ), 0, reinterpret_cast< void** >( &p_pReturn ) );
			dxCheckError( l_hr, "ID3D10Buffer :: Lock" );
		}

		static bool									CheckError			( HRESULT p_hResult, Castor::String const & p_strText, bool p_bThrow );
		static	DWORD								GetLockFlags		( uint32_t p_uiFlags );
		static inline Castor::String				GetHlslErrorString	( int p_index)														{ return HlslStrings[p_index]; }
		static inline DWORD							Get					( Castor3D::eTEXTURE_TYPE p_index)									{ return TextureDimensions[p_index]; }
		static inline DWORD							Get					( Castor3D::eLIGHT_INDEXES p_uiIndex)								{ return LightIndexes[p_uiIndex]; }
		static inline DWORD							Get					( Castor3D::eINTERPOLATION_MODE p_eInterpolationMode)				{ return TextureInterpolation[p_eInterpolationMode]; }
		static inline DWORD							Get					( Castor3D::eBLEND_SOURCE p_eArgument)								{ return TextureArguments[p_eArgument]; }
		static inline DWORD							Get					( Castor3D::eATTACHMENT_POINT p_eAttach	)							{ return Index[p_eAttach]; }
		static inline DXGI_FORMAT					Get					( Castor::ePIXEL_FORMAT p_pixelFormat)								{ return PixelFormats[p_pixelFormat]; }
		static inline D3D10_TEXTURE_ADDRESS_MODE	Get					( Castor3D::eWRAP_MODE p_eWrapMode)									{ return TextureWrapMode[p_eWrapMode]; }
		static inline D3D10_BLEND					Get					( Castor3D::eBLEND p_eBlendFactor)									{ return BlendFactors[p_eBlendFactor]; }
		static inline D3D10_BLEND_OP				Get					( Castor3D::eBLEND_OP p_eOp )										{ return BlendOps[p_eOp]; }
		static inline D3D10_PRIMITIVE_TOPOLOGY		Get					( Castor3D::eTOPOLOGY p_index)								{ return DrawTypes[p_index]; }
		static inline D3D10_USAGE					Get					( Castor3D::eBUFFER_ACCESS_TYPE p_eType )							{ return Usages[p_eType]; }
		static inline D3D10_DEPTH_WRITE_MASK		Get					( Castor3D::eDEPTH_MASK p_eMask )									{ return DepthMasks[p_eMask]; }
		static inline D3D10_COMPARISON_FUNC			Get					( Castor3D::eDEPTH_FUNC p_eFunc )									{ return DepthFuncs[p_eFunc]; }
		static inline D3D10_COMPARISON_FUNC			Get					( Castor3D::eSTENCIL_FUNC p_eFunc )									{ return StencilFuncs[p_eFunc]; }
		static inline D3D10_STENCIL_OP				Get					( Castor3D::eSTENCIL_OP p_eOp )										{ return StencilOps[p_eOp]; }
		static inline D3D10_FILL_MODE				Get					( Castor3D::eFILL_MODE p_eMode )									{ return FillModes[p_eMode]; }
		static inline D3D10_CULL_MODE				Get					( Castor3D::eFACE p_eFace )											{ return CullModes[p_eFace]; }
		static inline UINT GetCpuAccessFlags( DWORD dwAccess )
		{
			UINT l_uiReturn = 0;

			if( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_STATIC )
			{
				l_uiReturn = 0;
			}
			else if( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_STREAM )
			{
				if( dwAccess & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_uiReturn = D3D10_CPU_ACCESS_WRITE;
				}
				else
				{
					l_uiReturn = 0;
				}
			}
			else if( dwAccess & Castor3D::eBUFFER_ACCESS_TYPE_DYNAMIC )
			{
				if( dwAccess & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_uiReturn = D3D10_CPU_ACCESS_WRITE;
				}
				else
				{
					l_uiReturn = 0;
				}
			}

			return l_uiReturn;
		}
	};

	template< typename T > void SafeRelease( T *& p_ptReleasable )
	{
		if( p_ptReleasable )
		{
			p_ptReleasable->Release();
			p_ptReleasable = NULL;
		}
	}
}

#endif
