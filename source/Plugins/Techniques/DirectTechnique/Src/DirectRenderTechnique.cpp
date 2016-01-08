#include "DirectRenderTechnique.hpp"

#include <Camera.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilState.hpp>
#include <FrameBuffer.hpp>
#include <Light.hpp>
#include <MaterialManager.hpp>
#include <OverlayManager.hpp>
#include <PanelOverlay.hpp>
#include <RasteriserState.hpp>
#include <RenderTarget.hpp>
#include <RenderSystem.hpp>
#include <Scene.hpp>
#include <TextOverlay.hpp>

#include <FontManager.hpp>
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
				cuT( "Texture1D c3d_sLights: register( t0 );\n" )
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
				cuT( "	for( int i = 0; i < c3d_iLightsCount.x; i++ )\n" )
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

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "direct" ), p_renderTarget, p_renderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
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
#if !defined( NDEBUG )

		auto l_background = GetOwner()->GetMaterialManager().Find( cuT( "Gray" ) );
		auto l_foreground = GetOwner()->GetMaterialManager().Find( cuT( "DarkRed" ) );
		auto l_font = GetOwner()->GetFontManager().get( cuT( "Arial20" ) );
		auto l_panel = GetOwner()->GetOverlayManager().CreatePanel( cuT( "DirectRenderTechnique_DebugPanel" ), Position( 0, 200 ), Size( 350, 600 ), l_background );
		m_debugOverlay = GetOwner()->GetOverlayManager().CreateText( cuT( "DirectRenderTechnique_DebugText" ), Position( 5, 5 ), Size( 340, 590 ), l_foreground, l_font, l_panel->GetOverlay().shared_from_this() );
		m_debugOverlay->SetVAlign( eVALIGN_TOP );

#endif

		return true;
	}

	void RenderTechnique::DoCleanup()
	{
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_pRenderTarget->GetFrameBuffer()->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
#if !defined( NDEBUG )

		auto l_scene = GetOwner()->GetRenderSystem()->GetTopScene();
		auto l_node = l_scene->GetNode( cuT( "FinalNode" ) );
		auto l_light = l_scene->GetLight( cuT( "PointLight" ) );

		if ( l_node && l_light )
		{
			auto reflect = []( Point3f const & p_incident, Point3f const & p_normal )
			{
				return p_incident - 2.0_r * point::dot( p_normal, p_incident ) * p_normal;
			};

			Castor::StringStream l_stream;
			auto l_camera = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
			l_stream << cuT( "Camera position:\n  " ) << l_camera[0] << cuT( "\n  " ) << l_camera[1] << cuT( "\n  " ) << l_camera[2] << "\n\n";

			SceneNodeSPtr l_node = l_scene->GetNode( cuT( "FinalNode" ) );
			Point3f l_normal( 0, 0, -1 );
			Point3f l_vertex = l_node->GetDerivedPosition();
			l_normal = l_node->GetDerivedTransformationMatrix() * l_normal;
			l_stream << cuT( "Object position:\n  " ) << l_vertex[0] << cuT( "\n  " ) << l_vertex[1] << cuT( "\n  " ) << l_vertex[2] << "\n";
			l_stream << cuT( "Object normal:\n  " ) << l_normal[0] << cuT( "\n  " ) << l_normal[1] << cuT( "\n  " ) << l_normal[2] << "\n\n";

			auto l_positionType = l_light->GetPositionType();
			Point3f l_direction( l_positionType[0], l_positionType[1], l_positionType[2] );
			point::normalise( l_direction );
			l_stream << cuT( "Light direction:\n  " ) << l_direction[0] << cuT( "\n  " ) << l_direction[1] << cuT( "\n  " ) << l_direction[2] << "\n";

			float l_diffuseFactor = point::dot( l_normal, -l_direction );

			if ( l_diffuseFactor > 0 )
			{
				Point3f l_diffuse = Point3f( 0.8, 0.8, 0.8 ) * l_diffuseFactor;
				l_stream << cuT( "Light diffuse:\n  " ) << l_diffuse[0] << cuT( "\n  " ) << l_diffuse[1] << cuT( "\n  " ) << l_diffuse[2] << "\n";

				Point3f l_vertexToEye = point::get_normalised( l_camera - l_vertex );
				Point3f l_lightReflect = point::get_normalised( reflect( l_direction, l_normal ) );
				float l_specularFactor = point::dot( l_vertexToEye, l_lightReflect );

				if ( l_specularFactor > 0 )
				{
					l_specularFactor = pow( l_specularFactor, 51.2f );
					Point3f l_specular = Point3f( 1, 1, 1 ) * l_specularFactor;
					l_stream << cuT( "Light specular:\n  " ) << l_specular[0] << cuT( "\n  " ) << l_specular[1] << cuT( "\n  " ) << l_specular[2] << "\n";
				}
			}

			m_debugOverlay->SetCaption( l_stream.str() );
		}
		else
		{
			m_debugOverlay->GetOverlay().GetParent()->SetVisible( false );
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

#if C3D_HAS_GL_RENDERER

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_flags )const
	{
#define CHECK_FLAG( channel ) ( ( p_flags & ( channel ) ) == ( channel ) )

		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
		l_writer << GLSL::Version() << Endl();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		Vec3 vtx_vertex( l_writer.GetIn< Vec3 >( cuT( "vtx_vertex" ) ) );
		Vec3 vtx_normal( l_writer.GetIn< Vec3 >( cuT( "vtx_normal" ) ) );
		Vec3 vtx_tangent( l_writer.GetIn< Vec3 >( cuT( "vtx_tangent" ) ) );
		Vec3 vtx_bitangent( l_writer.GetIn< Vec3 >( cuT( "vtx_bitangent" ) ) );
		Vec3 vtx_texture( l_writer.GetIn< Vec3 >( cuT( "vtx_texture" ) ) );

		Vec4 pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		if ( l_writer.GetOpenGl().HasTbo() )
		{
			UNIFORM( l_writer, SamplerBuffer, c3d_sLights );
		}
		else
		{
			UNIFORM( l_writer, Sampler1D, c3d_sLights );
		}

		Optional< Sampler2D > c3d_mapColour( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ), CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) ) );
		Optional< Sampler2D > c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ), CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) ) );
		Optional< Sampler2D > c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ), CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) ) );
		Optional< Sampler2D > c3d_mapNormal( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormal" ), CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) ) );
		Optional< Sampler2D > c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapOpacity" ), CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) ) );
		Optional< Sampler2D > c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ), CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) ) );
		Optional< Sampler2D > c3d_mapHeight( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapHeight" ), CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) ) );
		Optional< Sampler2D > c3d_mapGloss( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapGloss" ), CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.X, vtx_normal.Y, vtx_normal.Z ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.XYZ );
			pxl_v4FragColor = vec4( Float( 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapSpecular( &l_writer, cuT( "l_v3MapSpecular" ) );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			if ( p_flags != 0 )
			{
				if ( CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.XY ).XYZ );
					l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
				{
					l_fMatShininess = texture2D( c3d_mapGloss, vtx_texture.XY ).R;
				}
			}

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetLight( i ), c3d_v3CameraPosition, l_fMatShininess,
													 FragmentInput{ vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetLight( i ), c3d_v3CameraPosition, l_fMatShininess,
											   FragmentInput{ vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetLight( i ), c3d_v3CameraPosition, l_fMatShininess,
											  FragmentInput{ vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											  l_output );
			}
			ROF;

			if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
			{
				l_fAlpha = texture2D( c3d_mapOpacity, vtx_texture.XY ).R * c3d_fMatOpacity;
			}

			if ( p_flags && p_flags != eTEXTURE_CHANNEL_OPACITY )
			{
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

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) )
				{
					l_v3Specular *= l_v3MapSpecular;
				}
			}

			pxl_v4FragColor = vec4( l_v3Emissive +
									l_writer.Paren( l_v3Ambient * c3d_v4MatAmbient.XYZ ) +
									l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.XYZ ) +
									l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.XYZ ) +
									c3d_v4MatEmissive.XYZ, l_fAlpha );
		};
		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();

#undef CHECK_FLAG
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

			if ( p_flags )
			{
				int l_iIndex = 1;

				if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapColour: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapColour = c3d_mapColour.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapColour;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapAmbient: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapAmbient = c3d_mapAmbient.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapAmbient;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapDiffuse: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapDiffuse = c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapDiffuse;\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
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

				if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapOpacity: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapOpacity = c3d_mapOpacity.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapSpecular: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapSpecular = c3d_mapSpecular.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
					l_strMainLightsLoopEnd += cuT( "	l_v4Specular *= l_v4MapSpecular;\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					StringStream l_strIndex;
					l_strIndex << l_iIndex++;
					l_strDeclarations += cuT( "Texture2D c3d_mapHeight: register( t" ) + l_strIndex.str() + cuT( " );\n" );
					l_strMainDeclarations += cuT( "	float4 l_v4MapHeight = c3d_mapHeight.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
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
