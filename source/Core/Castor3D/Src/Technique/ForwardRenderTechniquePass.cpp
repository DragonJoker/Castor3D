#include "ForwardRenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>
#include <GlslUtils.hpp>

using namespace Castor;

namespace Castor3D
{
	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_opaque
		, bool p_multisampling
		, bool p_environment
		, SceneNode const * p_ignored )
		: RenderTechniquePass{ p_name
			, p_scene
			, p_camera
			, p_opaque
			, p_multisampling
			, p_environment
			, p_ignored }
		, m_directionalShadowMap{ *p_scene.GetEngine() }
		, m_spotShadowMap{ *p_scene.GetEngine() }
		, m_pointShadowMap{ *p_scene.GetEngine() }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	void ForwardRenderTechniquePass::Render( RenderInfo & p_info, bool p_shadows )
	{
		m_scene.GetLightCache().BindLights();
		DoRender( p_info, p_shadows );
		m_scene.GetLightCache().UnbindLights();
	}

	bool ForwardRenderTechniquePass::InitialiseShadowMaps()
	{
		m_scene.GetLightCache().ForEach( [this]( Light & p_light )
		{
			if ( p_light.IsShadowProducer() )
			{
				switch ( p_light.GetLightType() )
				{
				case LightType::eDirectional:
					m_directionalShadowMap.AddLight( p_light );
					break;

				case LightType::ePoint:
					m_pointShadowMap.AddLight( p_light );
					break;

				case LightType::eSpot:
					m_spotShadowMap.AddLight( p_light );
					break;
				}
			}
		} );

		bool l_result = m_directionalShadowMap.Initialise();

		if ( l_result )
		{
			l_result = m_spotShadowMap.Initialise();
		}

		if ( l_result )
		{
			l_result = m_pointShadowMap.Initialise();
		}

		ENSURE( l_result );
		return l_result;
	}

	void ForwardRenderTechniquePass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void ForwardRenderTechniquePass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_camera, p_queues );
		m_spotShadowMap.Update( *m_camera, p_queues );
		m_directionalShadowMap.Update( *m_camera, p_queues );
	}

	void ForwardRenderTechniquePass::RenderShadowMaps()
	{
		m_directionalShadowMap.Render();
		m_pointShadowMap.Render();
		m_spotShadowMap.Render();
	}

	void ForwardRenderTechniquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_directionalShadowMap.GetTexture() ) );
		p_depthMaps.push_back( std::ref( m_spotShadowMap.GetTexture() ) );

		for ( auto & l_map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( l_map ) );
		}
	}

	String ForwardRenderTechniquePass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_MODEL( l_writer );

		// Fragment Intputs
		auto vtx_position = l_writer.GetInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.GetInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.GetInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		LegacyMaterials l_materials{ l_writer };
		l_materials.Declare();

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_fresnelBias = l_writer.GetUniform< Float >( cuT( "c3d_fresnelBias" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = l_writer.GetUniform< Float >( cuT( "c3d_fresnelScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = l_writer.GetUniform< Float >( cuT( "c3d_fresnelPower" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_mapEnvironment( l_writer.GetUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );

		auto c3d_fheightScale( l_writer.GetUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		GLSL::Utils l_utils{ l_writer };
		l_utils.DeclareApplyGamma();
		l_utils.DeclareRemoveGamma();

		auto l_parallaxMapping = DeclareParallaxMappingFunc( l_writer, p_textureFlags, p_programFlags );

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vtx_normal ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_materials.GetShininess( c3d_materialIndex ) );
			auto l_diffuse = l_writer.GetLocale( cuT( "l_diffuse" ), l_utils.RemoveGamma( l_materials.GetGamma( c3d_materialIndex ), l_materials.GetDiffuse( c3d_materialIndex ) ) );
			auto l_emissive = l_writer.GetLocale( cuT( "l_emissive" ), l_diffuse * l_materials.GetEmissive( c3d_materialIndex ) );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto l_envAmbient = l_writer.GetLocale( cuT( "l_envAmbient" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto l_envDiffuse = l_writer.GetLocale( cuT( "l_envDiffuse" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), vtx_texture );

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 1.0f );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto l_viewDir = -l_writer.GetLocale( cuT( "l_viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				l_texCoord.xy() = l_parallaxMapping( l_texCoord.xy(), l_viewDir );
			}

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );
			OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting( l_worldEye
				, l_fMatShininess
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, l_v3Normal )
				, l_output );
			ComputePostLightingMapContributions( l_writer, l_diffuse, l_v3Specular, l_emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			pxl_v4FragColor.xyz() = l_v3Ambient * l_diffuse
				+ l_writer.Paren( l_v3Diffuse * l_diffuse )
				+ l_writer.Paren( l_v3Specular * l_materials.GetSpecular( c3d_materialIndex ) )
				+ l_diffuse * l_emissive;

			if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
				|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
			{
				auto l_incident = l_writer.GetLocale( cuT( "l_i" ), normalize( vtx_position - c3d_v3CameraPosition ) );
				auto l_reflectedColour = l_writer.GetLocale( cuT( "l_reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto l_refractedColour = l_writer.GetLocale( cuT( "l_refractedColour" ), l_diffuse / 2.0 );

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection ) )
				{
					auto l_reflect = l_writer.GetLocale( cuT( "l_reflect" )
						, reflect( l_incident, l_v3Normal ) );
					l_reflectedColour = texture( c3d_mapEnvironment, l_reflect ).xyz() * length( pxl_v4FragColor.xyz() );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					auto l_refract = l_writer.GetLocale( cuT( "l_refract" ), refract( l_incident, l_v3Normal, l_materials.GetRefractionRatio( c3d_materialIndex ) ) );
					l_refractedColour = texture( c3d_mapEnvironment, l_refract ).xyz() * l_diffuse / length( l_diffuse );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
					&& !CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					pxl_v4FragColor.xyz() = l_reflectedColour * l_diffuse / length( l_diffuse );
				}
				else
				{
					auto l_refFactor = l_writer.GetLocale( cuT( "l_refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( l_incident, l_v3Normal ), c3d_fresnelPower ) );
					pxl_v4FragColor.xyz() = mix( l_refractedColour, l_reflectedColour, l_refFactor );
				}
			}

			if ( !m_opaque )
			{
				auto l_alpha = l_writer.GetLocale( cuT( "l_alpha" ), l_materials.GetOpacity( c3d_materialIndex ) );

				if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
				{
					l_alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				}

				pxl_v4FragColor.a() = l_alpha;
			}

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.y() );
			}
		} );

		return l_writer.Finalise();
	}
}
