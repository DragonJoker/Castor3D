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

#	include <GlShaderSource.hpp>
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

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "deferred" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_lightPassFrameBuffer = m_pRenderTarget->CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i] = m_renderSystem->CreateDynamicTexture();
			m_lightPassTextures[i]->SetRenderTarget( p_renderTarget.shared_from_this() );
			m_lightPassTexAttachs[i] = m_pRenderTarget->CreateAttachment( m_lightPassTextures[i] );
		}

		m_lightPassBufDepth = m_lightPassFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24S8 );
		m_lightPassDepthAttach = m_pRenderTarget->CreateAttachment( m_lightPassBufDepth );
		m_lightPassShaderProgram = GetOwner()->GetShaderManager().GetNewProgram();

		m_geometryPassDsState = GetOwner()->CreateDepthStencilState( cuT( "GeometricPassState" ) );
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

		m_lightPassDsState = GetOwner()->CreateDepthStencilState( cuT( "LightPassState" ) );
		m_lightPassDsState->SetStencilTest( true );
		m_lightPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_lightPassDsState->SetStencilWriteMask( 0 );
		m_lightPassDsState->SetStencilFrontRef( 1 );
		m_lightPassDsState->SetStencilBackRef( 1 );
		m_lightPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetDepthTest( true );
		m_lightPassDsState->SetDepthMask( eWRITING_MASK_ZERO );

		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, l_vertexDeclarationElements );

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
		CASTOR_ASSERT( sizeof( l_pBuffer ) == 6 * l_stride );
		l_pVtxBuffer->Resize( sizeof( l_pBuffer ) );
		uint8_t * l_buffer = l_pVtxBuffer->data();
		std::memcpy( l_buffer, l_pBuffer, sizeof( l_pBuffer ) );

		for ( auto && l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
			l_buffer += l_stride;
		}

		m_pGeometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );

		m_viewport = std::make_shared< Viewport >( *m_renderSystem->GetOwner(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
		m_viewport->SetLeft( real( 0.0 ) );
		m_viewport->SetRight( real( 1.0 ) );
		m_viewport->SetTop( real( 1.0 ) );
		m_viewport->SetBottom( real( 0.0 ) );
		m_viewport->SetNear( real( 0.0 ) );
		m_viewport->SetFar( real( 1.0 ) );
	}

	RenderTechnique::~RenderTechnique()
	{
		m_viewport.reset();
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
		bool l_bReturn = m_lightPassFrameBuffer->Create( 0 );

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			l_bReturn = m_lightPassTextures[i]->Create();
			m_lightPassTextures[i]->SetSampler( m_sampler );
		}

		l_bReturn &= m_lightPassBufDepth->Create();

		if ( l_bReturn )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				m_lightPassShaderProgram->CreateFrameVariable( g_strNames[i], eSHADER_TYPE_PIXEL )->SetValue( m_lightPassTextures[i].get() );
			}

			m_lightPassMatrices = GetOwner()->GetShaderManager().CreateMatrixBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = GetOwner()->GetShaderManager().CreateSceneBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_lightPassScene = l_scene;

			m_pGeometryBuffers->Create();

			for ( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
			{
				eSHADER_MODEL l_eModel = eSHADER_MODEL( i );

				if ( m_renderSystem->CheckSupport( l_eModel ) )
				{
					m_lightPassShaderProgram->SetSource( eSHADER_TYPE_VERTEX,	l_eModel, DoGetLightPassVertexShaderSource( 0 ) );
					m_lightPassShaderProgram->SetSource( eSHADER_TYPE_PIXEL,	l_eModel, DoGetLightPassPixelShaderSource( 0 ) );
				}
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pGeometryBuffers->Destroy();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i]->Destroy();
		}

		m_lightPassBufDepth->Destroy();
		m_lightPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			m_lightPassTextures[i]->SetDimension( eTEXTURE_DIMENSION_2D );

			if ( i != eDS_TEXTURE_POSITION )
			{
				m_lightPassTextures[i]->SetImage( m_size, ePIXEL_FORMAT_A8R8G8B8 );
			}
			else
			{
				m_lightPassTextures[i]->SetImage( m_size, ePIXEL_FORMAT_ARGB32F );
			}

			m_lightPassTextures[i]->Initialise( p_index++ );
		}

		m_lightPassBufDepth->Initialise( m_size );

		if ( m_lightPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				l_bReturn &= m_lightPassTexAttachs[i]->Attach( eATTACHMENT_POINT( eATTACHMENT_POINT_COLOUR0 + i ), m_lightPassFrameBuffer, eTEXTURE_TARGET_2D );
			}

			l_bReturn &= m_lightPassDepthAttach->Attach( eATTACHMENT_POINT_DEPTH, m_lightPassFrameBuffer );
			m_lightPassFrameBuffer->Unbind();
		}

		m_geometryPassDsState->Initialise();
		m_lightPassDsState->Initialise();

		m_lightPassShaderProgram->Initialise();
		m_lightPassMatrices = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );
		FrameVariableBufferSPtr l_scene = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferScene );
		l_scene->GetVariable( ShaderProgramBase::CameraPos, m_pShaderCamera );
		m_lightPassScene = l_scene;
		m_pGeometryBuffers->Initialise( m_lightPassShaderProgram, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_pShaderCamera.reset();
		m_geometryPassDsState->Cleanup();
		m_lightPassDsState->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_lightPassShaderProgram->Cleanup();
		m_lightPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTexAttachs[i]->Detach();
		}

		m_lightPassDepthAttach->Detach();
		m_lightPassFrameBuffer->Unbind();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i]->Cleanup();
		}

		m_lightPassBufDepth->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_lightPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );;
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_geometryPassDsState->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		Size const & l_size = m_pRenderTarget->GetSize();
		Size l_halfSize( l_size.width() / 2, l_size.height() / 2 );
		Pipeline & l_pipeline = m_renderSystem->GetPipeline();
		ContextRPtr l_pContext = m_renderSystem->GetCurrentContext();
		m_lightPassFrameBuffer->Unbind();

		if ( m_viewport )
		{
			bool l_bReturn = true;
			m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
			m_lightPassDsState->Apply();
			//m_pRenderTarget->GetDepthStencilState()->Apply();
			m_pRenderTarget->GetRasteriserState()->Apply();
			//m_pRenderTarget->GetRenderer()->BeginScene();
			l_pContext->SetClearColour( m_renderSystem->GetTopScene()->GetBackgroundColour() );
			l_pContext->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
			m_viewport->SetSize( m_size );
			m_viewport->Render();
			l_pContext->CullFace( eFACE_BACK );

			if ( m_pShaderCamera )
			{
				Point3r l_position = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
				m_pShaderCamera->SetValue( l_position );
				m_lightPassShaderProgram->Bind( 0, 1 );
				l_pipeline.ApplyMatrices( *m_lightPassMatrices.lock(), 0xFFFFFFFFFFFFFFFF );

#if DEBUG_BUFFERS

				int l_width = int( m_size.width() );
				int l_height = int( m_size.height() );
				int l_thirdWidth = int( l_width / 3.0f );
				int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
				int l_halfHeight = int( l_height / 2.0f );
				m_lightPassTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, 0, l_thirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_lightPassTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_lightPassTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, 0, l_width, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_lightPassTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, l_halfHeight, l_thirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_lightPassTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, l_halfHeight, l_twoThirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_lightPassTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, l_halfHeight, l_width, l_height ), eINTERPOLATION_MODE_LINEAR );

#else

				for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
				{
					l_bReturn = m_lightPassTextures[i]->Bind();
				}

				if ( l_bReturn )
				{
					m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, m_lightPassShaderProgram, m_arrayVertex.size(), 0 );

					for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
					{
						m_lightPassTextures[i]->Unbind();
					}
				}

#endif

				m_lightPassShaderProgram->Unbind();
			}

			//m_pRenderTarget->EndScene();
			m_pFrameBuffer->Unbind();
		}
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_uiFlags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlPixelShaderSource( p_uiFlags );
		}
#endif
#if C3D_HAS_D3D11_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11PixelShaderSource( p_uiFlags );
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

	String RenderTechnique::DoGetLightPassPixelShaderSource( uint32_t p_uiFlags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassPixelShaderSource( p_uiFlags );
		}

#endif
#if C3D_HAS_D3D11_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11LightPassPixelShaderSource( p_uiFlags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

#if C3D_HAS_GL_RENDERER

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_uiFlags )const
	{
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

		UNIFORM( l_writer, Sampler1D, c3d_sLights );
		Sampler2D c3d_mapColour;
		Sampler2D c3d_mapAmbient;
		Sampler2D c3d_mapDiffuse;
		Sampler2D c3d_mapNormal;
		Sampler2D c3d_mapOpacity;
		Sampler2D c3d_mapSpecular;
		Sampler2D c3d_mapHeight;
		Sampler2D c3d_mapGloss;

		Lighting< BlinnPhongLightingModel > l_lighting;
		l_lighting.Declare_Light( l_writer );
		l_lighting.Declare_GetLight( l_writer );
		l_lighting.Declare_ComputeLightDirection( l_writer );
		l_lighting.Declare_ComputeFresnel( l_writer );

		if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
		{
			l_lighting.Declare_Bump( l_writer );
		}

		if ( p_uiFlags != 0 )
		{
			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				c3d_mapColour = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				c3d_mapAmbient = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				c3d_mapNormal = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormal" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				c3d_mapOpacity = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapOpacity" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				c3d_mapHeight = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapHeight" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				c3d_mapGloss = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapGloss" ) );
			}
		}
		
		uint32_t l_index = 0;
		FRAG_OUTPUT( l_writer, Vec4, out_c3dPosition, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dDiffuse, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dNormal, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dTangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dBitangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dSpecular, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dEmissive, l_index++ );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v3Position, vtx_vertex.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( vec3( vtx_tangent.x(), vtx_tangent.y(), vtx_tangent.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, normalize( vec3( vtx_bitangent.x(), vtx_bitangent.y(), vtx_bitangent.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, vec4( Float( 0.0f ), 0, 0, 0 ) );
			//LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( c3d_v3CameraPosition - vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.xyz() );
			l_v3Diffuse = c3d_v4MatDiffuse;

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_v3Diffuse *= texture2D( c3d_mapColour, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz();
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				l_v3Diffuse *= texture2D( c3d_mapAmbient, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz();
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				l_v3Diffuse *= texture2D( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz();
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				l_v3Normal += normalize( texture2D( c3d_mapNormal, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() * 2.0 - 1.0 );
				l_v3Tangent -= l_v3Normal * dot( l_v3Tangent, l_v3Normal );
				l_v3Bitangent = cross( l_v3Normal, l_v3Tangent );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_fAlpha = texture2D( c3d_mapOpacity, vec2( vtx_texture.x(), vtx_texture.y() ) ).r() * c3d_fMatOpacity;
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				l_v4Specular.xyz() *= texture2D( c3d_mapSpecular, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz();
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapHeight, texture2D( c3d_mapHeight, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				l_v4Specular.w() *= texture2D( c3d_mapGloss, vec2( vtx_texture.x(), vtx_texture.y() ) ).x();
			}

			out_c3dPosition = vec4( l_v3Position, 1 );
			out_c3dDiffuse = vec4( l_v3Diffuse, 1 );
			out_c3dNormal = vec4( l_v3Normal, 1 );
			out_c3dTangent = vec4( l_v3Tangent, 1 );
			out_c3dBitangent = vec4( l_v3Bitangent, 1 );
			out_c3dSpecular = vec4( l_v4Specular );
			out_c3dEmissive = vec4( l_v3Emissive );
		};

		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetGlLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// Vertex inputs
		ATTRIBUTE( l_writer, Vec4, vertex );
		ATTRIBUTE( l_writer, Vec3, texture );

		// UBOs
		UBO_MATRIX( l_writer );

		// Outputs
		OUT( l_writer, Vec3, vtx_texture );
		l_writer << Legacy_MatrixOut();

		std::function< void() > l_main = [&]()
		{
			l_writer << Legacy_MatrixCopy();
			vtx_texture = texture;
			LOCALE_ASSIGN( l_writer, Vec4, position, c3d_mtxProjectionModelView * vertex );
			BUILTIN( l_writer, Vec4, gl_Position ) = vec4( position.x(), position.y(), position.z(), position.w() );
		};

		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetGlLightPassPixelShaderSource( uint32_t p_uiFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		UNIFORM( l_writer, Sampler1D, c3d_sLights );
		UNIFORM( l_writer, Sampler2D, c3d_mapPosition );
		UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
		UNIFORM( l_writer, Sampler2D, c3d_mapNormals );
		UNIFORM( l_writer, Sampler2D, c3d_mapTangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapBitangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapSpecular );
		UNIFORM( l_writer, Sampler2D, c3d_mapEmissive );

		IN( l_writer, Vec2, vtx_texture );

		LAYOUT( l_writer, Vec4, pxl_v4FragColor );

		Lighting< BlinnPhongLightingModel > l_lighting;
		l_lighting.Declare_Light( l_writer );
		l_lighting.Declare_GetLight( l_writer );
		l_lighting.Declare_ComputeLightDirection( l_writer );
		l_lighting.Declare_ComputeFresnel( l_writer );
		l_lighting.Declare_Bump( l_writer );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Positions, texture2D( c3d_mapPosition, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Diffuses, texture2D( c3d_mapDiffuse, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normals, texture2D( c3d_mapNormals, vtx_texture ) / 2.0 );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangents, texture2D( c3d_mapTangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangents, texture2D( c3d_mapBitangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Speculars, texture2D( c3d_mapSpecular, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Emissives, texture2D( c3d_mapEmissive, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Float, l_fShininess, l_v4Speculars.w() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, l_v4Positions.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_v4Normals.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, l_v4Bitangents.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_v4Tangents.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, l_v4Emissives.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpVec, neg( l_v3Position ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, vec3( dot( l_v3TmpVec, l_v3Tangent ), dot( l_v3TmpVec, l_v3Bitangent ), dot( l_v3TmpVec, l_v3Normal ) ) );
			l_v3EyeVec = normalize( l_v3EyeVec );

			FOR( l_writer, Int, i, 0, cuT( "i < c3d_iLightsCount" ), cuT( "++i" ) )
			{
				LOCALE_ASSIGN( l_writer, GLSL::Light, l_light, l_lighting.GetLight( i ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4LightDir, l_lighting.ComputeLightDirection( l_light, l_v3Position, c3d_mtxModelView ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3LightDir, l_v4LightDir.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fAttenuation, l_v4LightDir.w() );
				l_lighting.Bump( l_v3Tangent, l_v3Bitangent, l_v3Normal, l_v3LightDir, l_fAttenuation );
				LOCALE_ASSIGN( l_writer, Float, l_fLambert, max( Float( &l_writer, 0 ), dot( l_v3Normal, l_v3LightDir ) ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v3MatSpecular, l_v4Speculars.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fFresnel, l_lighting.ComputeFresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_fShininess, l_v3MatSpecular ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpDiffuse, l_light.m_v4Diffuse().xyz() * l_v4Diffuses.xyz() * l_fLambert );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpSpecular, l_light.m_v4Specular().xyz() * l_v3MatSpecular * l_fFresnel );
				l_v3Diffuse += l_v3TmpDiffuse * l_fAttenuation;
				l_v3Specular += l_v3TmpSpecular * l_fAttenuation;
			}
			ROF

			pxl_v4FragColor = vec4( l_v3Emissive + l_v3Diffuse + l_v3Specular, 1 );
		};

		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

#endif
#if C3D_HAS_D3D11_RENDERER

	String RenderTechnique::DoGetD3D11PixelShaderSource( uint32_t p_uiFlags )const
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
