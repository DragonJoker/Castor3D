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
#include <GlslPhongLighting.hpp>
#include <GlslMetallicBrdfLighting.hpp>
#include <GlslSpecularBrdfLighting.hpp>

using namespace Castor;

namespace Castor3D
{
	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_environment
		, SceneNode const * p_ignored
		, SsaoConfig const & p_config )
		: RenderTechniquePass{ p_name
			, p_scene
			, p_camera
			, p_environment
			, p_ignored
			, p_config }
		, m_directionalShadowMap{ *p_scene.GetEngine() }
		, m_spotShadowMap{ *p_scene.GetEngine() }
		, m_pointShadowMap{ *p_scene.GetEngine() }
	{
	}

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_oit
		, bool p_environment
		, SceneNode const * p_ignored
		, SsaoConfig const & p_config )
		: RenderTechniquePass{ p_name
			, p_scene
			, p_camera
			, p_oit
			, p_environment
			, p_ignored
			, p_config }
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

	void ForwardRenderTechniquePass::AddShadowProducer( Light & p_light )
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
	}

	bool ForwardRenderTechniquePass::InitialiseShadowMaps()
	{
		m_scene.GetLightCache().ForEach( [this]( Light & p_light )
		{
			AddShadowProducer( p_light );
		} );

		bool result = m_directionalShadowMap.Initialise();

		if ( result )
		{
			result = m_spotShadowMap.Initialise();
		}

		if ( result )
		{
			result = m_pointShadowMap.Initialise();
		}

		ENSURE( result );
		return result;
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

		for ( auto & map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( map ) );
		}
	}
	

	GLSL::Shader ForwardRenderTechniquePass::DoGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.DeclAttribute< Int >( ShaderProgram::Material, CheckFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::Declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Outputs
		auto vtx_position = writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.DeclOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.DeclOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.DeclOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.DeclOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.DeclLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.DeclLocale( cuT( "v4Normal" ), vec4( normal, 0.0 ) );
			auto v4Tangent = writer.DeclLocale( cuT( "v4Tangent" ), vec4( tangent, 0.0 ) );
			auto v3Texture = writer.DeclLocale( cuT( "v3Texture" ), texture );
			auto mtxModel = writer.DeclLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( CheckFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, programFlags );

				if ( CheckFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = material;
				}
				else
				{
					vtx_material = c3d_materialIndex;
				}
			}
			else if ( CheckFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
				vtx_material = material;
			}
			else
			{
				mtxModel = c3d_mtxModel;
				vtx_material = c3d_materialIndex;
			}

			if ( CheckFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.DeclLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			v4Vertex = c3d_mtxView * v4Vertex;
			mtxModel = transpose( inverse( mtxModel ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( writer.Paren( mtxModel * -v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( writer.Paren( mtxModel * v4Normal ).xyz() );
			}

			vtx_tangent = normalize( writer.Paren( mtxModel * v4Tangent ).xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * v4Vertex;

			auto tbn = writer.DeclLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}

	GLSL::Shader ForwardRenderTechniquePass::DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.DeclInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.DeclInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.DeclInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.DeclInput< Int >( cuT( "vtx_material" ) );

		LegacyMaterials materials{ writer };
		materials.Declare();

		if ( writer.HasTextureBuffers() )
		{
			auto c3d_sLights = writer.DeclUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.DeclUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, CheckFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular
			, CheckFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight
			, CheckFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss
			, CheckFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_fresnelBias = writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" )
			, CheckFlag( textureFlags, TextureChannel::eReflection ) || CheckFlag( textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" )
			, CheckFlag( textureFlags, TextureChannel::eReflection ) || CheckFlag( textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" )
			, CheckFlag( textureFlags, TextureChannel::eReflection ) || CheckFlag( textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( textureFlags, TextureChannel::eReflection )
			|| CheckFlag( textureFlags, TextureChannel::eRefraction ) ) );

		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = legacy::CreateLightingModel( writer
			, GetShadowType( sceneFlags ) );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4FragColor( writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto v3Normal = writer.DeclLocale( cuT( "v3Normal" ), normalize( vtx_normal ) );
			auto v3Ambient = writer.DeclLocale( cuT( "v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto v3Diffuse = writer.DeclLocale( cuT( "v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto v3Specular = writer.DeclLocale( cuT( "v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto fMatShininess = writer.DeclLocale( cuT( "fMatShininess" ), materials.GetShininess( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), utils.RemoveGamma(gamma, materials.GetDiffuse( vtx_material ) ) );
			auto emissive = writer.DeclLocale( cuT( "emissive" ), diffuse * materials.GetEmissive( vtx_material ) );
			auto worldEye = writer.DeclLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.DeclLocale( cuT( "envAmbient" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto envDiffuse = writer.DeclLocale( cuT( "envDiffuse" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 1.0f );

			if ( CheckFlag( textureFlags, TextureChannel::eHeight )
				&& CheckFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			legacy::ComputePreLightingMapContributions( writer
				, v3Normal
				, fMatShininess
				, textureFlags
				, programFlags
				, sceneFlags );
			OutputComponents output{ v3Diffuse, v3Specular };
			lighting->ComputeCombinedLighting( worldEye
				, fMatShininess
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, v3Normal )
				, output );
			legacy::ComputePostLightingMapContributions( writer
				, diffuse
				, v3Specular
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );

			pxl_v4FragColor.xyz() = writer.Paren( v3Ambient + v3Diffuse + emissive ) * diffuse
				+ v3Specular * materials.GetSpecular( vtx_material );

			if ( CheckFlag( textureFlags, TextureChannel::eReflection )
				|| CheckFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.DeclLocale( cuT( "i" ), normalize( vtx_position - c3d_v3CameraPosition ) );
				auto reflectedColour = writer.DeclLocale( cuT( "reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto refractedColour = writer.DeclLocale( cuT( "refractedColour" ), diffuse / 2.0 );

				if ( CheckFlag( textureFlags, TextureChannel::eReflection ) )
				{
					auto reflected = writer.DeclLocale( cuT( "reflected" )
						, reflect( incident, v3Normal ) );
					reflectedColour = texture( c3d_mapEnvironment, reflected ).xyz() * length( pxl_v4FragColor.xyz() );
				}

				if ( CheckFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					auto refracted = writer.DeclLocale( cuT( "refracted" ), refract( incident, v3Normal, materials.GetRefractionRatio( vtx_material ) ) );
					refractedColour = texture( c3d_mapEnvironment, refracted ).xyz() * diffuse / length( diffuse );
				}

				if ( CheckFlag( textureFlags, TextureChannel::eReflection )
					&& !CheckFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					pxl_v4FragColor.xyz() = reflectedColour * diffuse / length( diffuse );
				}
				else
				{
					auto refFactor = writer.DeclLocale( cuT( "refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( incident, v3Normal ), c3d_fresnelPower ) );
					pxl_v4FragColor.xyz() = mix( refractedColour, reflectedColour, refFactor );
				}
			}

			if ( !m_opaque )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

				if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				}

				pxl_v4FragColor.a() = alpha;
			}

			if ( GetFogType( sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto wvPosition = writer.DeclLocale( cuT( "wvPosition" ), writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.ApplyFog( pxl_v4FragColor, length( wvPosition ), wvPosition.y() );
			}
		} );

		return writer.Finalise();
	}

	GLSL::Shader ForwardRenderTechniquePass::DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.DeclInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.DeclInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.DeclInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.DeclInput< Int >( cuT( "vtx_material" ) );

		PbrMRMaterials materials{ writer };
		materials.Declare();

		if ( writer.HasTextureBuffers() )
		{
			auto c3d_sLights = writer.DeclUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = writer.DeclUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapAlbedo( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAlbedo
			, CheckFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapRoughness
			, CheckFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapMetallic( writer.DeclUniform< Sampler2D >( ShaderProgram::MapMetallic
			, CheckFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( textureFlags, TextureChannel::eReflection )
			|| CheckFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::mr::CreateLightingModel( writer
			, GetShadowType( sceneFlags ) );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();
		utils.DeclareFresnelSchlick();
		utils.DeclareComputeMetallicIBL();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4FragColor( writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "v3Normal" ), normalize( vtx_normal ) );
			auto ambient = writer.DeclLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto metalness = writer.DeclLocale( cuT( "metallic" ), materials.GetMetallic( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto albedo = writer.DeclLocale( cuT( "albedo" ), utils.RemoveGamma( gamma, materials.GetAlbedo( vtx_material ) ) );
			auto roughness = writer.DeclLocale( cuT( "roughness" ), materials.GetRoughness( vtx_material ) );
			auto emissive = writer.DeclLocale( cuT( "emissive" ), albedo * materials.GetEmissive( vtx_material ) );
			auto worldEye = writer.DeclLocale( cuT( "worldEye" ), c3d_v3CameraPosition );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.DeclLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( CheckFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 1.0f );

			if ( CheckFlag( textureFlags, TextureChannel::eHeight )
				&& CheckFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::mr::ComputePreLightingMapContributions( writer
				, normal
				, metalness
				, roughness
				, textureFlags
				, programFlags
				, sceneFlags );
			pbr::mr::ComputePostLightingMapContributions( writer
				, albedo
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" )
				, lighting->ComputeCombinedLighting( worldEye
					, albedo
					, metalness
					, roughness
					, c3d_shadowReceiver
					, FragmentInput( vtx_position, normal ) ) );

			ambient *= occlusion * utils.ComputeMetallicIBL( normal
				, vtx_position
				, albedo
				, metalness
				, roughness
				, c3d_v3CameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf
				, 0_i );
			pxl_v4FragColor.xyz() = diffuse + emissive + ambient;

			if ( !m_opaque )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

				if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				}

				pxl_v4FragColor.a() = alpha;
			}

			if ( GetFogType( sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto wvPosition = writer.DeclLocale( cuT( "wvPosition" ), writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.ApplyFog( pxl_v4FragColor, length( wvPosition ), wvPosition.y() );
			}
		} );

		return writer.Finalise();
	}

	GLSL::Shader ForwardRenderTechniquePass::DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.DeclInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.DeclInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.DeclInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.DeclInput< Int >( cuT( "vtx_material" ) );

		PbrSGMaterials materials{ writer };
		materials.Declare();

		if ( writer.HasTextureBuffers() )
		{
			auto c3d_sLights = writer.DeclUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = writer.DeclUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, CheckFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular
			, CheckFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss
			, CheckFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( textureFlags, TextureChannel::eReflection )
			|| CheckFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::sg::CreateLightingModel( writer
			, GetShadowType( sceneFlags ) );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();
		utils.DeclareFresnelSchlick();
		utils.DeclareComputeSpecularIBL();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4FragColor( writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "v3Normal" ), normalize( vtx_normal ) );
			auto ambient = writer.DeclLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto specular = writer.DeclLocale( cuT( "specular" ), materials.GetSpecular( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), utils.RemoveGamma( gamma, materials.GetDiffuse( vtx_material ) ) );
			auto glossiness = writer.DeclLocale( cuT( "glossiness" ), materials.GetGlossiness( vtx_material ) );
			auto emissive = writer.DeclLocale( cuT( "emissive" ), diffuse * materials.GetEmissive( vtx_material ) );
			auto worldEye = writer.DeclLocale( cuT( "worldEye" ), c3d_v3CameraPosition );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.DeclLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( CheckFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 1.0f );

			if ( CheckFlag( textureFlags, TextureChannel::eHeight )
				&& CheckFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::sg::ComputePreLightingMapContributions( writer
				, normal
				, specular
				, glossiness
				, textureFlags
				, programFlags
				, sceneFlags );
			pbr::sg::ComputePostLightingMapContributions( writer
				, diffuse
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto light = writer.DeclLocale( cuT( "light" )
				, lighting->ComputeCombinedLighting( worldEye
					, diffuse
					, specular
					, glossiness
					, c3d_shadowReceiver
					, FragmentInput( vtx_position, normal ) ) );

			ambient *= occlusion * utils.ComputeSpecularIBL( normal
				, vtx_position
				, diffuse
				, specular
				, glossiness
				, c3d_v3CameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf
				, 0_i );
			pxl_v4FragColor.xyz() = light + emissive + ambient;

			if ( !m_opaque )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

				if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				}

				pxl_v4FragColor.a() = alpha;
			}

			if ( GetFogType( sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto wvPosition = writer.DeclLocale( cuT( "wvPosition" ), writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.ApplyFog( pxl_v4FragColor, length( wvPosition ), wvPosition.y() );
			}
		} );

		return writer.Finalise();
	}
}
