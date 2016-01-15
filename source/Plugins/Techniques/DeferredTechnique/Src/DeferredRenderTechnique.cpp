#include "DeferredRenderTechnique.hpp"

#include <BlendStateManager.hpp>
#include <BufferDeclaration.hpp>
#include <BufferElementDeclaration.hpp>
#include <BufferElementGroup.hpp>
#include <Camera.hpp>
#include <ColourRenderBuffer.hpp>
#include <Context.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilStateManager.hpp>
#include <DynamicTexture.hpp>
#include <Engine.hpp>
#include <FrameBuffer.hpp>
#include <FrameVariableBuffer.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <MatrixBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <Pipeline.hpp>
#include <PointFrameVariable.hpp>
#include <RasteriserState.hpp>
#include <RenderBufferAttachment.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <ShaderManager.hpp>
#include <TextureAttachment.hpp>
#include <VertexBuffer.hpp>
#include <Viewport.hpp>

#include <Logger.hpp>

#if C3D_HAS_GL_RENDERER

#	include <GlslSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;

#endif

#if C3D_HAS_D3D11_RENDERER

#	include <Dx11RenderSystem.hpp>
using namespace Dx11Render;

#endif

#define DEBUG_BUFFERS 0

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
				cuT( "uniform int4 c3d_iLightsCount;\n" )
				cuT( "uniform float3 c3d_v3CameraPosition;\n" )
				cuT( "Texture1D c3d_sLights;\n" )
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
				cuT( "	l_lightReturn.m_v4Ambient = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (0 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v4Diffuse = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (1 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v4Specular = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (2 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v4Position = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (3 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v3Attenuation = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (4 * 0.001) + 0.0005 ).xyz;\n" )
				cuT( "	float4 l_v4A = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (5 * 0.001) + 0.0005 );\n" )
				cuT( "	float4 l_v4B = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (6 * 0.001) + 0.0005 );\n" )
				cuT( "	float4 l_v4C = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (7 * 0.001) + 0.0005 );\n" )
				cuT( "	float4 l_v4D = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (8 * 0.001) + 0.0005 );\n" )
				cuT( "	float2 l_v2Spot = c3d_sLights.Sample( LightsSampler, (i * 0.01) + (9 * 0.001) + 0.0005 ).xy;\n" )
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
				cuT( "	for( int i = 0; i < c3d_iLightsCount.x; i++ )\n" )
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

	String DS_TEXTURE_NAME[] =
	{
		cuT( "c3d_mapPosition" ),
		cuT( "c3d_mapAmbient" ),
		cuT( "c3d_mapDiffuse" ),
		cuT( "c3d_mapNormals" ),
		cuT( "c3d_mapTangent" ),
		cuT( "c3d_mapBitangent" ),
		cuT( "c3d_mapSpecular" ),
		cuT( "c3d_mapEmissive" ),
		cuT( "c3d_mapDepth" ),
	};

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "deferred" ), p_renderTarget, p_pRenderSystem, p_params )
		, m_viewport( Viewport::Ortho( *m_renderSystem->GetOwner(), 0, 1, 0, 1, 0, 1 ) )
	{
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_geometryPassFrameBuffer = m_pRenderTarget->CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i] = m_renderSystem->CreateDynamicTexture( eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
			m_lightPassTextures[i]->SetRenderTarget( p_renderTarget.shared_from_this() );
			m_geometryPassTexAttachs[i] = m_pRenderTarget->CreateAttachment( m_lightPassTextures[i] );
		}

		m_lightPassShaderProgram = GetOwner()->GetShaderManager().GetNewProgram();

		m_geometryPassDsState = GetOwner()->GetDepthStencilStateManager().Create( cuT( "GeometricPassState" ) );
		m_geometryPassDsState->SetStencilTest( true );
		m_geometryPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_geometryPassDsState->SetStencilWriteMask( 0xFFFFFFFF );
		m_geometryPassDsState->SetStencilFrontRef( 1 );
		m_geometryPassDsState->SetStencilBackRef( 1 );
		m_geometryPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_NEVER );
		m_geometryPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_NEVER );
		m_geometryPassDsState->SetStencilFrontFailOp( eSTENCIL_OP_REPLACE );
		m_geometryPassDsState->SetStencilBackFailOp( eSTENCIL_OP_REPLACE );
		m_geometryPassDsState->SetStencilFrontDepthFailOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilBackDepthFailOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilFrontPassOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilBackPassOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetDepthTest( true );
		m_geometryPassDsState->SetDepthMask( eWRITING_MASK_ALL );

		m_lightPassDsState = GetOwner()->GetDepthStencilStateManager().Create( cuT( "LightPassState" ) );
		m_lightPassDsState->SetStencilTest( true );
		m_lightPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_lightPassDsState->SetStencilWriteMask( 0 );
		m_lightPassDsState->SetStencilFrontRef( 1 );
		m_lightPassDsState->SetStencilBackRef( 1 );
		m_lightPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetDepthTest( true );
		m_lightPassDsState->SetDepthMask( eWRITING_MASK_ZERO );

		m_lightPassBlendState = GetOwner()->GetBlendStateManager().Create( cuT( "LightPassState" ) );

		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( *m_renderSystem->GetOwner(), l_vertexDeclarationElements );

		real l_pBuffer[] =
		{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1,
		};
		uint32_t l_stride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( sizeof( l_pBuffer ) );
		uint8_t * l_buffer = l_pVtxBuffer->data();
		std::memcpy( l_buffer, l_pBuffer, sizeof( l_pBuffer ) );

		for ( auto && l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
			l_buffer += l_stride;
		}

		m_pGeometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr, eTOPOLOGY_TRIANGLES );
		GetOwner()->SetPerObjectLighting( false );
	}

	RenderTechnique::~RenderTechnique()
	{
		m_pGeometryBuffers.reset();
		m_lightPassShaderProgram.reset();
		m_pDeclaration.reset();
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_return = m_geometryPassFrameBuffer->Create( 0 );

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
		{
			l_return = m_lightPassTextures[i]->Create();
			m_lightPassTextures[i]->SetSampler( GetOwner()->GetLightsSampler() );
		}

		if ( l_return )
		{
			m_lightPassShaderProgram->CreateFrameVariable( ShaderProgramBase::Lights, eSHADER_TYPE_PIXEL );

			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
			{
				m_lightPassShaderProgram->CreateFrameVariable( DS_TEXTURE_NAME[i], eSHADER_TYPE_PIXEL )->SetValue( m_lightPassTextures[i].get() );
			}

			m_lightPassMatrices = GetOwner()->GetShaderManager().CreateMatrixBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = GetOwner()->GetShaderManager().CreateSceneBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_lightPassScene = l_scene;

			m_pGeometryBuffers->Create();
			eSHADER_MODEL l_model = GetOwner()->GetRenderSystem()->GetMaxShaderModel();
			m_lightPassShaderProgram->SetSource( eSHADER_TYPE_VERTEX, l_model, DoGetLightPassVertexShaderSource( 0 ) );
			m_lightPassShaderProgram->SetSource( eSHADER_TYPE_PIXEL, l_model, DoGetLightPassPixelShaderSource( 0 ) );
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pGeometryBuffers->Destroy();

		for ( int i = 0; i < eDS_TEXTURE_DEPTH; i++ )
		{
			m_lightPassTextures[i]->Destroy();
		}

		m_geometryPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;

		for ( int i = 0; i < eDS_TEXTURE_DEPTH && l_return; i++ )
		{
			m_lightPassTextures[i]->SetType( eTEXTURE_TYPE_2D );
			m_lightPassTextures[i]->SetImage( m_pRenderTarget->GetSize(), ePIXEL_FORMAT_ARGB32F );
			l_return = m_lightPassTextures[i]->Initialise( p_index++ );
		}

		if ( l_return )
		{
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->SetType( eTEXTURE_TYPE_2D );
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->SetImage( m_pRenderTarget->GetSize(), ePIXEL_FORMAT_DEPTH32 );
			l_return = m_lightPassTextures[eDS_TEXTURE_DEPTH]->Initialise( p_index++ );
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_pRenderTarget->GetSize() );
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		}

		if ( l_return )
		{
			for ( int i = 0; i < eDS_TEXTURE_DEPTH && l_return; i++ )
			{
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, i, m_geometryPassTexAttachs[i], eTEXTURE_TARGET_2D );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH], eTEXTURE_TARGET_2D );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->IsComplete();
			}

			m_geometryPassFrameBuffer->Unbind();
		}

		m_geometryPassDsState->Initialise();
		m_lightPassDsState->Initialise();
		m_lightPassBlendState->Initialise();

		m_lightPassShaderProgram->Initialise();
		m_lightPassMatrices = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );
		FrameVariableBufferSPtr l_scene = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferScene );
		l_scene->GetVariable( ShaderProgramBase::CameraPos, m_pShaderCamera );
		m_lightPassScene = l_scene;
		m_pGeometryBuffers->Initialise( m_lightPassShaderProgram, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		m_pShaderCamera.reset();
		m_geometryPassDsState->Cleanup();
		m_lightPassDsState->Cleanup();
		m_lightPassBlendState->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_lightPassShaderProgram->Cleanup();
		m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();


		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i]->Cleanup();
		}
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );;
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		//m_geometryPassDsState->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_geometryPassFrameBuffer->Unbind();

#if DEBUG_BUFFERS

		Size const & l_size = m_pRenderTarget->GetSize();
		int l_width = int( l_size.width() );
		int l_height = int( l_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		m_geometryPassTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, 0, l_thirdWidth, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_AMBIENT]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_thirdHeight, l_width, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_EMISSIVE]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH]->Blit( m_pRenderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_twothirdHeight, l_width, l_height ), eINTERPOLATION_MODE_LINEAR );

#else

		Pipeline & l_pipeline = m_renderSystem->GetPipeline();
		ContextRPtr l_pContext = m_renderSystem->GetCurrentContext();

		m_pRenderTarget->GetFrameBuffer()->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		m_pRenderTarget->GetFrameBuffer()->SetClearColour( m_renderSystem->GetTopScene()->GetBackgroundColour() );
		m_pRenderTarget->GetFrameBuffer()->Clear();

		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		m_lightPassBlendState->Apply();

		m_viewport.SetSize( m_pRenderTarget->GetSize() );
		m_viewport.Render( l_pipeline );
		l_pContext->CullFace( eFACE_BACK );

		if ( m_pShaderCamera )
		{
			bool l_return = true;
			//Point3r l_position = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
			//m_pShaderCamera->SetValue( l_position );
			l_pipeline.ApplyMatrices( *m_lightPassMatrices.lock(), 0xFFFFFFFFFFFFFFFF );
			auto & l_sceneBuffer = *m_lightPassScene.lock();
			m_renderSystem->GetTopScene()->BindLights( *m_lightPassShaderProgram, l_sceneBuffer );
			m_renderSystem->GetTopScene()->BindCamera( l_sceneBuffer );
			m_lightPassShaderProgram->Bind( 0, 1 );

			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
			{
				l_return = m_lightPassTextures[i]->Bind();
			}

			if ( l_return )
			{
				m_pGeometryBuffers->Draw( m_lightPassShaderProgram, m_arrayVertex.size(), 0 );

				for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
				{
					m_lightPassTextures[i]->Unbind();
				}
			}

			m_lightPassShaderProgram->Unbind();
			m_renderSystem->GetTopScene()->UnbindLights( *m_lightPassShaderProgram, *m_lightPassScene.lock() );
		}

#endif
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_flags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlPixelShaderSource( p_flags );
		}
#endif
#if C3D_HAS_D3D11_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11PixelShaderSource( p_flags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassVertexShaderSource( p_uiProgramFlags );
		}

#endif
#if C3D_HAS_D3D11_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11LightPassVertexShaderSource( p_uiProgramFlags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassPixelShaderSource( uint32_t p_flags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassPixelShaderSource( p_flags );
		}

#endif
#if C3D_HAS_D3D11_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11LightPassPixelShaderSource( p_flags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

#if C3D_HAS_GL_RENDERER

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_flags )const
	{
#define CHECK_FLAG( channel ) ( ( p_flags & ( channel ) ) == ( channel ) )

		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		IN( l_writer, Vec3, vtx_vertex );
		IN( l_writer, Vec3, vtx_normal );
		IN( l_writer, Vec3, vtx_tangent );
		IN( l_writer, Vec3, vtx_bitangent );
		IN( l_writer, Vec3, vtx_texture );

		Optional< Sampler2D > c3d_mapColour( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ), CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) ) );
		Optional< Sampler2D > c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ), CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) ) );
		Optional< Sampler2D > c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ), CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) ) );
		Optional< Sampler2D > c3d_mapNormal( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormal" ), CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) ) );
		Optional< Sampler2D > c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapOpacity" ), CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) ) );
		Optional< Sampler2D > c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ), CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) ) );
		Optional< Sampler2D > c3d_mapHeight( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapHeight" ), CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) ) );
		Optional< Sampler2D > c3d_mapGloss( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapGloss" ), CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) ) );

		uint32_t l_index = 0;
		FRAG_OUTPUT( l_writer, Vec4, out_c3dPosition, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dAmbient, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dDiffuse, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dNormal, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dTangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dBitangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dSpecular, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dEmissive, l_index++ );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

			if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
			{
				l_fAlpha *= texture2D( c3d_mapOpacity, vtx_texture.XY ).R;
			}

			IF( l_writer, cuT( "l_fAlpha > 0" ) )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, vtx_vertex );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vtx_normal ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( vtx_tangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, normalize( vtx_bitangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, c3d_v4MatDiffuse.XYZ );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, c3d_v4MatAmbient.XYZ );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, vec4( c3d_v4MatSpecular.XYZ, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.XYZ );

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) )
				{
					l_v3Normal += normalize( texture2D( c3d_mapNormal, vtx_texture.XY ).XYZ * 2.0f - 1.0f );
					l_v3Tangent -= l_v3Normal * dot( l_v3Tangent, l_v3Normal );
					l_v3Bitangent = cross( l_v3Normal, l_v3Tangent );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) )
				{
					if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.XYZ * texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ, texture2D( c3d_mapGloss, vtx_texture.XY ).X );
					}
					else
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.XYZ * texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ, c3d_fMatShininess );
					}
				}
				else if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.XYZ, texture2D( c3d_mapGloss, vtx_texture.XY ).X );
				}
				else
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.XYZ, c3d_fMatShininess );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapHeight, texture2D( c3d_mapHeight, vtx_texture.XY ).XYZ );
				}

				out_c3dPosition = vec4( l_v3Position, 1 );
				out_c3dAmbient = vec4( l_v3Ambient, 1 );
				out_c3dDiffuse = vec4( l_v3Diffuse, l_fAlpha );
				out_c3dNormal = vec4( l_v3Normal, 1 );
				out_c3dTangent = vec4( l_v3Tangent, 1 );
				out_c3dBitangent = vec4( l_v3Bitangent, 1 );
				out_c3dSpecular = vec4( l_v4Specular );
				out_c3dEmissive = vec4( l_v3Emissive, 0 );
			}
			ELSE
			{
				l_writer.Discard();
			}
			FI
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();

#undef CHECK_FLAG
	}

	String RenderTechnique::DoGetGlLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// Shader inputs
		ATTRIBUTE( l_writer, Vec2, vertex );
		ATTRIBUTE( l_writer, Vec2, texture );
		UBO_MATRIX( l_writer );

		// Shader outputs
		OUT( l_writer, Vec2, vtx_texture );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.X, vertex.Y, 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetGlLightPassPixelShaderSource( uint32_t p_flags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
		l_writer << GLSL::Version() << Endl();

		IN( l_writer, Vec2, vtx_texture );

		if ( l_writer.GetOpenGl().HasTbo() )
		{
			UNIFORM( l_writer, SamplerBuffer, c3d_sLights );
		}
		else
		{
			UNIFORM( l_writer, Sampler1D, c3d_sLights );
		}

		UNIFORM( l_writer, Sampler2D, c3d_mapPosition );
		UNIFORM( l_writer, Sampler2D, c3d_mapAmbient );
		UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
		UNIFORM( l_writer, Sampler2D, c3d_mapNormals );
		UNIFORM( l_writer, Sampler2D, c3d_mapTangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapBitangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapSpecular );
		UNIFORM( l_writer, Sampler2D, c3d_mapEmissive );
		UNIFORM( l_writer, Sampler2D, c3d_mapDepth );
		UBO_SCENE( l_writer );

		FRAG_OUTPUT( l_writer, Vec4, pxl_v4FragColor, 0 );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Position, texture2D( c3d_mapPosition, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, texture2D( c3d_mapAmbient, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Diffuse, texture2D( c3d_mapDiffuse, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, texture2D( c3d_mapNormals, vtx_texture ) / 2.0 );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, texture2D( c3d_mapTangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, texture2D( c3d_mapBitangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, texture2D( c3d_mapSpecular, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Emissive, texture2D( c3d_mapEmissive, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Depth, texture2D( c3d_mapDepth, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, l_v4Specular.W );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, l_v4Specular.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, l_v4Position.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_v4Normal.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, l_v4Bitangent.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_v4Tangent.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, l_v4Emissive.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_eye, l_v3Position - c3d_v3CameraPosition );
			LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.X, c3d_v3CameraPosition.Y, c3d_v3CameraPosition.Z ) );

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											  l_output );
			}
			ROF;

			pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( Int( &l_writer, 1 ) - l_v4Emissive.W ) *
													l_writer.Paren( l_writer.Paren( l_v3Ambient + l_v4Ambient.XYZ ) +
																	l_writer.Paren( l_v3Diffuse * l_v4Diffuse.XYZ ) +
																	l_writer.Paren( l_v3Specular * l_v4Specular.XYZ ) +
																	l_v3Emissive ) ) +
									l_writer.Paren( l_v4Emissive.W * l_v4Diffuse.XYZ ), 1 );
		} );

		return l_writer.Finalise();
	}

#endif
#if C3D_HAS_D3D11_RENDERER

	String RenderTechnique::DoGetD3D11PixelShaderSource( uint32_t p_flags )const
	{
		String l_strReturn;
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( m_renderSystem );
		std::unique_ptr< UniformsBase > l_pUniforms = UniformsBase::Get( *l_renderSystem );
		std::unique_ptr< InOutsBase > l_pInputs = InOutsBase::Get( *l_renderSystem );

		if ( l_pUniforms && l_pInputs )
		{
			String l_strPixelDeclarations = g_ds.GetGSPixelDeclarations();
			String l_strPixelMainDeclarations = g_ds.GetGSPixelMainDeclarations();
			String l_strPixelMainLightsLoopAfterLightDir = g_ds.GetGSPixelMainLightsLoopAfterLightDir();
			String l_strPixelMainEnd = g_ds.GetGSPixelMainEnd();
			String l_strPixelMainLightsLoopEnd;

			if ( p_flags != 0 )
			{
				int l_iIndex = 1;

				if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapColour: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapColour = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapColour;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapAmbient: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapAmbient = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapAmbient;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapDiffuse: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapDiffuse = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapNormal: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapNormal = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainDeclarations += cuT( "	l_v4Normal += float4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapOpacity: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapOpacity = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * l_v4MapOpacity.a;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapSpecular: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapSpecular = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular *= l_v4MapSpecular;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations += cuT( "texture c3d_mapHeight: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strPixelMainDeclarations += cuT( "	float4 l_v4MapHeight = tex2D( DefaultSampler, p_input.TextureUV.xy );\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
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
		}

		return l_strReturn;
	}

	String RenderTechnique::DoGetD3D11LightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		return g_ds.GetLSVertexProgram();
	}

	String RenderTechnique::DoGetD3D11LightPassPixelShaderSource( uint32_t p_flags )const
	{
		return g_ds.GetLSPixelProgram();
	}

#endif
}
