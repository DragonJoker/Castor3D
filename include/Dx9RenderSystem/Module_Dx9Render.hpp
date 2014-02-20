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
#ifndef ___Dx9_ModuleRender___
#define ___Dx9_ModuleRender___

#if !defined( NDEBUG )
#	define D3D_DEBUG_INFO
#endif

#include <D3D9.h>
#include <D3DX9.h>
#include <D3DX9Effect.h>

#ifdef _WIN32
#	ifdef Dx9RenderSystem_EXPORTS
#		define C3D_Dx9_API __declspec(dllexport)
#	else
#		define C3D_Dx9_API __declspec(dllimport)
#	endif
#else
#	define C3D_Dx9_API
#endif

namespace Dx9Render
{
	template< typename T, class D3dBufferObject					> class DxVertexBufferObject;
	template< typename T, std::size_t Count						> class DxVertexAttribsBuffer;
	template< typename T										> class DxOneFrameVariable;
	template< typename T, std::size_t Count						> class DxPointFrameVariable;
	template< typename T, std::size_t Rows, std::size_t Columns	> class DxMatrixFrameVariable;
	class DxVertexBuffer;
	class DxTextureBuffer;
	class DxRenderSystem;
	class DxSubmeshRenderer;
	class DxTextureRenderer;
	class DxLightRenderer;
	class DxCameraRenderer;
	class DxWindowRenderer;
	class DxOverlayRenderer;
	class DxShaderObject;
	class DxShaderProgram;
	class DxFrameVariableBase;
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

	DECLARE_VECTOR( DxTextureAttachment*,		DxTextureAttachmentSPtr		);
	DECLARE_VECTOR( DxRenderBufferAttachment*,	DxRenderBufferAttachmentSPtr	);

	typedef std::shared_ptr<	DxVertexBuffer			>	DxVertexBufferPtr;
	typedef std::shared_ptr<	DxTextureBuffer			>	DxTextureBufferPtr;
	typedef std::shared_ptr<	DxShaderObject			>	DxShaderObjectPtr;
	typedef std::shared_ptr<	DxShaderProgram			>	DxShaderProgramPtr;
	typedef std::shared_ptr<	DxFrameVariableBase		>	Dx9FrameVariablePtr;

	typedef std::vector<	DxShaderProgramPtr							>	DxShaderProgramPtrArray;
	typedef std::vector<	DxShaderObjectPtr							>	DxShaderObjectPtrArray;
	typedef std::map<		Castor::String,		Dx9FrameVariablePtr		>	Dx9FrameVariablePtrStrMap;

	struct D3DRenderState
	{
		D3DRENDERSTATETYPE	m_eType;
		DWORD				m_dwEnable;
		DWORD				m_dwDisable;
	};

	class DirectX9
	{
	private:
		static Castor::String						HlslStrings				[];
		static Castor::String						HlslErrors				[];
		static D3DPRIMITIVETYPE						DrawTypes				[Castor3D::eTOPOLOGY_COUNT];
		static DWORD								TextureDimensions		[Castor3D::eTEXTURE_DIMENSION_COUNT];
		static D3DCMPFUNC							AlphaFuncs				[Castor3D::eALPHA_FUNC_COUNT];
		static D3DTEXTUREADDRESS					TextureWrapMode			[Castor3D::eWRAP_MODE_COUNT];
		static DWORD								TextureInterpolation	[Castor3D::eINTERPOLATION_MODE_COUNT];
		static D3DTEXTUREOP							RgbBlendFuncs			[Castor3D::eRGB_BLEND_FUNC_COUNT];
		static D3DTEXTUREOP							AlphaBlendFuncs			[Castor3D::eALPHA_BLEND_FUNC_COUNT];
		static DWORD 								TextureArguments		[Castor3D::eBLEND_SOURCE_COUNT];
		static DWORD 								LightIndexes			[Castor3D::eLIGHT_INDEXES_COUNT];
		static D3DLIGHTTYPE							LightTypes				[Castor3D::eLIGHT_TYPE_COUNT];
		static D3DBLEND 							BlendFactors			[Castor3D::eBLEND_COUNT];
		static D3DBLENDOP							BlendOps				[Castor3D::eBLEND_OP_COUNT];
		static D3DFORMAT							PixelFormats			[Castor::ePIXEL_FORMAT_COUNT];
		static D3DCULL								CullModes				[Castor3D::eFACE_COUNT];
		static D3DFILLMODE							FillModes				[3];
		static D3DCMPFUNC							DepthFuncs				[Castor3D::eDEPTH_FUNC_COUNT];
		static D3DSTENCILOP							StencilOps				[Castor3D::eSTENCIL_OP_COUNT];
		static D3DCMPFUNC							StencilFuncs			[Castor3D::eSTENCIL_FUNC_COUNT];
		static DWORD								Index					[Castor3D::eATTACHMENT_POINT_COUNT];
		static D3DRenderState						Tweaks					[Castor3D::eTWEAK_COUNT];
		static std::map< HRESULT, Castor::String >	MapErrors;

	public:
		DirectX9();
		~DirectX9();

		static bool							CheckError			( HRESULT p_hResult, Castor::String const & p_strText, bool p_bThrow );
		static	DWORD						GetLockFlags		( uint32_t p_uiFlags );
		static inline Castor::String		GetHlslErrorString	( int p_index)											{ return HlslStrings[p_index]; }
		static inline DWORD					GetDrawType			( Castor3D::eTOPOLOGY p_index)					{ return DrawTypes[p_index]; }
		static inline D3DPRIMITIVETYPE		Get					( Castor3D::eTOPOLOGY p_index)					{ return DrawTypes[p_index]; }
		static inline DWORD					Get					( Castor3D::eTEXTURE_TYPE p_index)						{ return TextureDimensions[p_index]; }
		static inline DWORD					Get					( Castor3D::eLIGHT_INDEXES p_uiIndex)					{ return LightIndexes[p_uiIndex]; }
		static inline D3DCMPFUNC			Get					( Castor3D::eALPHA_FUNC p_eAlphaFunc)					{ return AlphaFuncs[p_eAlphaFunc]; }
		static inline D3DTEXTUREADDRESS		Get					( Castor3D::eWRAP_MODE p_eWrapMode)						{ return TextureWrapMode[p_eWrapMode]; }
		static inline DWORD					Get					( Castor3D::eINTERPOLATION_MODE p_eInterpolationMode)	{ return TextureInterpolation[p_eInterpolationMode]; }
		static inline D3DTEXTUREOP			Get					( Castor3D::eRGB_BLEND_FUNC p_eMode)					{ return RgbBlendFuncs[p_eMode]; }
		static inline D3DTEXTUREOP			Get					( Castor3D::eALPHA_BLEND_FUNC p_eMode)					{ return AlphaBlendFuncs[p_eMode]; }
		static inline DWORD					Get					( Castor3D::eBLEND_SOURCE p_eArgument)					{ return TextureArguments[p_eArgument]; }
		static inline D3DBLEND				Get					( Castor3D::eBLEND p_eBlendFactor)						{ return BlendFactors[p_eBlendFactor]; }
		static inline D3DBLENDOP			Get					( Castor3D::eBLEND_OP p_eBlendOp )						{ return BlendOps[p_eBlendOp]; }
		static inline D3DFORMAT				Get					( Castor::ePIXEL_FORMAT p_pixelFormat)					{ return PixelFormats[p_pixelFormat]; }
		static inline D3DLIGHTTYPE			Get					( Castor3D::eLIGHT_TYPE p_eLightType)					{ return LightTypes[p_eLightType]; }
		static inline D3DCULL				Get					( Castor3D::eFACE p_eFace )								{ return CullModes[p_eFace]; }
		static inline D3DFILLMODE			Get					( Castor3D::eFILL_MODE p_eFill )						{ return FillModes[p_eFill]; }
		static inline D3DCMPFUNC			Get					( Castor3D::eDEPTH_FUNC p_eFunc )						{ return DepthFuncs[p_eFunc]; }
		static inline D3DSTENCILOP			Get					( Castor3D::eSTENCIL_OP p_eOp )							{ return StencilOps[p_eOp]; }
		static inline D3DCMPFUNC			Get					( Castor3D::eSTENCIL_FUNC p_eFunc )						{ return StencilFuncs[p_eFunc]; }
		static inline DWORD					Get					( Castor3D::eATTACHMENT_POINT p_eAttach	)				{ return Index[p_eAttach]; }
		static inline DWORD					GetComponents		( uint32_t p_uiTargets )								{ return (p_uiTargets & Castor3D::eBUFFER_COMPONENT_COLOUR ? D3DCLEAR_TARGET : 0) | (p_uiTargets & Castor3D::eBUFFER_COMPONENT_DEPTH ? D3DCLEAR_ZBUFFER : 0) | (p_uiTargets & Castor3D::eBUFFER_COMPONENT_STENCIL ? D3DCLEAR_STENCIL : 0); }
		static inline D3DRenderState		Get					( Castor3D::eTWEAK p_eTweak )							{ return Tweaks[p_eTweak]; }
		static inline DWORD GetBufferFlags( std::size_t p_uiFlags )
		{
			DWORD l_dwReturn = 0;

			if( p_uiFlags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
			{
				l_dwReturn |= D3DUSAGE_WRITEONLY;
			}

			if( p_uiFlags & Castor3D::eBUFFER_ACCESS_TYPE_DYNAMIC )
			{
				l_dwReturn |= D3DUSAGE_DYNAMIC;
			}

			return l_dwReturn;
		}
	};

#	undef dxCheckError
#   ifndef NDEBUG
#	    define dxCheckError( hr, txt )			DirectX9::CheckError( hr, cuT( txt ), false )
#   else
#	    define dxCheckError( hr, txt )			SUCCEEDED( hr )
#   endif

	template< class T > void SafeRelease( T *& p_tReleasable )
	{
		if( p_tReleasable )
		{
			p_tReleasable->Release();
			p_tReleasable = NULL;
		}
	}
}

#endif
