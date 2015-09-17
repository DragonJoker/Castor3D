#include "DirectRenderTechnique.hpp"

#include <RenderTarget.hpp>
#include <FrameBuffer.hpp>
#include <DepthStencilState.hpp>
#include <RasteriserState.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <DepthStencilRenderBuffer.hpp>

#include <Logger.hpp>

#if C3D_HAS_GL_RENDERER
#	include <GlPixelShaderSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;
#endif

#if C3D_HAS_D3D11_RENDERER
#	include <Dx11RenderSystem.hpp>
using namespace Dx11Render;
#endif

using namespace Castor;
using namespace Castor3D;

namespace Direct
{
#if C3D_HAS_D3D11_RENDERER
	class PixelShaderSource
	{
	private:
		String m_strDeclarations;
		String m_strLights;
		String m_strMainDeclarations;
		String m_strMainLightsLoop;
		String m_strMainLightsLoopAfterLightDir;
		String m_strMainLightsLoopEnd;
		String m_strMainEnd;
		String m_strOutput;

	public:
		PixelShaderSource()
		{
			m_strDeclarations =
				cuT( "Texture2D c3d_sLights: register( t0 );\n" )
				cuT( "SamplerState LightsSampler: register( s0 );\n" )
				cuT( "SamplerState DefaultSampler\n" )
				cuT( "{\n" )
				cuT( "	AddressU = WRAP;\n" )
				cuT( "	AddressV = WRAP;\n" )
				cuT( "	AddressW = WRAP;\n" )
				cuT( "	MipFilter = NONE;\n" )
				cuT( "	MinFilter = LINEAR;\n" )
				cuT( "	MagFilter = LINEAR;\n" )
				cuT( "};\n" );
			m_strLights =
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
				cuT( "	l_lightReturn.m_v4Ambient = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (0 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Diffuse = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (1 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Specular = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (2 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v4Position = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (3 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	l_lightReturn.m_v3Attenuation = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (4 * 0.001) + 0.0005, 0.0 ) ).xyz;\n" )
				cuT( "	float4 l_v4A = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (5 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	float4 l_v4B = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (6 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	float4 l_v4C = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (7 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	float4 l_v4D = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (8 * 0.001) + 0.0005, 0.0 ) );\n" )
				cuT( "	float2 l_v2Spot = c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (9 * 0.001) + 0.0005, 0.0 ) ).xy;\n" )
				cuT( "	l_lightReturn.m_iType = int( l_lightReturn.m_v4Position.w );\n" )
				cuT( "	l_lightReturn.m_mtx4Orientation = float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n" )
				cuT( "	l_lightReturn.m_fExponent = l_v2Spot.x;\n" )
				cuT( "	l_lightReturn.m_fCutOff = l_v2Spot.x;\n" )
				cuT( "	return l_lightReturn;\n" )
				cuT( "}\n" );
			m_strMainDeclarations =
				cuT( "float4 mainPx( in PxInput p_input ) : [PxlOutput]\n" )
				cuT( "{\n" )
				cuT( "	float4 l_v4Return = float4( 0, 0, 0, 0 );\n" )
				cuT( "	Light l_light;\n" )
				cuT( "	float3 l_v3LightDir;\n" )
				cuT( "	float l_fLambert;\n" )
				cuT( "	float l_fSpecular;\n" )
				cuT( "	float3 l_v3Normal = p_input.Normal;\n" )
				cuT( "	float4 l_v4Ambient = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4Diffuse = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4Specular = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4TmpAmbient = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4TmpDiffuse = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float4 l_v4TmpSpecular = float4( 0, 0, 0, 0 );\n" )
				cuT( "	float3 l_v3EyeVec = normalize( p_input.Eye );\n" )
				cuT( "	float l_fAlpha = c3d_fMatOpacity;\n" );
			m_strMainLightsLoop =
				cuT( "	for( int i = 0; i < c3d_iLightsCount; i++ )\n" )
				cuT( "	{\n" )
				cuT( "		l_light = GetLight( i );\n" )
				cuT( "		l_v3LightDir = normalize( mul( l_light.m_v4Position, c3d_mtxModelView ).xyz - p_input.Vertex );\n" );
			m_strMainLightsLoopAfterLightDir =
				cuT( "		l_fLambert = max( dot( l_v3Normal, l_v3LightDir ), 0.0 );\n" )
				cuT( "		l_fSpecular = pow( clamp( dot( reflect( -l_v3LightDir, l_v3Normal ), l_v3EyeVec ), 0.0, 1.0 ), c3d_fMatShininess );\n" )
				cuT( "		l_v4TmpAmbient = l_light.m_v4Ambient * c3d_v4MatAmbient;\n" )
				cuT( "		l_v4TmpDiffuse = l_light.m_v4Diffuse * c3d_v4MatDiffuse * l_fLambert;\n" )
				cuT( "		l_v4TmpSpecular = l_light.m_v4Specular * c3d_v4MatSpecular * l_fSpecular;\n" );
			m_strMainLightsLoopEnd =
				cuT( "		l_v4Ambient += l_v4TmpAmbient;\n" )
				cuT( "		l_v4Diffuse += l_v4TmpDiffuse;\n" )
				cuT( "		l_v4Specular += l_v4TmpSpecular;\n" )
				cuT( "	}\n" );
			m_strMainEnd =
				cuT( "	return l_v4Return;\n" )
				cuT( "}\n" );
			m_strOutput =
				cuT( "l_v4Return" );
		}

	public:
		virtual String const & GetDeclarations()
		{
			return m_strDeclarations;
		}
		virtual String const & GetLights()
		{
			return m_strLights;
		}
		virtual String const & GetMainDeclarations()
		{
			return m_strMainDeclarations;
		}
		virtual String const & GetMainLightsLoop()
		{
			return m_strMainLightsLoop;
		}
		virtual String const & GetMainLightsLoopAfterLightDir()
		{
			return m_strMainLightsLoopAfterLightDir;
		}
		virtual String const & GetMainLightsLoopEnd()
		{
			return m_strMainLightsLoopEnd;
		}
		virtual String const & GetMainEnd()
		{
			return m_strMainEnd;
		}
		virtual String const & GetOutput()
		{
			return m_strOutput;
		}
	};

	PixelShaderSource g_ps;
#endif

	RenderTechnique::RenderTechnique()
		: RenderTechniqueBase( cuT( "direct" ) )
	{
	}

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "direct" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		return true;
	}

	void RenderTechnique::DoDestroy()
	{
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return true;
	}

	void RenderTechnique::DoCleanup()
	{
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_pFrameBuffer->Unbind();
	}

	String RenderTechnique::DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER
		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlVertexShaderSource( p_uiProgramFlags );
		}
#endif

#if C3D_HAS_D3D11_RENDERER
		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			return DoGetD3D11VertexShaderSource( p_uiProgramFlags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
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

#if C3D_HAS_GL_RENDERER
	String RenderTechnique::DoGetGlVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();
		// Vertex inputs
		ATTRIBUTE( l_writer, Vec4, vertex );
		ATTRIBUTE( l_writer, Vec3, normal );
		ATTRIBUTE( l_writer, Vec3, tangent );
		ATTRIBUTE( l_writer, Vec3, bitangent );
		ATTRIBUTE( l_writer, Vec3, texture );
		IVec4 bone_ids;
		Vec4 weights;
		Mat4 transform;

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			bone_ids = l_writer.GetAttribute< IVec4 >( cuT( "bone_ids" ) );
			weights = l_writer.GetAttribute< Vec4 >( cuT( "weights" ) );
		}

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			transform = l_writer.GetAttribute< Mat4 >( cuT( "transform" ) );
		}

		UBO_MATRIX( l_writer );

		// Outputs
		OUT( l_writer, Vec3, vtx_vertex );
		OUT( l_writer, Vec3, vtx_normal );
		OUT( l_writer, Vec3, vtx_tangent );
		OUT( l_writer, Vec3, vtx_bitangent );
		OUT( l_writer, Vec3, vtx_texture );
		l_writer << Legacy_MatrixOut();

		std::function< void() > l_main = [&]()
		{
			l_writer << Legacy_MatrixCopy();
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Vertex, vec4( vertex.xyz(), 1.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, vec4( normal, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, vec4( tangent, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, vec4( bitangent, 0.0 ) );

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
			{
				LOCALE( l_writer, Mat4, l_mtxBoneTransform );
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 0 )]] * weights[Int( 0 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 1 )]] * weights[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 2 )]] * weights[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 3 )]] * weights[Int( 3 )];
				//l_mtxBoneTransform = transpose( l_mtxBoneTransform );
				l_v4Vertex = l_mtxBoneTransform * l_v4Vertex;
				l_v4Normal = l_mtxBoneTransform * l_v4Normal;
				l_v4Tangent = l_mtxBoneTransform * l_v4Tangent;
				l_v4Bitangent = l_mtxBoneTransform * l_v4Bitangent;
			}

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxMV, c3d_mtxView * transform );
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxN, transpose( inverse( l_mtxMV ) ) );
				l_v4Vertex = l_mtxMV * l_v4Vertex;
				l_v4Normal = l_mtxN * l_v4Normal;
				l_v4Tangent = l_mtxN * l_v4Tangent;
				l_v4Bitangent = l_mtxN * l_v4Bitangent;
			}
			else
			{
				l_v4Vertex = c3d_mtxModelView * l_v4Vertex;
				l_v4Normal = c3d_mtxNormal * l_v4Normal;
				l_v4Tangent = c3d_mtxNormal * l_v4Tangent;
				l_v4Bitangent = c3d_mtxNormal * l_v4Bitangent;
			}

			vtx_texture = texture;
			vtx_vertex = l_v4Vertex.xyz();
			vtx_normal = normalize( l_v4Normal.xyz() );
			vtx_tangent = normalize( l_v4Tangent.xyz() );
			vtx_bitangent = normalize( l_v4Bitangent.xyz() );
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_uiFlags )const
	{
		using namespace GlRender::GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
		l_writer << GLSL::Version() << Endl();

		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Pixel inputs
		IN( l_writer, Vec3, vtx_vertex );
		IN( l_writer, Vec3, vtx_normal );
		IN( l_writer, Vec3, vtx_tangent );
		IN( l_writer, Vec3, vtx_bitangent );
		IN( l_writer, Vec3, vtx_texture );

		LAYOUT( l_writer, Vec4, pxl_v4FragColor );
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

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( 0.0f ), 0, 0 ) );
			//LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( c3d_v3CameraPosition - vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.xyz() );
			pxl_v4FragColor = vec4( Float( &l_writer, 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapSpecular( &l_writer, cuT( "l_v3MapSpecular" ) );

			if ( p_uiFlags != 0 )
			{
				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.xy() ).xyz() );
					l_v3Normal = normalize( l_v3MapNormal.xyz() * Float( 2.0f ) - Float( 1.0f ) );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, texture2D( c3d_mapSpecular, vtx_texture.xy() ).xyz() );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					l_fShininess = texture2D( c3d_mapGloss, vtx_texture.xy() ).r();
				}
			}

			FOR( l_writer, Int, i, 0, cuT( "i < c3d_iLightsCount" ), cuT( "++i" ) )
			{
				LOCALE_ASSIGN( l_writer, GLSL::Light, l_light, l_lighting.GetLight( i ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4LightDir, l_lighting.ComputeLightDirection( l_light, vtx_vertex, c3d_mtxModelView ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3LightDir, l_v4LightDir.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fAttenuation, l_v4LightDir.w() );

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					l_lighting.Bump( vtx_tangent, vtx_bitangent, vtx_normal, l_v3LightDir, l_fAttenuation );
				}

				LOCALE_ASSIGN( l_writer, Float, l_fLambert, max( dot( l_v3Normal, l_v3LightDir ), 0.0 ) );
				//l_fLambert = dot( l_v3Normal, -l_v3LightDir );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MatSpecular, c3d_v4MatSpecular.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fFresnel, l_lighting.ComputeFresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_fShininess, l_v3MatSpecular ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpAmbient, ( l_light.m_v4Ambient().xyz() * l_light.m_v4Ambient().w() * c3d_v4MatAmbient.xyz() ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpDiffuse, ( l_light.m_v4Diffuse().xyz() * l_light.m_v4Diffuse().w() * c3d_v4MatDiffuse.xyz() * l_fLambert ) / l_fAttenuation );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpSpecular, ( l_light.m_v4Specular().xyz() * l_light.m_v4Specular().w() * l_v3MatSpecular * l_fFresnel ) / l_fAttenuation );

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					l_v3TmpSpecular = l_fAttenuation * l_light.m_v4Specular().xyz() * l_v3MapSpecular * l_v3TmpSpecular;
				}

				l_v3Ambient += l_v3TmpAmbient;
				l_v3Diffuse += l_v3TmpDiffuse;
				l_v3Specular += l_v3TmpSpecular;
			}
			ROF

			if ( p_uiFlags != 0 )
			{
				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					l_fAlpha = texture2D( c3d_mapOpacity, vtx_texture.xy() ).r() * c3d_fMatOpacity;
				}
			}

			//pxl_v4FragColor = vec4( l_v3Emissive + l_v3Ambient + l_v3Diffuse + l_v3Specular, l_fAlpha );
			pxl_v4FragColor = vec4( l_v3Diffuse, l_fAlpha );
		};
		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
#endif

#if C3D_HAS_D3D11_RENDERER
	String RenderTechnique::DoGetD3D11VertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		String l_strReturn;
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( m_renderSystem );
		std::unique_ptr< UniformsBase > l_pUniforms = UniformsBase::Get( *l_renderSystem );
		std::unique_ptr< InOutsBase > l_pInputs = InOutsBase::Get( *l_renderSystem );

		if ( l_pUniforms && l_pInputs )
		{
			l_strReturn += l_pUniforms->GetVertexInMatrices( 0 );
			l_strReturn += l_pInputs->GetVtxInput();
			l_strReturn += l_pInputs->GetVtxOutput();
			l_strReturn +=
				cuT( "VtxOutput mainVx( in VtxInput p_input )\n" )
				cuT( "{\n" )
				cuT( "	VtxOutput l_output;\n" )
				cuT( "	p_input.Position.w = 1.0f;\n" )
				cuT( "	float3x3 l_mtxNormal = (float3x3)c3d_mtxNormal;\n" )
				cuT( "	l_output.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n" )
				cuT( "	l_output.Normal = normalize( mul( p_input.Normal, l_mtxNormal ) );\n" )
				cuT( "	l_output.Tangent = normalize( mul( p_input.Tangent, l_mtxNormal ) );\n" )
				cuT( "	l_output.Binormal = cross( l_output.Tangent, l_output.Normal );\n" )
				cuT( "	l_output.Vertex = mul( p_input.Position, c3d_mtxModelView ).xyz;\n" )
				cuT( "	l_output.TextureUV = p_input.TextureUV;\n" )
				cuT( "	float3 tmpVec = -l_output.Vertex;\n" )
				cuT( "	l_output.Eye.x = dot( tmpVec, l_output.Tangent );\n" )
				cuT( "	l_output.Eye.y = dot( tmpVec, l_output.Binormal );\n" )
				cuT( "	l_output.Eye.z = dot( tmpVec, l_output.Normal  );\n" )
				cuT( "	return l_output;\n" )
				cuT( "}\n" );
		}

		return l_strReturn;
	}

	String RenderTechnique::DoGetD3D11PixelShaderSource( uint32_t p_uiFlags )const
	{
		String l_strReturn;
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( m_renderSystem );
		std::unique_ptr< UniformsBase > l_pUniforms = UniformsBase::Get( *l_renderSystem );
		std::unique_ptr< InOutsBase > l_pInputs = InOutsBase::Get( *l_renderSystem );

		if ( l_pUniforms && l_pInputs )
		{
			String l_strOutput = g_ps.GetOutput();
			String l_strDeclarations = g_ps.GetDeclarations();
			String l_strLights = g_ps.GetLights();
			String l_strMainDeclarations = g_ps.GetMainDeclarations();
			String l_strMainLightsLoop = g_ps.GetMainLightsLoop();
			String l_strMainLightsLoopEnd = g_ps.GetMainLightsLoopEnd();
			String l_strMainEnd = g_ps.GetMainEnd();
			String l_strMainLightsLoopAfterLightDir = g_ps.GetMainLightsLoopAfterLightDir();
			l_strDeclarations += l_pUniforms->GetPixelInMatrices( 0 );
			l_strDeclarations += l_pUniforms->GetPixelScene( 1 );
			l_strDeclarations += l_pUniforms->GetPixelPass( 2 );
			l_strDeclarations += l_pInputs->GetPxlInput();
			l_strMainDeclarations = string::replace( l_strMainDeclarations, cuT( "[PxlOutput]" ), l_pInputs->GetPxlOutput() );

			if ( p_uiFlags )
			{
				int l_iIndex = 1;

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapColour: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapColour = c3d_mapColour.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapColour;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapAmbient: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapAmbient = c3d_mapAmbient.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapAmbient;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapDiffuse: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapDiffuse = c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapDiffuse;\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapNormal: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapNormal = c3d_mapNormal.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainDeclarations += cuT( "	float l_fSqrLength;\n" );
					l_strMainDeclarations += cuT( "	float l_fAttenuation;\n" );
					l_strMainDeclarations += cuT( "	float l_fInvRadius = 0.02;\n" );
					l_strMainDeclarations += cuT( "	l_v3Normal = normalize( l_v4MapNormal.xyz * 2.0 - 1.0 );\n" );
					//l_strMainLightsLoop += cuT( "		l_v3LightDir = (vtx_mtxView * vec4( l_v3LightDir, 1 )).xyz;\n" );
					l_strMainLightsLoop += cuT( "		l_v3LightDir = float3( dot( l_v3LightDir, p_input.Tangent ), dot( l_v3LightDir, p_input.Binormal ), dot( l_v3LightDir, p_input.Normal ) );\n" );
					l_strMainLightsLoop += cuT( "		l_fSqrLength = dot( l_v3LightDir, l_v3LightDir );\n" );
					l_strMainLightsLoop += cuT( "		l_v3LightDir = l_v3LightDir * rsqrt( l_fSqrLength );\n" );
					l_strMainLightsLoop += cuT( "		l_fAttenuation = clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" );
					l_strMainLightsLoopAfterLightDir += cuT( "		l_v4Ambient *= l_fAttenuation;\n" );
					l_strMainLightsLoopAfterLightDir += cuT( "		l_v4Diffuse *= l_fAttenuation;\n" );
					l_strMainLightsLoopAfterLightDir += cuT( "		l_v4Specular *= l_fAttenuation;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapOpacity: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapOpacity = c3d_mapOpacity.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapSpecular: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapSpecular = c3d_mapSpecular.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Specular *= l_v4MapSpecular;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapHeight: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapHeight = c3d_mapHeight.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapGloss: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapGloss = c3d_mapGloss.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				}
			}

			l_strMainLightsLoopEnd	+= cuT( "	" ) + l_strOutput + cuT( " = float4( ( l_v4Ambient + l_v4Diffuse + l_v4Specular ).xyz, l_fAlpha );\n" );
			l_strReturn = l_strDeclarations + l_strLights + l_strMainDeclarations + l_strMainLightsLoop + l_strMainLightsLoopAfterLightDir + l_strMainLightsLoopEnd + l_strMainEnd;
			//Logger::LogDebug( l_strReturn );
		}

		return l_strReturn;
	}
#endif
}
