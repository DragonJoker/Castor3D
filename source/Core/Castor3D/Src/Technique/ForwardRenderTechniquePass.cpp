#include "ForwardRenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"

using namespace castor;

namespace castor3d
{
	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, Scene & scene
		, Camera * camera
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderTechniquePass{ name
			, scene
			, camera
			, environment
			, ignored
			, config }
	{
	}

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, Scene & scene
		, Camera * camera
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderTechniquePass{ name
			, scene
			, camera
			, oit
			, environment
			, ignored
			, config }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	void ForwardRenderTechniquePass::render( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		m_scene.getLightCache().bindLights();
		doRender( info
			, shadowMaps
			, jitter );
		m_scene.getLightCache().unbindLights();
	}

	glsl::Shader ForwardRenderTechniquePass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.declAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.declAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.declAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( ShaderProgram::Material
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( ShaderProgram::Normal2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Tangent2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Outputs
		auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" ) );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto curVertex = writer.declLocale( cuT( "curVertex" )
				, vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else
			{
				mtxModel = c3d_mtxModel;
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, mat3( c3d_mtxNormal ) );
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" )
					, vec3( 1.0_f - c3d_time ) );
				curVertex = vec4( glsl::fma( curVertex.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				v4Normal = vec4( glsl::fma( v4Normal.xyz(), time, normal2.xyz() * c3d_time ), 1.0 );
				v4Tangent = vec4( glsl::fma( v4Tangent.xyz(), time, tangent2.xyz() * c3d_time ), 1.0 );
				v3Texture = glsl::fma( v3Texture, time, texture2 * c3d_time );
			}

			vtx_texture = v3Texture;
			curVertex = mtxModel * curVertex;
			vtx_position = curVertex.xyz();
			auto prvVertex = writer.declLocale( cuT( "prvVertex" )
				, c3d_prvView * curVertex );
			curVertex = c3d_curView * curVertex;
			auto mtxNormal = writer.getBuiltin< Mat3 >( cuT( "mtxNormal" ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( writer.paren( mtxNormal * -v4Normal.xyz() ) );
			}
			else
			{
				vtx_normal = normalize( writer.paren( mtxNormal * v4Normal.xyz() ) );
			}

			vtx_tangent = normalize( writer.paren( mtxNormal * v4Tangent.xyz() ) );
			vtx_tangent = normalize( glsl::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_projection * curVertex;
			prvVertex = c3d_projection * prvVertex;
			// Convert the jitter from non-homogeneous coordiantes to homogeneous
			// coordinates and add it:
			// (note that for providing the jitter in non-homogeneous projection space,
			//  pixel coordinates (screen space) need to multiplied by two in the C++
			//  code)
			gl_Position.xy() -= c3d_curJitter * gl_Position.w();
			prvVertex.xy() -= c3d_prvJitter * gl_Position.w();

			auto tbn = writer.declLocale( cuT( "tbn" )
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition;
			vtx_curPosition = gl_Position.xyw();
			vtx_prvPosition = prvVertex.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader ForwardRenderTechniquePass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" ) );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );

		shader::LegacyMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( ShaderProgram::Lights, 1u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( ShaderProgram::Lights, 1u );
		}

		auto index = MinTextureIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( ShaderProgram::MapSpecular
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGloss( writer.declSampler< Sampler2D >( ShaderProgram::MapGloss
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( ShaderProgram::MapTransmittance
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_fresnelBias = writer.declUniform< Float >( cuT( "c3d_fresnelBias" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = writer.declUniform< Float >( cuT( "c3d_fresnelScale" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = writer.declUniform< Float >( cuT( "c3d_fresnelPower" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::legacy::createLightingModel( writer
			, getShadowType( sceneFlags )
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( m_opaque && alphaFunc != ComparisonFunc::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, vec3( c3d_cameraPosition.x(), c3d_cameraPosition.y(), c3d_cameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f, 1.0_f, 1.0_f ) );
			shader::legacy::computePreLightingMapContributions( writer
				, normal
				, matShininess
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::legacy::computePostLightingMapContributions( writer
				, matDiffuse
				, matSpecular
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matShininess
				, c3d_shadowReceiver
				, shader::FragmentInput( vtx_position, normal )
				, output );

			pxl_fragColor.xyz() = glsl::fma( ambient + lightDiffuse
				, matDiffuse
				, glsl::fma( lightSpecular
					, material.m_specular()
					, matEmissive ) );

			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "i" )
					, normalize( vtx_position - c3d_cameraPosition ) );
				auto reflectedColour = writer.declLocale( cuT( "reflectedColour" )
					, vec3( 0.0_f, 0, 0 ) );
				auto refractedColour = writer.declLocale( cuT( "refractedColour" )
					, matDiffuse / 2.0 );

				if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					auto reflected = writer.declLocale( cuT( "reflected" )
						, reflect( incident, normal ) );
					reflectedColour = texture( c3d_mapEnvironment, reflected ).xyz() * length( pxl_fragColor.xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					auto refracted = writer.declLocale( cuT( "refracted" )
						, refract( incident, normal, material.m_refractionRatio() ) );
					refractedColour = texture( c3d_mapEnvironment, refracted ).xyz() * matDiffuse / length( matDiffuse );
				}

				if ( checkFlag( textureFlags, TextureChannel::eReflection )
					&& !checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					pxl_fragColor.xyz() = reflectedColour * matDiffuse / length( matDiffuse );
				}
				else
				{
					auto refFactor = writer.declLocale( cuT( "refFactor" )
						, glsl::fma( c3d_fresnelScale
							, pow( 1.0_f + dot( incident, normal ), c3d_fresnelPower )
							, c3d_fresnelBias ) );
					pxl_fragColor.xyz() = mix( refractedColour, reflectedColour, refFactor );
				}
			}

			if ( !m_opaque )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				pxl_fragColor.a() = alpha;
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				auto wvPosition = writer.declLocale( cuT( "wvPosition" )
					, writer.paren( c3d_curView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.applyFog( pxl_fragColor, length( wvPosition ), wvPosition.y() );
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}

	glsl::Shader ForwardRenderTechniquePass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" ) );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );

		shader::PbrMRMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( cuT( "c3d_sLights" ), 1u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( cuT( "c3d_sLights" ), 1u );
		}

		auto index = MinTextureIndex;
		auto c3d_mapAlbedo( writer.declSampler< Sampler2D >( ShaderProgram::MapAlbedo
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declSampler< Sampler2D >( ShaderProgram::MapRoughness
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.declSampler< Sampler2D >( ShaderProgram::MapMetallic
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( ShaderProgram::MapTransmittance
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( ShaderProgram::MapIrradiance
			, index++ );
		auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( ShaderProgram::MapPrefiltered
			, index++ );
		auto c3d_mapBrdf = writer.declSampler< Sampler2D >( ShaderProgram::MapBrdf
			, index++ );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::pbr::mr::createLightingModel( writer
			, getShadowType( sceneFlags )
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeMetallicIBL();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( m_opaque && alphaFunc != ComparisonFunc::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic() );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, utils.removeGamma( matGamma, material.m_albedo() ) );
			auto matEmissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive() ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			shader::pbr::mr::computePreLightingMapContributions( writer
				, normal
				, matMetallic
				, matRoughness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::mr::computePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matAlbedo
				, matMetallic
				, matRoughness
				, c3d_shadowReceiver
				, shader::FragmentInput( vtx_position, normal )
				, output );

			ambient *= occlusion * utils.computeMetallicIBL( normal
				, vtx_position
				, matAlbedo
				, matMetallic
				, matRoughness
				, c3d_cameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf );
			pxl_fragColor.xyz() = glsl::fma( lightDiffuse
				, matAlbedo
				, lightSpecular + matEmissive + ambient );

			if ( !m_opaque )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				pxl_fragColor.a() = alpha;
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				auto wvPosition = writer.declLocale( cuT( "wvPosition" )
					, writer.paren( c3d_curView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.applyFog( pxl_fragColor, length( wvPosition ), wvPosition.y() );
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}

	glsl::Shader ForwardRenderTechniquePass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Intputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" ) );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );

		shader::PbrSGMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( cuT( "c3d_sLights" ), 1u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( cuT( "c3d_sLights" ), 1u );
		}

		auto index = MinTextureIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( ShaderProgram::MapSpecular
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.declSampler< Sampler2D >( ShaderProgram::MapGloss
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( ShaderProgram::MapTransmittance
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( ShaderProgram::MapIrradiance
			, index++ );
		auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( ShaderProgram::MapPrefiltered
			, index++ );
		auto c3d_mapBrdf = writer.declSampler< Sampler2D >( ShaderProgram::MapBrdf
			, index++ );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::pbr::sg::createLightingModel( writer
			, getShadowType( sceneFlags )
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeSpecularIBL();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, -normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( m_opaque && alphaFunc != ComparisonFunc::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			shader::pbr::sg::computePreLightingMapContributions( writer
				, normal
				, matSpecular
				, matGlossiness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::sg::computePostLightingMapContributions( writer
				, matDiffuse
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matDiffuse
				, matSpecular
				, matGlossiness
				, c3d_shadowReceiver
				, shader::FragmentInput( vtx_position, normal )
				, output );

			ambient *= occlusion * utils.computeSpecularIBL( normal
				, vtx_position
				, matDiffuse
				, matSpecular
				, matGlossiness
				, c3d_cameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf );
			pxl_fragColor.xyz() = glsl::fma( lightDiffuse
				, matDiffuse
				, lightSpecular + matEmissive + ambient );

			if ( !m_opaque )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				pxl_fragColor.a() = alpha;
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				auto wvPosition = writer.declLocale( cuT( "wvPosition" )
					, writer.paren( c3d_curView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.applyFog( pxl_fragColor, length( wvPosition ), wvPosition.y() );
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}
}
