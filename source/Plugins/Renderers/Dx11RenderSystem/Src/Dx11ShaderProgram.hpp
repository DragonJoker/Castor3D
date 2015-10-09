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
#ifndef ___DX11_SHADER_PROGRAM_H___
#define ___DX11_SHADER_PROGRAM_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <ShaderProgram.hpp>

namespace Dx11Render
{
	class UniformsBase
	{
	public:
		inline Castor::String GetVertexInMatrices( int32_t p_index )const
		{
			xchar l_buffer[1024];
#if !CASTOR_USE_UNICODE
			sprintf( l_buffer, m_strVertexInMatrices.c_str(), p_index );
#else
			wsprintf( l_buffer, m_strVertexInMatrices.c_str(), p_index );
#endif
			return l_buffer;
		}

		inline Castor::String GetPixelInMatrices( int32_t p_index )const
		{
			xchar l_buffer[1024];
#if !CASTOR_USE_UNICODE
			sprintf( l_buffer, m_strPixelInMatrices.c_str(), p_index );
#else
			wsprintf( l_buffer, m_strPixelInMatrices.c_str(), p_index );
#endif
			return l_buffer;
		}

		inline Castor::String GetPixelPass( int32_t p_index )const
		{
			xchar l_buffer[1024];
#if !CASTOR_USE_UNICODE
			sprintf( l_buffer, m_strPixelPass.c_str(), p_index );
#else
			wsprintf( l_buffer, m_strPixelPass.c_str(), p_index );
#endif
			return l_buffer;
		}

		inline Castor::String GetPixelScene( int32_t p_index )const
		{
			xchar l_buffer[1024];
#if !CASTOR_USE_UNICODE
			sprintf( l_buffer, m_strPixelScene.c_str(), p_index );
#else
			wsprintf( l_buffer, m_strPixelScene.c_str(), p_index );
#endif
			return l_buffer;
		}

		C3D_Dx11_API static std::unique_ptr< UniformsBase > Get( DxRenderSystem const & p_renderSystem );

	protected:
		Castor::String m_strVertexInMatrices;
		Castor::String m_strPixelInMatrices;
		Castor::String m_strPixelPass;
		Castor::String m_strPixelScene;
	};

	class UniformsBuf
		: public UniformsBase
	{
	public:
		C3D_Dx11_API UniformsBuf()
		{
			m_strVertexInMatrices =
				cuT( "cbuffer Matrices : register( cb%d )\n" )
				cuT( "{\n" )
				cuT( "	matrix c3d_mtxProjection;\n" )
				cuT( "	matrix c3d_mtxModel;\n" )
				cuT( "	matrix c3d_mtxView;\n" )
				cuT( "	matrix c3d_mtxModelView;\n" )
				cuT( "	matrix c3d_mtxProjectionModelView;\n" )
				cuT( "	matrix c3d_mtxNormal;\n" )
				cuT( "	matrix c3d_mtxTexture0;\n" )
				cuT( "	matrix c3d_mtxTexture1;\n" )
				cuT( "	matrix c3d_mtxTexture2;\n" )
				cuT( "	matrix c3d_mtxTexture3;\n" )
				cuT( "};\n" );
			m_strPixelInMatrices =
				cuT( "cbuffer Matrices : register( cb%d )\n" )
				cuT( "{\n" )
				cuT( "	matrix c3d_mtxProjection;\n" )
				cuT( "	matrix c3d_mtxModel;\n" )
				cuT( "	matrix c3d_mtxView;\n" )
				cuT( "	matrix c3d_mtxModelView;\n" )
				cuT( "	matrix c3d_mtxProjectionModelView;\n" )
				cuT( "	matrix c3d_mtxNormal;\n" )
				cuT( "	matrix c3d_mtxTexture0;\n" )
				cuT( "	matrix c3d_mtxTexture1;\n" )
				cuT( "	matrix c3d_mtxTexture2;\n" )
				cuT( "	matrix c3d_mtxTexture3;\n" )
				cuT( "};\n" );
			m_strPixelScene =
				cuT( "cbuffer Scene : register( cb%d )\n" )
				cuT( "{\n" )
				cuT( "	int c3d_iLightsCount;\n" )
				cuT( "	float4 c3d_v4AmbientLight;\n" )
				cuT( "	float4 c3d_v4BackgroundColour;\n" )
				cuT( "	float3 c3d_v3CameraPosition;\n" )
				cuT( "};\n" );
			m_strPixelPass =
				cuT( "cbuffer PassBuffer : register( cb%d )\n" )
				cuT( "{\n" )
				cuT( "	float4 c3d_v4MatAmbient;\n" )
				cuT( "	float4 c3d_v4MatDiffuse;\n" )
				cuT( "	float4 c3d_v4MatEmissive;\n" )
				cuT( "	float4 c3d_v4MatSpecular;\n" )
				cuT( "	float c3d_fMatShininess;\n" )
				cuT( "	float c3d_fMatOpacity;\n" )
				cuT( "	float c3d_fMatFill0;\n" )
				cuT( "	float c3d_fMatFill1;\n" )
				cuT( "};\n" );
		}
	};

	class UniformsStd
		: public UniformsBase
	{
	public:
		C3D_Dx11_API UniformsStd()
		{
			m_strVertexInMatrices =
				cuT( "uniform float4x4 c3d_mtxProjection;\n" )
				cuT( "uniform float4x4 c3d_mtxModel;\n" )
				cuT( "uniform float4x4 c3d_mtxView;\n" )
				cuT( "uniform float4x4 c3d_mtxModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxProjectionModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxNormal;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture0;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture1;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture2;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture3;\n" );
			m_strPixelInMatrices =
				cuT( "uniform float4x4 c3d_mtxProjection;\n" )
				cuT( "uniform float4x4 c3d_mtxModel;\n" )
				cuT( "uniform float4x4 c3d_mtxView;\n" )
				cuT( "uniform float4x4 c3d_mtxModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxProjectionModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxNormal;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture0;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture1;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture2;\n" )
				cuT( "uniform float4x4 c3d_mtxTexture3;\n" );
			m_strPixelScene =
				cuT( "uniform int c3d_iLightsCount;\n" )
				cuT( "uniform float4 c3d_v4AmbientLight;\n" )
				cuT( "uniform float4 c3d_v4BackgroundColour;\n" )
				cuT( "uniform float3 c3d_v3CameraPosition;\n" );
			m_strPixelPass =
				cuT( "uniform float4 c3d_v4MatAmbient;\n" )
				cuT( "uniform float4 c3d_v4MatDiffuse;\n" )
				cuT( "uniform float4 c3d_v4MatEmissive;\n" )
				cuT( "uniform float4 c3d_v4MatSpecular;\n" )
				cuT( "uniform float c3d_fMatShininess;\n" )
				cuT( "uniform float c3d_fMatOpacity;\n" );
		}
	};

	class InOutsBase
	{
	public:
		Castor::String const & GetVtxInput()const
		{
			return m_strVtxInput;
		}

		Castor::String const & GetVtxOutput()const
		{
			return m_strVtxOutput;
		}

		Castor::String const & GetPxlInput()const
		{
			return m_strPxlInput;
		}

		Castor::String const & GetPxlOutput()const
		{
			return m_strPxlOutput;
		}

		C3D_Dx11_API static std::unique_ptr< InOutsBase > Get( DxRenderSystem const & p_renderSystem );

	protected:
		Castor::String m_strVtxInput;
		Castor::String m_strVtxOutput;
		Castor::String m_strPxlInput;
		Castor::String m_strPxlOutput;
	};

	class InOutsOld
		: public InOutsBase
	{
	public:
		C3D_Dx11_API InOutsOld()
		{
			m_strVtxInput =
				cuT( "struct VtxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				//cuT( "	int4 BoneIDs: BLENDINDICES;\n")
				//cuT( "	float4 Weights: BLENDWEIGHT;\n" )
				//cuT( "	float4x4 Matrix: MATRIX;\n" )
				cuT( "};\n" );
			m_strVtxOutput =
				cuT( "struct VtxOutput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Vertex: POSITION1;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "	float3 Eye: POSITION2;\n" )
				cuT( "};\n" );
			m_strPxlInput =
				cuT( "struct PxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Vertex: POSITION1;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "	float3 Eye: POSITION2;\n" )
				cuT( "};\n" );
			m_strPxlOutput = cuT( "COLOR0" );
		}
	};

	class InOutsNew
		: public InOutsBase
	{
	public:
		C3D_Dx11_API InOutsNew()
		{
			m_strVtxInput =
				cuT( "struct VtxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				//cuT( "	int4 BoneIDs: BLENDINDICES;\n")
				//cuT( "	float4 Weights: BLENDWEIGHT;\n" )
				//cuT( "	float4x4 Matrix: MATRIX;\n"	 )
				cuT( "};\n" );
			m_strVtxOutput =
				cuT( "struct VtxOutput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: SV_POSITION;\n" )
				cuT( "	float3 Vertex: POSITION0;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "	float3 Eye: POSITION1;\n" )
				cuT( "};\n" );
			m_strPxlInput =
				cuT( "struct PxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: SV_POSITION;\n" )
				cuT( "	float3 Vertex: POSITION0;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Binormal: BINORMAL0;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "	float3 Eye: POSITION1;\n" )
				cuT( "};\n" );
			m_strPxlOutput = cuT( "SV_TARGET" );
		}
	};

	class DxShaderProgram
		: public Castor3D::ShaderProgramBase
	{
	public:
		DxShaderProgram( DxRenderSystem & p_renderSystem );
		virtual ~DxShaderProgram();

		/**
		 * Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( Castor::String & strLog );

		virtual void Initialise();
		virtual void Cleanup();
		virtual void Bind( uint8_t p_byIndex, uint8_t p_byCount );
		virtual void Unbind();

		int GetAttributeLocation( Castor::String const & CU_PARAM_UNUSED( p_name ) )const
		{
			return 0;
		}
		ID3DBlob * GetCompiled( Castor3D::eSHADER_TYPE p_type );

	private:
		Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_type );
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable > DoCreateTextureVariable( int p_iNbOcc );
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const;

	protected:
		Castor::String m_linkerLog;
		DxRenderSystem * m_pDxRenderSystem;
	};
}

#endif
