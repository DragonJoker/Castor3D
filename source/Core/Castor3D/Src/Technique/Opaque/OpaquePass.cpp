#include "OpaquePass.hpp"

#include "LightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	String const OpaquePass::Output1 = cuT( "c3d_output1" );
	String const OpaquePass::Output2 = cuT( "c3d_output2" );
	String const OpaquePass::Output3 = cuT( "c3d_output3" );
	String const OpaquePass::Output4 = cuT( "c3d_output4" );

	OpaquePass::OpaquePass( Scene & p_scene
		, Camera * p_camera
		, SsaoConfig const & p_config )
		: Castor3D::RenderTechniquePass{ cuT( "deferred_opaque" )
			, p_scene
			, p_camera
			, false
			, nullptr
			, p_config }
		, m_directionalShadowMap{ *p_scene.GetEngine() }
		, m_spotShadowMap{ *p_scene.GetEngine() }
		, m_pointShadowMap{ *p_scene.GetEngine() }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	void OpaquePass::Render( RenderInfo & p_info, bool p_shadows )
	{
		DoRender( p_info, p_shadows );
	}

	void OpaquePass::AddShadowProducer( Light & p_light )
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

	bool OpaquePass::InitialiseShadowMaps()
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

	void OpaquePass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void OpaquePass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_camera, p_queues );
		m_spotShadowMap.Update( *m_camera, p_queues );
		m_directionalShadowMap.Update( *m_camera, p_queues );
	}

	void OpaquePass::RenderShadowMaps()
	{
	}

	void OpaquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
	}

	void OpaquePass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
	}

	GLSL::Shader OpaquePass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.DeclAttribute< Int >( ShaderProgram::Material, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );
		SkinningUbo::Declare( writer, p_programFlags );
		UBO_MORPHING( writer, p_programFlags );

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
			auto p = writer.DeclLocale( cuT( "p" ), vec4( position.xyz(), 1.0 ) );
			auto n = writer.DeclLocale( cuT( "n" ), vec4( normal, 0.0 ) );
			auto t = writer.DeclLocale( cuT( "t" ), vec4( tangent, 0.0 ) );
			auto tex = writer.DeclLocale( cuT( "tex" ), texture );
			auto mtxModel = writer.DeclLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, p_programFlags );

				if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = material;
				}
				else
				{
					vtx_material = c3d_materialIndex;
				}
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
				vtx_material = material;
			}
			else
			{
				mtxModel = c3d_mtxModel;
				vtx_material = c3d_materialIndex;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.DeclLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				p = vec4( p.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				n = vec4( n.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				t = vec4( t.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				tex = tex * writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = tex;
			p = mtxModel * p;
			vtx_position = p.xyz();
			p = c3d_mtxView * p;
			auto mtxNormal = writer.DeclLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( mtxModel ) ) ) );

			if ( p_invertNormals )
			{
				vtx_normal = normalize( mtxNormal * -n.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * n.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * t.xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * p;

			auto tbn = writer.DeclLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}

	GLSL::Shader OpaquePass::DoGetLegacyPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
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

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && p_alphaFunc != ComparisonFunc::eAlways ) );
		auto c3d_mapHeight( writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment, CheckFlag( p_textureFlags, TextureChannel::eReflection ) ) );
		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );
		Declare_EncodeMaterial( writer );
		GLSL::Utils utils{ writer };
		utils.DeclareRemoveGamma();

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto v3Normal = writer.DeclLocale( cuT( "v3Normal" ), normalize( vtx_normal ) );
			auto v3Diffuse = writer.DeclLocale( cuT( "v3Diffuse" ), materials.GetDiffuse( vtx_material ) );
			auto v3Specular = writer.DeclLocale( cuT( "v3Specular" ), materials.GetSpecular( vtx_material ) );
			auto fMatShininess = writer.DeclLocale( cuT( "fMatShininess" ), materials.GetShininess( vtx_material ) );
			auto v3Emissive = writer.DeclLocale( cuT( "v3Emissive" ), v3Diffuse * materials.GetEmissive( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			legacy::ComputePreLightingMapContributions( writer
				, v3Normal
				, fMatShininess
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			legacy::ComputePostLightingMapContributions( writer
				, v3Diffuse
				, v3Specular
				, v3Emissive
				, gamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			auto flags = writer.DeclLocale( cuT( "flags" ), 0.0_f );
			EncodeMaterial( writer
				, c3d_shadowReceiver
				, CheckFlag( p_textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			if ( p_alphaFunc != ComparisonFunc::eAlways
				&& CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );

				switch ( p_alphaFunc )
				{
				case ComparisonFunc::eLess:
					IF( writer, alpha >= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eLEqual:
					IF( writer, alpha > materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eEqual:
					IF( writer, alpha != materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eNEqual:
					IF( writer, alpha == materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGEqual:
					IF( writer, alpha < materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGreater:
					IF( writer, alpha <= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;
				}
			}

			out_c3dOutput1 = vec4( v3Normal, flags );
			out_c3dOutput2 = vec4( v3Diffuse, 0.0_f );
			out_c3dOutput3 = vec4( v3Specular, fMatShininess );
			out_c3dOutput4 = vec4( v3Emissive, materials.GetRefractionRatio( vtx_material ) );
		} );

		return writer.Finalise();
	}

	GLSL::Shader OpaquePass::DoGetPbrMRPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
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

		auto c3d_mapAlbedo( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAlbedo
			, CheckFlag( p_textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapRoughness
			, CheckFlag( p_textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.DeclUniform< Sampler2D >( ShaderProgram::MapMetallic
			, CheckFlag( p_textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_fheightScale = writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );
		Declare_EncodeMaterial( writer );
		GLSL::Utils utils{ writer };
		utils.DeclareRemoveGamma();

		if ( CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			utils.DeclareGetMapNormal();
		}

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto matAlbedo = writer.DeclLocale( cuT( "matDiffuse" ), materials.GetAlbedo( vtx_material ) );
			auto matRoughness = writer.DeclLocale( cuT( "matRoughness" ), materials.GetRoughness( vtx_material ) );
			auto matMetallic = writer.DeclLocale( cuT( "matMetallic" ), materials.GetMetallic( vtx_material ) );
			auto matEmissive = writer.DeclLocale( cuT( "matEmissive" ), matAlbedo * materials.GetEmissive( vtx_material ) );
			auto matGamma = writer.DeclLocale( cuT( "matGamma" ), materials.GetGamma( vtx_material ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::mr::ComputePreLightingMapContributions( writer
				, normal
				, matMetallic
				, matRoughness
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			pbr::mr::ComputePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			auto flags = writer.DeclLocale( cuT( "flags" ), 0.0_f );
			EncodeMaterial( writer
				, c3d_shadowReceiver
				, 0_i
				, 0_i
				, c3d_envMapIndex
				, flags );

			if ( p_alphaFunc != ComparisonFunc::eAlways
				&& CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );

				switch ( p_alphaFunc )
				{
				case ComparisonFunc::eLess:
					IF( writer, alpha >= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eLEqual:
					IF( writer, alpha > materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eEqual:
					IF( writer, alpha != materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eNEqual:
					IF( writer, alpha == materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGEqual:
					IF( writer, alpha < materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGreater:
					IF( writer, alpha <= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;
				}
			}

			auto ambientOcclusion = writer.DeclLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matAlbedo, 0.0_f );
			out_c3dOutput3 = vec4( matMetallic, matRoughness, ambientOcclusion, 0.0_f );
			out_c3dOutput4 = vec4( matEmissive, materials.GetRefractionRatio( vtx_material ) );
		} );

		return writer.Finalise();
	}

	GLSL::Shader OpaquePass::DoGetPbrSGPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
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

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular
			, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss
			, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_fheightScale = writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );
		Declare_EncodeMaterial( writer );
		GLSL::Utils utils{ writer };
		utils.DeclareRemoveGamma();

		if ( CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
		{
			utils.DeclareGetMapNormal();
		}

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto matDiffuse = writer.DeclLocale( cuT( "matDiffuse" ), materials.GetDiffuse( vtx_material ) );
			auto matGlossiness = writer.DeclLocale( cuT( "matGlossiness" ), materials.GetGlossiness( vtx_material ) );
			auto matSpecular = writer.DeclLocale( cuT( "matSpecular" ), materials.GetSpecular( vtx_material ) );
			auto matEmissive = writer.DeclLocale( cuT( "matEmissive" ), matDiffuse * materials.GetEmissive( vtx_material ) );
			auto matGamma = writer.DeclLocale( cuT( "matGamma" ), materials.GetGamma( vtx_material ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::sg::ComputePreLightingMapContributions( writer
				, normal
				, matSpecular
				, matGlossiness
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			pbr::sg::ComputePostLightingMapContributions( writer
				, matDiffuse
				, matEmissive
				, matGamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			auto flags = writer.DeclLocale( cuT( "flags" ), 0.0_f );
			EncodeMaterial( writer
				, c3d_shadowReceiver
				, 0_i
				, 0_i
				, c3d_envMapIndex
				, flags );

			if ( p_alphaFunc != ComparisonFunc::eAlways
				&& CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );

				switch ( p_alphaFunc )
				{
				case ComparisonFunc::eLess:
					IF( writer, alpha >= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eLEqual:
					IF( writer, alpha > materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eEqual:
					IF( writer, alpha != materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eNEqual:
					IF( writer, alpha == materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGEqual:
					IF( writer, alpha < materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;

				case ComparisonFunc::eGreater:
					IF( writer, alpha <= materials.GetAlphaRef( vtx_material ) )
					{
						writer.Discard();
					}
					FI;
					break;
				}
			}

			auto ambientOcclusion = writer.DeclLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matDiffuse, matGlossiness );
			out_c3dOutput3 = vec4( matSpecular, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, materials.GetRefractionRatio( vtx_material ) );
		} );

		return writer.Finalise();
	}

	void OpaquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & scene = *m_camera->GetScene();
		m_sceneUbo.Update( scene, *m_camera, false );
	}
}
