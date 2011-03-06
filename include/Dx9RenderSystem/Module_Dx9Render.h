/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include <D3D9.h>

#ifdef _WIN32
#	ifdef Dx9RenderSystem_EXPORTS
#		define C3D_Dx9_API __declspec(dllexport)
#	else
#		define C3D_Dx9_API __declspec(dllimport)
#	endif
#else
#	define C3D_Dx9_API
#endif

#define GL_INVALID_INDEX -1

namespace Castor3D
{
	class RenderWindow;
	class Geometry;

	template <typename T, size_t Count> class Dx9VertexAttribs;
	template <typename T, class D3dBufferObject> class Dx9VertexBufferObject;
	template <typename T, size_t Count> class Dx9VertexAttribsBuffer;
	class Dx9VertexBuffer;
	class Dx9NormalsBuffer;
	class Dx9TextureBuffer;
	class Dx9VertexInfosBufferObject;
	class Dx9RenderSystem;
	class Dx9Plugin;
	class Dx9SubmeshRenderer;
	class Dx9MeshRenderer;
	class Dx9GeometryRenderer;
	class Dx9TextureEnvironmentRenderer;
	class Dx9TextureRenderer;
	class Dx9MaterialRenderer;
	class Dx9LightRenderer;
	class Dx9CameraRenderer;
	class Dx9WindowRenderer;
	class Dx9OverlayRenderer;
	class Dx9ShaderObject;
	class Dx9ShaderProgram;
	class Dx9VertexShader;
	class Dx9FragmentShader;
	class Dx9GeometryShader;
	class Dx9FrameVariableBase;
	class Dx9IntFrameVariable;
	class Dx9RealFrameVariable;
	template <typename T> class Dx9OneFrameVariable;
	template <typename T, size_t Count> class Dx9PointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class Dx9MatrixFrameVariable;
	class CgDx9ShaderProgram;
	class CgDx9VertexShader;
	class CgDx9FragmentShader;
	class CgDx9GeometryShader;
	class CgDx9FrameVariableBase;
	class CgDx9IntFrameVariable;
	class CgDx9RealFrameVariable;
	template <typename T> class CgDx9OneFrameVariable;
	template <typename T, size_t Count> class CgDx9PointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class CgDx9MatrixFrameVariable;
	class Dx9Pipeline;
	class Dx9Context;

	typedef Dx9VertexAttribs<real, 3		>	Dx9VertexAttribs3r;
	typedef Dx9VertexAttribs<real, 2		>	Dx9VertexAttribs2r;
	typedef Dx9VertexAttribs<int, 3			>	Dx9VertexAttribs3i;
	typedef Dx9VertexAttribs<int, 2			>	Dx9VertexAttribs2i;
	typedef Dx9VertexAttribsBuffer<real, 3	>	Dx9VertexAttribsBuffer3r;
	typedef Dx9VertexAttribsBuffer<real, 2	>	Dx9VertexAttribsBuffer2r;
	typedef Dx9VertexAttribsBuffer<int, 3	>	Dx9VertexAttribsBuffer3i;
	typedef Dx9VertexAttribsBuffer<int, 2	>	Dx9VertexAttribsBuffer2i;

	typedef shared_ptr<	Dx9VertexBuffer				>	Dx9VertexBufferPtr;
	typedef shared_ptr<	Dx9NormalsBuffer			>	Dx9NormalsBufferPtr;
	typedef shared_ptr<	Dx9TextureBuffer			>	Dx9TextureBufferPtr;
	typedef shared_ptr<	Dx9VertexAttribs3r			>	GLVertexAttribs3rPtr;
	typedef shared_ptr<	Dx9VertexAttribs2r			>	Dx9VertexAttribs2rPtr;
	typedef shared_ptr<	Dx9VertexAttribs3i			>	Dx9VertexAttribs3iPtr;
	typedef shared_ptr<	Dx9VertexAttribs2i			>	Dx9VertexAttribs2iPtr;
	typedef shared_ptr<	Dx9VertexAttribsBuffer3r	>	Dx9VertexAttribsBuffer3rPtr;
	typedef shared_ptr<	Dx9VertexAttribsBuffer2r	>	Dx9VertexAttribsBuffer2rPtr;
	typedef shared_ptr<	Dx9VertexAttribsBuffer3i	>	Dx9VertexAttribsBuffer3iPtr;
	typedef shared_ptr<	Dx9VertexAttribsBuffer2i	>	Dx9VertexAttribsBuffer2iPtr;
	typedef shared_ptr<	Dx9ShaderObject				>	Dx9ShaderObjectPtr;
	typedef shared_ptr<	Dx9ShaderProgram			>	Dx9ShaderProgramPtr;
	typedef shared_ptr<	Dx9VertexShader				>	Dx9VertexShaderPtr;
	typedef shared_ptr<	Dx9FragmentShader			>	Dx9FragmentShaderPtr;
	typedef shared_ptr<	Dx9GeometryShader			>	Dx9GeometryShaderPtr;
	typedef shared_ptr<	Dx9FrameVariableBase		>	Dx9FrameVariablePtr;
	typedef shared_ptr<	CgDx9ShaderProgram			>	CgDx9ShaderProgramPtr;
	typedef shared_ptr<	CgDx9VertexShader			>	CgDx9VertexShaderPtr;
	typedef shared_ptr<	CgDx9FragmentShader			>	CgDx9FragmentShaderPtr;
	typedef shared_ptr<	CgDx9GeometryShader			>	CgDx9GeometryShaderPtr;
	typedef shared_ptr<	CgDx9FrameVariableBase		>	CgDx9FrameVariablePtr;

	typedef Container<		Dx9ShaderProgramPtr							>::Vector	Dx9ShaderProgramPtrArray;
	typedef Container<		CgDx9ShaderProgramPtr						>::Vector	CgDx9ShaderProgramPtrArray;
	typedef Container<		Dx9ShaderObjectPtr							>::Vector	Dx9ShaderObjectPtrArray;
	typedef KeyedContainer<	String,				Dx9FrameVariablePtr		>::Map		Dx9FrameVariablePtrStrMap;
	typedef KeyedContainer<	String,				CgDx9FrameVariablePtr	>::Map		CgDx9FrameVariablePtrStrMap;

	class D3dEnum
	{
	private:
		static String						HlslStrings				[];
		static String						HlslErrors				[];
		static D3DPRIMITIVETYPE				DrawTypes				[eNbDrawTypes];
		static DWORD						EnvironmentModes		[TextureEnvironment::eNbEnvModes];
		static D3DTEXTUREOP					RgbCombinations			[TextureEnvironment::eNbRgbCombinations];
		static DWORD						RgbOperands				[TextureEnvironment::eNbRgbOperands];
		static D3DTEXTUREOP					AlphaCombinations		[TextureEnvironment::eNbAlphaCombinations];
		static DWORD						AlphaOperands			[TextureEnvironment::eNbAlphaOperands];
		static DWORD						CombinationSources		[TextureEnvironment::eNbCombinationSources];
		static DWORD						TextureDimensions		[TextureUnit::eNbTextureDimensions];
		static D3DCMPFUNC					AlphaFuncs				[TextureUnit::eNbAlphaFuncs];
		static D3DTEXTUREADDRESS			TextureWrapMode			[TextureUnit::eNbWrapModes];
		static DWORD						TextureInterpolation	[TextureUnit::eNbInterpolationModes];
		static D3DTEXTUREOP					TextureRgbModes			[TextureUnit::eNbTextureRgbModes];
		static D3DTEXTUREOP					TextureAlphaModes		[TextureUnit::eNbTextureAlphaModes];
		static DWORD 						TextureArguments		[TextureUnit::eNbTextureArguments];
		static DWORD 						LightIndexes			[8];
		static D3DLIGHTTYPE					LightTypes				[Light::eNbLightTypes];
		static D3DBLEND 					SrcBlendFactors			[Pass::eNbSrcBlendFactors];
		static D3DBLEND 					DstBlendFactors			[Pass::eNbDstBlendFactors];
		static D3DFORMAT					PixelFormats			[eNbPixelFormats];
		static std::map <HRESULT, String>	MapErrors;

	public:
		static bool						D3dCheckError		( HRESULT p_hResult, const String & p_strText, bool p_bThrow);
		static void						CgCheckError		( const String & p_strText);
		static String					GetHlslErrorString	( int p_index);
		static DWORD					GetDrawType			( ePRIMITIVE_TYPE p_index);
		static D3DPRIMITIVETYPE			Get					( ePRIMITIVE_TYPE p_index);
		static DWORD					Get					( TextureEnvironment::eMODE p_index);
		static D3DTEXTUREOP				Get					( TextureEnvironment::eRGB_COMBINATION p_index);
		static DWORD					Get					( TextureEnvironment::eRGB_OPERAND p_index);
		static D3DTEXTUREOP				Get					( TextureEnvironment::eALPHA_COMBINATION p_index);
		static DWORD					Get					( TextureEnvironment::eALPHA_OPERAND p_index);
		static DWORD					Get					( TextureEnvironment::eCOMBINATION_SOURCE p_index);
		static DWORD					Get					( TextureUnit::eDIMENSION p_index);
		static DWORD					Get					( eLIGHT_INDEXES p_uiIndex);
		static D3DCMPFUNC				Get					( TextureUnit::eALPHA_FUNC p_eAlphaFunc);
		static D3DTEXTUREADDRESS		Get					( TextureUnit::eWRAP_MODE p_eWrapMode);
		static DWORD					Get					( TextureUnit::eINTERPOLATION_MODE p_eInterpolationMode);
		static D3DTEXTUREOP				Get					( TextureUnit::eTEXTURE_RGB_MODE p_eMode);
		static D3DTEXTUREOP				Get					( TextureUnit::eTEXTURE_ALPHA_MODE p_eMode);
		static DWORD					Get					( TextureUnit::eTEXTURE_ARGUMENT p_eArgument);
		static D3DBLEND					Get					( Pass::eSRC_BLEND_FACTOR p_eBlendFactor);
		static D3DBLEND					Get					( Pass::eDST_BLEND_FACTOR p_eBlendFactor);
		static D3DFORMAT				Get					( Castor::Resources::ePIXEL_FORMAT p_pixelFormat);
		static D3DLIGHTTYPE				Get					( Light::eLIGHT_TYPE p_eLightType);
		static DWORD					GetLockFlags		( size_t p_uiFlags);
	};

#   ifdef _DEBUG
#	    define CheckDxError( hr, x, b) D3dEnum::D3dCheckError( hr, x, b)
#	    define CheckCgError( x) D3dEnum::CgCheckError( x)
#   else
#	    define CheckDxError( hr, x, b) (hr == D3D_OK)
#	    define CheckCgError( x)
#   endif
}

#endif
