#include "DeferredRenderTechnique.hpp"

#include <FrameBuffer.hpp>
#include <ColourRenderBuffer.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilState.hpp>
#include <RenderBufferAttachment.hpp>
#include <TextureAttachment.hpp>
#include <RenderTarget.hpp>
#include <DepthStencilState.hpp>
#include <RasteriserState.hpp>
#include <Engine.hpp>
#include <RenderSystem.hpp>
#include <BufferElementDeclaration.hpp>
#include <BufferDeclaration.hpp>
#include <BufferElementGroup.hpp>
#include <VertexBuffer.hpp>
#include <GeometryBuffers.hpp>
#include <Viewport.hpp>
#include <Context.hpp>
#include <ShaderProgram.hpp>
#include <OneFrameVariable.hpp>
#include <PointFrameVariable.hpp>
#include <FrameVariableBuffer.hpp>
#include <Pipeline.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <ShaderManager.hpp>
#include <Camera.hpp>
#include <DynamicTexture.hpp>
#include <IndexBuffer.hpp>
#include <MatrixBuffer.hpp>
#include <VertexBuffer.hpp>

#include <Logger.hpp>

#if C3D_HAS_GL_RENDERER
#	include <GlPixelShaderSource.hpp>
#	include <GlDeferredShaderSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;
#endif

#if C3D_HAS_D3D11_RENDERER
#	include <Dx11RenderSystem.hpp>
using namespace Dx11Render;
#endif

using namespace Castor;
using namespace Castor3D;

namespace Deferred
{
#if C3D_HAS_D3D11_RENDERER
	class DeferredShaderSource
	{
	protected:
		String m_strGSVertexProgram;
		String m_strGSPixelDeclarations;
		String m_strGSPixelMainDeclarations;
		String m_strGSPixelMainLightsLoopAfterLightDir;
		String m_strGSPixelMainEnd;
		String m_strLSVertexProgram;
		String m_strLSPixelProgram;

	public:
		DeferredShaderSource()
		{
			m_strGSVertexProgram =
				cuT( "uniform float4x4 c3d_mtxProjectionModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxModelView;\n" )
				cuT( "uniform float4x4 c3d_mtxView;\n" )
				cuT( "uniform float4x4 c3d_mtxNormal;\n" )
				cuT( "struct VtxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Bitangent: TANGENT1;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "};\n" )
				cuT( "struct VtxOutput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float3 Vertex: POSITION1;\n" )
				cuT( "	float3 Normal: NORMAL0;\n" )
				cuT( "	float3 Tangent: TANGENT0;\n" )
				cuT( "	float3 Bitangent: TANGENT1;\n" )
				cuT( "	float3 TextureUV: TEXCOORD0;\n" )
				cuT( "};\n" )
				cuT( "VtxOutput mainVx( VtxInput p_input )\n" )
				cuT( "{\n" )
				cuT( "	VtxOutput l_output;\n" )
				cuT( "	p_input.Position.w = 1.0f;\n" )
				cuT( "	l_output.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n" )
				cuT( "	l_output.Vertex = l_output.Position.xyz;\n" )
				cuT( "	l_output.Normal = normalize( mul( float4( p_input.Normal, 1.0 ), c3d_mtxNormal ) ).xyz;\n" )
				cuT( "	l_output.Tangent = normalize( mul( float4( p_input.Tangent, 1.0 ), c3d_mtxNormal ) ).xyz;\n" )
				cuT( "	l_output.Bitangent = normalize( mul( float4( p_input.Bitangent, 1.0 ), c3d_mtxNormal ) ).xyz;\n" )
				cuT( "	l_output.TextureUV = p_input.TextureUV;\n" )
				cuT( "	return l_output;\n" )
				cuT( "}\n" );
			m_strGSPixelDeclarations =
				cuT( "SamplerState DefaultSampler\n" )
				cuT( "{\n" )
				cuT( "	AddressU  = WRAP;\n" )
				cuT( "	AddressV  = WRAP;\n" )
				cuT( "	AddressW  = WRAP;\n" )
				cuT( "	MipFilter = NONE;\n" )
				cuT( "	MinFilter = LINEAR;\n" )
				cuT( "	MagFilter = LINEAR;\n" )
				cuT( "};\n" )
				cuT( "uniform float4 c3d_v4AmbientLight;\n" )
				cuT( "uniform float4 c3d_v4MatAmbient;\n" )
				cuT( "uniform float4 c3d_v4MatEmissive;\n" )
				cuT( "uniform float4 c3d_v4MatDiffuse;\n" )
				cuT( "uniform float4 c3d_v4MatSpecular;\n" )
				cuT( "uniform float c3d_fMatShininess;\n" )
				cuT( "uniform float c3d_fMatOpacity;\n" )
				cuT( "float pack2Floats(float f1, float f2)\n" )
				cuT( "{\n" )
				cuT( "	return (f2 + clamp(f1, 0.f, 0.999f)) * 10.f; // * 10.f, because I would loose the coef sometimes. pow = 128 and coef = 0.1 were bad for example\n" )
				cuT( "}\n" )
				cuT( "struct PxlOutput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: COLOR0;\n" )
				cuT( "	float4 DiffSpecular: COLOR1;\n" )
				cuT( "	float4 Normals: COLOR2;\n" )
				cuT( "	float4 TanBitangent: COLOR3;\n" )
				cuT( "};\n" );
			m_strGSPixelMainDeclarations =
				cuT( "PxlOutput mainPx( in VtxOutput p_input )\n" )
				cuT( "{\n" )
				cuT( "	PxlOutput l_output;\n" )
				cuT( "	float4 l_v4Position = float4( p_input.Vertex, 1 );\n" )
				cuT( "	float4 l_v4Normal = float4( p_input.Normal, 1 );\n" )
				cuT( "	float4 l_v4Tangent = float4( p_input.Tangent, 1 );\n" )
				cuT( "	float4 l_v4Bitangent = float4( p_input.Bitangent, 1 );\n" )
				cuT( "	float4 l_v4Diffuse = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4Texture = float4( p_input.TextureUV, 1 );\n" )
				cuT( "	float4 l_v4Specular = float4( c3d_v4MatSpecular.xyz, c3d_fMatShininess );\n" )
				cuT( "	float l_fAlpha = c3d_fMatOpacity;\n" );
			m_strGSPixelMainLightsLoopAfterLightDir =
				cuT( "	l_v4Diffuse = c3d_v4MatDiffuse;\n" );
			m_strGSPixelMainEnd =
				cuT( "	return l_output;\n" )
				cuT( "}\n" );
			m_strLSVertexProgram =
				cuT( "uniform	float4x4 c3d_mtxProjectionModelView;\n" )
				cuT( "struct VtxInput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float2 TextureUV: TEXCOORD0;\n" )
				cuT( "};\n" )
				cuT( "struct VtxOutput\n" )
				cuT( "{\n" )
				cuT( "	float4 Position: POSITION0;\n" )
				cuT( "	float2 TextureUV: TEXCOORD0;\n" )
				cuT( "};\n" )
				cuT( "VtxOutput mainVx( in VtxInput p_input )\n" )
				cuT( "{\n" )
				cuT( "	VtxOutput l_out;\n" )
				cuT( "	p_input.Position.w = 1.0f;\n" )
				cuT( "	l_out.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n" )
				cuT( "	l_out.TextureUV = p_input.TextureUV;\n" )
				cuT( "	return l_out;\n" )
				cuT( "}\n" );
			m_strLSPixelProgram =
				cuT( "uniform int c3d_iLightsCount;\n" )
				cuT( "uniform float3 c3d_v3CameraPosition;\n" )
				cuT( "Texture2D c3d_sLights;\n" )
				cuT( "SamplerState LightsSampler\n" )
				cuT( "{\n" )
				cuT( "	MagFilter = POINT;\n" )
				cuT( "	MinFilter = POINT;\n" )
				cuT( "	MipFilter = POINT;\n" )
				cuT( "};\n" )
				cuT( "SamplerState DefaultSampler\n" )
				cuT( "{\n" )
				cuT( "	AddressU  = WRAP;\n" )
				cuT( "	AddressV  = WRAP;\n" )
				cuT( "	AddressW  = WRAP;\n" )
				cuT( "	MipFilter = NONE;\n" )
				cuT( "	MinFilter = LINEAR;\n" )
				cuT( "	MagFilter = LINEAR;\n" )
				cuT( "};\n" )
				cuT( "Texture2D c3d_mapPosition;\n" )
				cuT( "Texture2D c3d_mapDiffuse;\n" )
				cuT( "Texture2D c3d_mapNormals;\n" )
				cuT( "Texture2D c3d_mapTangent;\n" )
				cuT( "Texture2D c3d_mapBitangent;\n" )
				cuT( "Texture2D c3d_mapSpecular;\n" )
				cuT( "struct Light\n" )
				cuT( "{\n" )
				cuT( "	float4 m_v4Ambient;\n" )
				cuT( "	float4 m_v4Diffuse;\n" )
				cuT( "	float4 m_v4Specular;\n" )
				cuT( "	float4 m_v4Position;\n" )
				cuT( "	int m_iType;\n" )
				cuT( "	float3 m_v3Attenuation;\n" )
				cuT( "	float4x4 m_mtx4Orientation;\n" )
				cuT( "	float m_fExponent;\n" )
				cuT( "	float m_fCutOff;\n" )
				cuT( "};\n" )
				cuT( "Light GetLight( int i )\n" )
				cuT( "{\n" )
				cuT( "	Light l_lightReturn;\n" )
				cuT( "	l_lightReturn.m_v4Ambient = tex2D( LightsSampler, float2( (i * 0.1) + (0 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Diffuse = tex2D( LightsSampler, float2( (i * 0.1) + (1 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Specular = tex2D( LightsSampler, float2( (i * 0.1) + (2 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Position = tex2D( LightsSampler, float2( (i * 0.1) + (3 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v3Attenuation = tex2D( LightsSampler, float2( (i * 0.1) + (4 * 0.01) + 0.005, 0.0 ) ).xyz;\n" )
				cuT( "	float4 l_v4A = tex2D( LightsSampler, float2( (i * 0.1) + (5 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	float4 l_v4B = tex2D( LightsSampler, float2( (i * 0.1) + (6 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	float4 l_v4C = tex2D( LightsSampler, float2( (i * 0.1) + (7 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	float4 l_v4D = tex2D( LightsSampler, float2( (i * 0.1) + (8 * 0.01) + 0.005, 0.0 ) );\n" )
				cuT( "	float2 l_v2Spot = tex2D( LightsSampler, float2( (i * 0.1) + (9 * 0.01) + 0.005, 0.0 ) ).xy;\n" )
				cuT( "	l_lightReturn.m_iType = int( l_lightReturn.m_v4Position.w );\n" )
				cuT( "	l_lightReturn.m_mtx4Orientation = float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n" )
				cuT( "	l_lightReturn.m_fExponent = l_v2Spot.x;\n" )
				cuT( "	l_lightReturn.m_fCutOff = l_v2Spot.x;\n" )
				cuT( "	return l_lightReturn;\n" )
				cuT( "}\n" )
				cuT( "float2 unpack2Floats(float fBoth)\n" )
				cuT( "{\n" )
				cuT( "	fBoth *= 0.1f;\n" )
				cuT( "	float f1 = frac(fBoth);\n" )
				cuT( "	float f2 = fBoth - f1;\n" )
				cuT( "	return float2(f1, f2);\n" )
				cuT( "}\n" )
				cuT( "float4 mainPx( in VtxOutput p_input ) : COLOR0\n" )
				cuT( "{\n" )
				cuT( "	float4 l_output;\n" )
				cuT( "	Light l_light;\n" )
				cuT( "	float4 l_v4Diffuses;\n" )
				cuT( "	float4 l_v4Speculars;\n" )
				cuT( "	float4 l_v4Tangents;\n" )
				cuT( "	float4 l_v4Bitangents;\n" )
				cuT( "	float4 l_v4Positions = c3d_mapPosition.Sample( DefaultSampler, p_input.TextureUV );\n" )
				cuT( "	float4 l_v4Diffuses = c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV );\n" )
				cuT( "	float4 l_v4Speculars = c3d_mapSpecular.Sample( DefaultSampler, p_input.TextureUV );\n" )
				cuT( "	float4 l_v4Normals = c3d_mapNormals.Sample( DefaultSampler, p_input.TextureUV ) / 2.0;\n" )
				cuT( "	float4 l_v4Tangents = c3d_mapTangent.Sample( DefaultSampler, p_input.TextureUV );\n" )
				cuT( "	float4 l_v4Bitangents = c3d_mapBitangent.Sample( DefaultSampler, p_input.TextureUV );\n" )
				cuT( "	float3 l_v3Binormal = l_v4Bitangents.xyz;\n" )
				cuT( "	float l_fInvRadius = 0.02;\n" )
				cuT( "	float3 l_v3Specular = float3( 0, 0, 0 );\n" )
				cuT( "	float3 l_v3Diffuse = float3( 0, 0, 0 );\n" )
				cuT( "	float3 l_v3TmpVec = -l_v4Positions.xyz;\n" )
				cuT( "	float3 l_v3EyeVec;\n" )
				cuT( "	float3 l_v3HalfVector;\n" )
				cuT( "	float3 l_v3LightDir;\n" )
				cuT( "	float l_fLambert;\n" )
				cuT( "	float l_fSpecular;\n" )
				cuT( "	float l_fSqrLength;\n" )
				cuT( "	float l_fAttenuation;\n" )
				cuT( "	float3 l_v3TmpDiffuse;\n" )
				cuT( "	float3 l_v3TmpSpecular;\n" )
				cuT( "	l_v3EyeVec.x = dot( l_v3TmpVec, l_v4Tangents.xyz );\n" )
				cuT( "	l_v3EyeVec.y = dot( l_v3TmpVec, l_v3Binormal );\n" )
				cuT( "	l_v3EyeVec.z = dot( l_v3TmpVec, l_v4Normals.xyz );\n" )
				cuT( "	l_v3EyeVec = normalize( l_v3EyeVec );\n" )
				cuT( "	for( int i = 0; i < c3d_iLightsCount; i++ )\n" )
				cuT( "	{\n" )
				cuT( "		l_light = GetLight( i );\n" )
				cuT( "		l_v3LightDir = normalize( l_light.m_v4Position.xyz - l_v4Positions.xyz );\n" )
				cuT( "		l_v3LightDir = float3( dot( l_v3LightDir, l_v4Tangents.xyz ), dot( l_v3LightDir, l_v3Binormal ), dot( l_v3LightDir, l_v4Normals.xyz ) );\n" )
				cuT( "		l_fSqrLength = dot( l_v3LightDir, l_v3LightDir );\n" )
				cuT( "		l_v3LightDir = l_v3LightDir * rsqrt( l_fSqrLength );\n" )
				cuT( "		l_fAttenuation = clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" )
				cuT( "		l_fLambert = max( dot( l_v4Normals.xyz, l_v3LightDir ), 0.0 );\n" )
				cuT( "		l_fSpecular = pow( clamp( dot( reflect( -l_v3LightDir, l_v4Normals.xyz ), l_v3EyeVec ), 0.0, 1.0 ), 10.0 );\n" )
				cuT( "		l_v3TmpDiffuse = l_light.m_v4Diffuse.xyz * l_v4Diffuses.xyz * l_fLambert;\n" )
				cuT( "		l_v3TmpSpecular = l_light.m_v4Specular.xyz * l_v4Speculars.xyz * l_fSpecular;\n" )
				cuT( "		l_v3Diffuse += l_v3TmpDiffuse * l_fAttenuation;\n" )
				cuT( "		l_v3Specular += l_v3TmpSpecular * l_fAttenuation;\n" )
				cuT( "	}\n" )
				cuT( "	l_output = float4( l_v3Diffuse + l_v3Specular, 1 );\n" )
				cuT( "//	l_output = float4( l_v4Tangents );\n" )
				cuT( "	return l_output;\n" )
				cuT( "}\n" );
		}
		virtual const String & GetGSVertexProgram()
		{
			return m_strGSVertexProgram;
		}
		virtual const String & GetGSPixelDeclarations()
		{
			return m_strGSPixelDeclarations;
		}
		virtual const String & GetGSPixelMainDeclarations()
		{
			return m_strGSPixelMainDeclarations;
		}
		virtual const String & GetGSPixelMainLightsLoopAfterLightDir()
		{
			return m_strGSPixelMainLightsLoopAfterLightDir;
		}
		virtual const String & GetGSPixelMainEnd()
		{
			return m_strGSPixelMainEnd;
		}
		virtual const String & GetLSVertexProgram()
		{
			return m_strLSVertexProgram;
		}
		virtual const String & GetLSPixelProgram()
		{
			return m_strLSPixelProgram;
		}
	};

	DeferredShaderSource g_ds;
#endif

	String g_strNames[] =
	{
		cuT( "c3d_mapPosition" ),
		cuT( "c3d_mapDiffuse" ),
		cuT( "c3d_mapNormals" ),
		cuT( "c3d_mapTangent" ),
		cuT( "c3d_mapBitangent" ),
		cuT( "c3d_mapSpecular" ),
	};

	RenderTechnique::RenderTechnique()
		:	RenderTechniqueBase( cuT( "deferred" ) )
	{
	}

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		:	RenderTechniqueBase( cuT( "deferred" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_pDsFrameBuffer = m_pRenderTarget->CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i] = m_pRenderSystem->CreateDynamicTexture();
			m_pDsTextures[i]->SetRenderTarget( p_renderTarget.shared_from_this() );
			m_pDsTexAttachs[i] = m_pRenderTarget->CreateAttachment( m_pDsTextures[i] );
		}

		m_pDsBufDepth = m_pDsFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24S8 );
		m_pDsDepthAttach = m_pRenderTarget->CreateAttachment( m_pDsBufDepth );
		m_pDsShaderProgram = m_pEngine->GetShaderManager().GetNewProgram();
		m_pDsGeometricState = m_pEngine->CreateDepthStencilState( cuT( "GeometricPassState" ) );
		m_pDsLightsState = m_pEngine->CreateDepthStencilState( cuT( "LightPassState" ) );

		m_pDsGeometricState->SetStencilTest( true );
		m_pDsGeometricState->SetStencilReadMask( 0xFFFFFFFF );
		m_pDsGeometricState->SetStencilWriteMask( 0xFFFFFFFF );
		m_pDsGeometricState->SetStencilFrontRef( 1 );
		m_pDsGeometricState->SetStencilBackRef( 1 );
		m_pDsGeometricState->SetStencilFrontFunc( eSTENCIL_FUNC_NEVER );
		m_pDsGeometricState->SetStencilBackFunc( eSTENCIL_FUNC_NEVER );
		m_pDsGeometricState->SetStencilFrontFailOp( eSTENCIL_OP_REPLACE );
		m_pDsGeometricState->SetStencilBackFailOp( eSTENCIL_OP_REPLACE );
		m_pDsGeometricState->SetStencilFrontDepthFailOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilBackDepthFailOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilFrontPassOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilBackPassOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetDepthTest( true );
		m_pDsGeometricState->SetDepthMask( eWRITING_MASK_ALL );

		m_pDsLightsState->SetStencilTest( true );
		m_pDsLightsState->SetStencilReadMask( 0xFFFFFFFF );
		m_pDsLightsState->SetStencilWriteMask( 0 );
		m_pDsLightsState->SetStencilFrontRef( 1 );
		m_pDsLightsState->SetStencilBackRef( 1 );
		m_pDsLightsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_pDsLightsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_pDsLightsState->SetDepthTest( true );
		m_pDsLightsState->SetDepthMask( eWRITING_MASK_ZERO );

		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS ),
		};

		real l_pBuffer[] =
		{
			0, 0, 0, 0, 0,
			0, 1, 0, 0, 1,
			1, 1, 0, 1, 1,
			1, 0, 0, 1, 0,
		};

		std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		uint32_t i = 0;

		std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
		} );

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, l_vertexDeclarationElements );
		IndexBufferUPtr l_pIdxBuffer = std::make_unique< IndexBuffer >( m_pRenderSystem );

		l_pVtxBuffer->Resize( 4 * l_uiStride );
		m_arrayVertex[0]->LinkCoords( &l_pVtxBuffer->data()[0 * l_uiStride], l_uiStride );
		m_arrayVertex[1]->LinkCoords( &l_pVtxBuffer->data()[1 * l_uiStride], l_uiStride );
		m_arrayVertex[2]->LinkCoords( &l_pVtxBuffer->data()[2 * l_uiStride], l_uiStride );
		m_arrayVertex[3]->LinkCoords( &l_pVtxBuffer->data()[3 * l_uiStride], l_uiStride );

		l_pIdxBuffer->AddElement( 0 );
		l_pIdxBuffer->AddElement( 2 );
		l_pIdxBuffer->AddElement( 1 );
		l_pIdxBuffer->AddElement( 0 );
		l_pIdxBuffer->AddElement( 3 );
		l_pIdxBuffer->AddElement( 2 );
		m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), std::move( l_pIdxBuffer ), nullptr );

		m_pViewport = std::make_shared< Viewport >( m_pRenderSystem->GetEngine(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
		m_pViewport->SetLeft( real( 0.0 ) );
		m_pViewport->SetRight( real( 1.0 ) );
		m_pViewport->SetTop( real( 1.0 ) );
		m_pViewport->SetBottom( real( 0.0 ) );
		m_pViewport->SetNear( real( 0.0 ) );
		m_pViewport->SetFar( real( 1.0 ) );
	}

	RenderTechnique::~RenderTechnique()
	{
		m_pViewport.reset();
		m_pGeometryBuffers.reset();
		m_pDsShaderProgram.reset();
		m_pDeclaration.reset();
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		m_pRenderSystem->GetMainContext()->SetDeferredShading( true );
		bool l_bReturn = m_pDsFrameBuffer->Create( 0 );

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			l_bReturn = m_pDsTextures[i]->Create();
			m_pDsTextures[i]->SetSampler( m_sampler );
		}

		l_bReturn &= m_pDsBufDepth->Create();

		if ( l_bReturn )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				m_pDsShaderProgram->CreateFrameVariable( g_strNames[i], eSHADER_TYPE_PIXEL )->SetValue( m_pDsTextures[i].get() );
			}

			m_pDsMatrices = m_pEngine->GetShaderManager().CreateMatrixBuffer( *m_pDsShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = m_pEngine->GetShaderManager().CreateSceneBuffer( *m_pDsShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_pDsScene = l_scene;

			m_pGeometryBuffers->GetVertexBuffer().Create();
			m_pGeometryBuffers->GetIndexBuffer().Create();

			for ( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
			{
				eSHADER_MODEL l_eModel = eSHADER_MODEL( i );

				if ( m_pRenderSystem->CheckSupport( l_eModel ) )
				{
					m_pDsShaderProgram->SetSource( eSHADER_TYPE_VERTEX,	l_eModel, DoGetLightPassVertexShaderSource( 0 ) );
					m_pDsShaderProgram->SetSource( eSHADER_TYPE_PIXEL,	l_eModel, DoGetLightPassPixelShaderSource( 0 ) );
				}
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pGeometryBuffers->GetVertexBuffer().Destroy();
		m_pGeometryBuffers->GetIndexBuffer().Destroy();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i]->Destroy();
		}

		m_pDsBufDepth->Destroy();
		m_pDsFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			m_pDsTextures[i]->SetDimension(	eTEXTURE_DIMENSION_2D );

			if ( i != eDS_TEXTURE_POSITION )
			{
				m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_A8R8G8B8 );
			}
			else
			{
				m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_ARGB32F );
			}

			m_pDsTextures[i]->Initialise( p_index++ );
		}

		m_pDsBufDepth->Initialise( m_size );

		if ( m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				l_bReturn &= m_pDsTexAttachs[i]->Attach( eATTACHMENT_POINT( eATTACHMENT_POINT_COLOUR0 + i ),	m_pDsFrameBuffer, eTEXTURE_TARGET_2D );
			}

			l_bReturn &= m_pDsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pDsFrameBuffer );
			m_pDsFrameBuffer->Unbind();
		}

		m_pDsGeometricState->Initialise();
		m_pDsLightsState->Initialise();

		m_pDsShaderProgram->Initialise();
		m_pDsMatrices = m_pDsShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );
		FrameVariableBufferSPtr l_scene = m_pDsShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferScene );
		l_scene->GetVariable( ShaderProgramBase::CameraPos, m_pShaderCamera );
		m_pDsScene = l_scene;
		m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, m_pDsShaderProgram );
		m_pGeometryBuffers->GetIndexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, m_pDsShaderProgram );
		m_pGeometryBuffers->Initialise();
		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_pShaderCamera.reset();
		m_pDsGeometricState->Cleanup();
		m_pDsLightsState->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pDsShaderProgram->Cleanup();
		m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTexAttachs[i]->Detach();
		}

		m_pDsDepthAttach->Detach();
		m_pDsFrameBuffer->Unbind();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i]->Cleanup();
		}

		m_pDsBufDepth->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );;
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pDsGeometricState->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		Size const & l_size = m_pRenderTarget->GetSize();
		Size l_halfSize( l_size.width() / 2, l_size.height() / 2 );
		Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
		ContextRPtr l_pContext = m_pRenderSystem->GetCurrentContext();
		m_pDsFrameBuffer->Unbind();

		if ( m_pViewport )
		{
			bool l_bReturn = true;
			m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
			m_pDsLightsState->Apply();
			//m_pRenderTarget->GetDepthStencilState()->Apply();
			m_pRenderTarget->GetRasteriserState()->Apply();
			//m_pRenderTarget->GetRenderer()->BeginScene();
			l_pContext->SetClearColour( m_pRenderSystem->GetTopScene()->GetBackgroundColour() );
			l_pContext->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
			m_pViewport->SetSize( m_size );
			m_pViewport->Render();
			l_pContext->CullFace( eFACE_BACK );
			l_pPipeline->MatrixMode( eMTXMODE_MODEL );
			l_pPipeline->LoadIdentity();
			l_pPipeline->MatrixMode( eMTXMODE_VIEW );
			l_pPipeline->LoadIdentity();

			if ( m_pShaderCamera )
			{
				Point3r l_position = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
				m_pShaderCamera->SetValue( l_position );
				m_pDsShaderProgram->Bind( 0, 1 );
				l_pPipeline->ApplyMatrices( *m_pDsMatrices.lock(), 0xFFFFFFFFFFFFFFFF );

#if DEBUG_BUFFERS
				int l_width = int( m_size.width() );
				int l_height = int( m_size.height() );
				int l_thirdWidth = int( l_width / 3.0f );
				int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
				int l_halfHeight = int( l_height / 2.0f );
				m_pDsTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, 0, l_thirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, 0, l_width, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, l_halfHeight, l_thirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, l_halfHeight, l_twoThirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, l_halfHeight, l_width, l_height ), eINTERPOLATION_MODE_LINEAR );
#else

				for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
				{
					l_bReturn = m_pDsTextures[i]->Bind();
				}

				if ( l_bReturn )
				{
					m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, m_pDsShaderProgram, m_pGeometryBuffers->GetIndexBuffer().GetSize(), 0 );

					for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
					{
						m_pDsTextures[i]->Unbind();
					}
				}

#endif

				m_pDsShaderProgram->Unbind();
			}

			//m_pRenderTarget->EndScene();
			m_pFrameBuffer->Unbind();
		}
	}

	String RenderTechnique::DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		if ( !m_pRenderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlVertexShaderSource( p_uiProgramFlags );
		}
#endif

#if C3D_HAS_D3D11_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11VertexShaderSource( p_uiProgramFlags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_uiFlags )const
	{
		if ( !m_pRenderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlPixelShaderSource( p_uiFlags );
		}
#endif

#if C3D_HAS_D3D11_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11PixelShaderSource( p_uiFlags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		if ( !m_pRenderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassVertexShaderSource( p_uiProgramFlags );
		}
#endif

#if C3D_HAS_D3D11_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11LightPassVertexShaderSource( p_uiProgramFlags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassPixelShaderSource( uint32_t p_uiFlags )const
	{
		if ( !m_pRenderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassPixelShaderSource( p_uiFlags );
		}
#endif

#if C3D_HAS_D3D11_RENDERER
		if ( m_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11LightPassPixelShaderSource( p_uiFlags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

#if C3D_HAS_GL_RENDERER
	String RenderTechnique::DoGetGlVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::VariablesBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		GLSL::ConstantsBase * l_pConstants = GLSL::ConstantsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::KeywordsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
		String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
		String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
		String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
		String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
		String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();

		String l_strVertexInMatrices = l_pConstants->Matrices();
		String l_strVertexOutMatrices = l_pVariables->GetVertexOutMatrices();
		String l_strVertexMatrixCopy = l_pVariables->GetVertexMatrixCopy();
		str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
		l_strReturn += l_strVersion;
		l_strReturn += l_strAttribute0 + cuT( " <vec4> vertex;\n" );
		l_strReturn += l_strAttribute1 + cuT( " <vec3> normal;\n" );
		l_strReturn += l_strAttribute2 + cuT( " <vec3> tangent;\n" );
		l_strReturn += l_strAttribute3 + cuT( " <vec3> bitangent;\n" );
		l_strReturn += l_strAttribute4 + cuT( " <vec3> texture;\n" );

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += l_strAttribute5	+ cuT( "    <mat4>   transform;\n" );
		}

		l_strReturn += l_strVertexInMatrices;
		l_strReturn += l_strOut + cuT( " <vec3> vtx_vertex;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_normal;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_tangent;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_bitangent;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_texture;\n" );
		l_strReturn += l_strVertexOutMatrices;
		l_strReturn += cuT( "void main()\n" );
		l_strReturn += cuT( "{\n" );
		l_strReturn += l_strVertexMatrixCopy;
		l_strReturn += cuT( "	vtx_texture = texture;\n" );

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += cuT( "	<mat4> l_mtxMV = c3d_mtxView * transform;\n" );
			l_strReturn += cuT( "	<mat4> l_mtxN = transpose( inverse( l_mtxMV ) );\n" );
			l_strReturn += cuT( "	vtx_normal = normalize( (l_mtxN * <vec4>( normal, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	vtx_tangent = normalize( (l_mtxN * <vec4>( tangent, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	vtx_bitangent = normalize( (l_mtxN * <vec4>( bitangent, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	<vec4> l_v4Vtx = l_mtxMV * vertex;\n" );
			l_strReturn += cuT( "	gl_Position = c3d_mtxProjection * l_mtxMV * vertex;\n" );
		}
		else
		{
			l_strReturn += cuT( "	vtx_normal = normalize( (c3d_mtxNormal * <vec4>( normal, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	vtx_tangent = normalize( (c3d_mtxNormal * <vec4>( tangent, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	vtx_bitangent = normalize( (c3d_mtxNormal * <vec4>( bitangent, 0.0 )).xyz );\n" );
			l_strReturn += cuT( "	<vec4> l_v4Vtx = c3d_mtxModelView * vertex;\n" );
			l_strReturn += cuT( "	gl_Position = c3d_mtxProjectionModelView * vertex;\n" );
		}

		l_strReturn += cuT( "}\n" );

		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
		return l_strReturn;
	}

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_uiFlags )const
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::VariablesBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		GLSL::ConstantsBase * l_pConstants = GLSL::ConstantsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::KeywordsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();
		String l_strTexture1D = l_pKeywords->GetTexture1D();
		String l_strTexture2D = l_pKeywords->GetTexture2D();
		String l_strTexture3D = l_pKeywords->GetTexture3D();

		String l_strPixelDeclarations = deferredShaderSource.GetGSPixelDeclarations();
		String l_strPixelMainDeclarations = deferredShaderSource.GetGSPixelMainDeclarations();
		String l_strPixelMainLightsLoopAfterLightDir = deferredShaderSource.GetGSPixelMainLightsLoopAfterLightDir();
		String l_strPixelMainLightsLoopEnd;
		String l_strPixelMainEnd = deferredShaderSource.GetGSPixelMainEnd();
		String l_strPixelInMatrices = l_pConstants->Matrices();
		String l_strPixelScene = l_pConstants->Scene();
		String l_strPixelPass = l_pConstants->Pass();
		str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );
		l_strReturn += l_strVersion;
		l_strReturn += l_strPixelInMatrices;
		l_strReturn += l_strPixelScene;
		l_strReturn += l_strPixelPass;
		l_strReturn += l_strIn + cuT( " <vec3> vtx_vertex;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_normal;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_tangent;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_bitangent;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_texture;\n" );
		l_strReturn += l_strPixelDeclarations;
		l_strReturn += l_pKeywords->GetGSOutPositionDecl();
		l_strReturn += l_pKeywords->GetGSOutDiffuseDecl();
		l_strReturn += l_pKeywords->GetGSOutNormalDecl();
		l_strReturn += l_pKeywords->GetGSOutTangentDecl();
		l_strReturn += l_pKeywords->GetGSOutBitangentDecl();
		l_strReturn += l_pKeywords->GetGSOutSpecularDecl();
		l_strReturn += l_pKeywords->GetGSOutEmissiveDecl();

		if ( p_uiFlags != 0 )
		{
			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapColour;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapColour = <texture2D>( c3d_mapColour, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapColour;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapAmbient;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4    l_v4MapAmbient = <texture2D>( c3d_mapAmbient, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapAmbient;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapDiffuse;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapDiffuse = <texture2D>( c3d_mapDiffuse, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapNormal;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapNormal = <texture2D>( c3d_mapNormal, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainDeclarations += cuT( "	l_v4Normal += vec4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
				l_strPixelMainDeclarations += cuT( "	l_v4Tangent -= vec4( l_v4Normal.xyz * dot( l_v4Tangent.xyz, l_v4Normal.xyz ), 0 );\n" );
				l_strPixelMainDeclarations += cuT( "	l_v4Bitangent = vec4( cross( l_v4Normal.xyz, l_v4Tangent.xyz ), 1 );\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapOpacity;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapOpacity = <texture2D>( c3d_mapOpacity, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapSpecular;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapSpecular = <texture2D>( c3d_mapSpecular, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular.xyz *= l_v4MapSpecular.xyz;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapHeight;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapHeight = <texture2D>( c3d_mapHeight, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapGloss;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapGloss = <texture2D>( c3d_mapGloss, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular.w *= l_v4MapGloss.x;\n" );
			}
		}

		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutPositionName() + cuT( " = vec4( l_v4Position.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutDiffuseName() + cuT( " = vec4( l_v4Diffuse.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutNormalName() + cuT( " = vec4( l_v4Normal.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutTangentName() + cuT( " = vec4( l_v4Tangent.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutBitangentName() + cuT( " = vec4( l_v4Bitangent.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutSpecularName() + cuT( " = vec4( l_v4Specular );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutEmissiveName() + cuT( " = vec4( l_v4Emissive );\n" );
		l_strReturn += l_strPixelMainDeclarations + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
		//		Logger::LogDebug( l_strReturn );

		str_utils::replace( l_strReturn, cuT( "<texture1D>" ), l_strTexture1D );
		str_utils::replace( l_strReturn, cuT( "<texture2D>" ), l_strTexture2D );
		str_utils::replace( l_strReturn, cuT( "<texture3D>" ), l_strTexture3D );
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
		return l_strReturn;
	}

	String RenderTechnique::DoGetGlLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::VariablesBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		GLSL::ConstantsBase * l_pConstants = GLSL::ConstantsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::KeywordsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
		String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
		String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
		String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
		String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
		String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();

		String l_strVertexInMatrices = l_pConstants->Matrices();
		String l_strVertexOutMatrices = l_pVariables->GetVertexOutMatrices();
		String l_strVertexMatrixCopy = l_pVariables->GetVertexMatrixCopy();
		str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
		l_strReturn += l_strVersion;
		l_strReturn += l_strAttribute0 + cuT( " <vec4> vertex;\n" );
		l_strReturn += l_strAttribute1 + cuT( " <vec2> texture;\n" );
		l_strReturn += l_strVertexInMatrices;
		l_strReturn += l_strOut + cuT( " <vec2> vtx_texture;\n" );
		l_strReturn += l_strVertexOutMatrices;
		l_strReturn += cuT( "void main( void )\n" );
		l_strReturn += cuT( "{\n" );
		l_strReturn += l_strVertexMatrixCopy;
		l_strReturn += cuT( "	<vec4> position = c3d_mtxProjectionModelView * vertex;\n" );
		l_strReturn += cuT( "	gl_Position = vec4( position.x, position.y, position.z, position.w );\n" );
		l_strReturn += cuT( "	vtx_texture = texture;\n" );
		l_strReturn += cuT( "}\n" );

		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
		return l_strReturn;
	}

	String RenderTechnique::DoGetGlLightPassPixelShaderSource( uint32_t p_uiFlags )const
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::VariablesBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		GLSL::ConstantsBase * l_pConstants = GLSL::ConstantsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::KeywordsBase::Get( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGl() );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();
		String l_strTexture1D = l_pKeywords->GetTexture1D();
		String l_strTexture2D = l_pKeywords->GetTexture2D();
		String l_strTexture3D = l_pKeywords->GetTexture3D();

		String l_strPixelInMatrices = l_pVariables->GetPixelInMatrices();
		String l_strPixelScene = l_pConstants->Scene();
		String l_strPixelPass = l_pConstants->Pass();
		String l_strPixelMtxModelView = l_pVariables->GetPixelMtxModelView();
		str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );
		l_strReturn += l_strVersion;
		l_strReturn += l_strPixelInMatrices;
		l_strReturn += l_strPixelScene;
		l_strReturn += l_strPixelPass;
		l_strReturn += l_pKeywords->GetPixelOut();
		l_strReturn += cuT( "uniform sampler1D c3d_sLights;\n" );
		l_strReturn += deferredShaderSource.GetLSPixelProgram();
		str_utils::replace( l_strReturn, cuT( "<pxlin_mtxModelView>" ), l_strPixelMtxModelView );

		str_utils::replace( l_strReturn, cuT( "<texture1D>" ), l_strTexture1D );
		str_utils::replace( l_strReturn, cuT( "<texture2D>" ), l_strTexture2D );
		str_utils::replace( l_strReturn, cuT( "<texture3D>" ), l_strTexture3D );
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
		return l_strReturn;
	}
#endif

#if C3D_HAS_D3D11_RENDERER
	String RenderTechnique::DoGetD3D11VertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		return g_ds.GetGSVertexProgram();
	}

	String RenderTechnique::DoGetD3D11PixelShaderSource( uint32_t p_uiFlags )const
	{
		String l_strReturn;
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );
		std::unique_ptr< UniformsBase > l_pUniforms = UniformsBase::Get( *l_renderSystem );
		std::unique_ptr< InOutsBase > l_pInputs = InOutsBase::Get( *l_renderSystem );

		if ( l_pUniforms && l_pInputs )
		{
			String l_strPixelDeclarations = g_ds.GetGSPixelDeclarations();
			String l_strPixelMainDeclarations = g_ds.GetGSPixelMainDeclarations();
			String l_strPixelMainLightsLoopAfterLightDir = g_ds.GetGSPixelMainLightsLoopAfterLightDir();
			String l_strPixelMainEnd = g_ds.GetGSPixelMainEnd();
			String l_strPixelMainLightsLoopEnd;

			if ( p_uiFlags != 0 )
			{
				int l_iIndex = 1;

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapColour: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapColour = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapColour;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapAmbient: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapAmbient = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapAmbient;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapDiffuse: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapDiffuse = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapNormal: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapNormal = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainDeclarations += cuT( "	l_v4Normal += float4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapOpacity: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapOpacity = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * l_v4MapOpacity.a;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapSpecular: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapSpecular = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular *= l_v4MapSpecular;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapHeight: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapHeight = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapGloss: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapGloss = tex2D( GlossTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular.w *= l_v4MapGloss.x;\n" );
				}
			}

			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.Position = float4( l_v4Position.xyz,	1 );\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.Normals = float4( l_v4Normal.xyz,	1 );\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.x = pack2Floats(l_v4Diffuse.x,l_v4Diffuse.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.y = pack2Floats(l_v4Diffuse.z,l_v4Diffuse.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.z = pack2Floats(l_v4Specular.x,l_v4Specular.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.w = pack2Floats(l_v4Specular.z,l_v4Specular.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.x = pack2Floats(l_v4Tangent.x,l_v4Tangent.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.y = pack2Floats(l_v4Tangent.z,l_v4Tangent.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.z = pack2Floats(l_v4Bitangent.x,l_v4Bitangent.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.w = pack2Floats(l_v4Bitangent.z,l_v4Bitangent.w);\n" );
			l_strReturn = l_strPixelDeclarations + l_strPixelMainDeclarations + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
			//Logger::LogDebug( l_strReturn );
		}

		return l_strReturn;
	}

	String RenderTechnique::DoGetD3D11LightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		return g_ds.GetLSVertexProgram();
	}

	String RenderTechnique::DoGetD3D11LightPassPixelShaderSource( uint32_t p_uiFlags )const
	{
		return g_ds.GetLSPixelProgram();
	}
#endif
}
