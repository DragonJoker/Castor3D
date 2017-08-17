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
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		void doApplyAlphaFunc( glsl::GlslWriter & writer
			, ComparisonFunc alphaFunc
			, glsl::Float const & alpha
			, glsl::Int const & material
			, shader::Materials const & materials )
		{
			using namespace glsl;

			switch ( alphaFunc )
			{
			case ComparisonFunc::eLess:
				IF( writer, alpha >= materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;

			case ComparisonFunc::eLEqual:
				IF( writer, alpha > materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;

			case ComparisonFunc::eEqual:
				IF( writer, alpha != materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;

			case ComparisonFunc::eNEqual:
				IF( writer, alpha == materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;

			case ComparisonFunc::eGEqual:
				IF( writer, alpha < materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;

			case ComparisonFunc::eGreater:
				IF( writer, alpha <= materials.getAlphaRef( material ) )
				{
					writer.discard();
				}
				FI;
				break;
			}
		}
	}

	String const OpaquePass::Output1 = cuT( "c3d_output1" );
	String const OpaquePass::Output2 = cuT( "c3d_output2" );
	String const OpaquePass::Output3 = cuT( "c3d_output3" );
	String const OpaquePass::Output4 = cuT( "c3d_output4" );

	OpaquePass::OpaquePass( Scene & scene
		, Camera * camera
		, SsaoConfig const & config )
		: castor3d::RenderTechniquePass{ cuT( "deferred_opaque" )
			, scene
			, camera
			, false
			, nullptr
			, config }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	void OpaquePass::render( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps )
	{
		doRender( info, shadowMaps );
	}

	void OpaquePass::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
	}

	glsl::Shader OpaquePass::doGetVertexShaderSource( TextureChannels const & textureFlags
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
		UBO_SCENE( writer );
		UBO_MODEL( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
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
			auto p = writer.declLocale( cuT( "p" )
				, vec4( position.xyz(), 1.0 ) );
			auto n = writer.declLocale( cuT( "n" )
				, vec4( normal, 0.0 ) );
			auto t = writer.declLocale( cuT( "t" )
				, vec4( tangent, 0.0 ) );
			auto tex = writer.declLocale( cuT( "tex" )
				, texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );

				if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = material;
				}
				else
				{
					vtx_material = c3d_materialIndex;
				}
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
				vtx_material = material;
			}
			else
			{
				mtxModel = c3d_mtxModel;
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" )
					, 1.0_f - c3d_fTime );
				p = vec4( p.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				n = vec4( n.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				t = vec4( t.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				tex = tex * writer.paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = tex;
			p = mtxModel * p;
			vtx_position = p.xyz();
			p = c3d_mtxView * p;
			auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( mtxModel ) ) ) );

			if ( invertNormals )
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

			auto tbn = writer.declLocale( cuT( "tbn" )
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		shader::LegacyMaterials materials{ writer };
		materials.declare();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );

		auto c3d_mapDiffuse( writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( writer.declUniform< Sampler2D >( ShaderProgram::MapSpecular
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
		auto c3d_mapHeight( writer.declUniform< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( writer.declUniform< Sampler2D >( ShaderProgram::MapGloss
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection ) ) );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto v3Normal = writer.declLocale( cuT( "v3Normal" )
				, normalize( vtx_normal ) );
			auto v3Diffuse = writer.declLocale( cuT( "v3Diffuse" )
				, materials.getDiffuse( vtx_material ) );
			auto v3Specular = writer.declLocale( cuT( "v3Specular" )
				, materials.getSpecular( vtx_material ) );
			auto fMatShininess = writer.declLocale( cuT( "fMatShininess" )
				, materials.getShininess( vtx_material ) );
			auto v3Emissive = writer.declLocale( cuT( "v3Emissive" )
				, v3Diffuse * materials.getEmissive( vtx_material ) );
			auto gamma = writer.declLocale( cuT( "gamma" )
				, materials.getGamma( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			shader::legacy::computePreLightingMapContributions( writer
				, v3Normal
				, fMatShininess
				, textureFlags
				, programFlags
				, sceneFlags );
			shader::legacy::computePostLightingMapContributions( writer
				, v3Diffuse
				, v3Specular
				, v3Emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			if ( alphaFunc != ComparisonFunc::eAlways
				&& checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );
				doApplyAlphaFunc( writer, alphaFunc, alpha, vtx_material, materials );
			}

			out_c3dOutput1 = vec4( v3Normal, flags );
			out_c3dOutput2 = vec4( v3Diffuse, 0.0_f );
			out_c3dOutput3 = vec4( v3Specular, fMatShininess );
			out_c3dOutput4 = vec4( v3Emissive, materials.getRefractionRatio( vtx_material ) );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		shader::PbrMRMaterials materials{ writer };
		materials.declare();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );

		auto c3d_mapAlbedo( writer.declUniform< Sampler2D >( ShaderProgram::MapAlbedo
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declUniform< Sampler2D >( ShaderProgram::MapRoughness
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.declUniform< Sampler2D >( ShaderProgram::MapMetallic
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
		auto c3d_mapHeight( writer.declUniform< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_heightScale = writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, materials.getAlbedo( vtx_material ) );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, materials.getRoughness( vtx_material ) );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, materials.getMetallic( vtx_material ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, matAlbedo * materials.getEmissive( vtx_material ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, materials.getGamma( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			shader::pbr::mr::computePreLightingMapContributions( writer
				, normal
				, matMetallic
				, matRoughness
				, textureFlags
				, programFlags
				, sceneFlags );
			shader::pbr::mr::computePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			if ( alphaFunc != ComparisonFunc::eAlways
				&& checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );
				doApplyAlphaFunc( writer, alphaFunc, alpha, vtx_material, materials );
			}

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matAlbedo, 0.0_f );
			out_c3dOutput3 = vec4( matMetallic, matRoughness, ambientOcclusion, 0.0_f );
			out_c3dOutput4 = vec4( matEmissive, materials.getRefractionRatio( vtx_material ) );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// UBOs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

		// Fragment Inputs
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
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

		auto c3d_mapDiffuse( writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declUniform< Sampler2D >( ShaderProgram::MapSpecular
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.declUniform< Sampler2D >( ShaderProgram::MapGloss
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
		auto c3d_mapHeight( writer.declUniform< Sampler2D >( ShaderProgram::MapHeight
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_heightScale = writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t index = 0;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" ), materials.getDiffuse( vtx_material ) );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" ), materials.getGlossiness( vtx_material ) );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" ), materials.getSpecular( vtx_material ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" ), matDiffuse * materials.getEmissive( vtx_material ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" ), materials.getGamma( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" ), vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			shader::pbr::sg::computePreLightingMapContributions( writer
				, normal
				, matSpecular
				, matGlossiness
				, textureFlags
				, programFlags
				, sceneFlags );
			shader::pbr::sg::computePostLightingMapContributions( writer
				, matDiffuse
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			if ( alphaFunc != ComparisonFunc::eAlways
				&& checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, texture( c3d_mapOpacity, texCoord.xy() ).r() );
				doApplyAlphaFunc( writer, alphaFunc, alpha, vtx_material, materials );
			}

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matDiffuse, matGlossiness );
			out_c3dOutput3 = vec4( matSpecular, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, materials.getRefractionRatio( vtx_material ) );
		} );

		return writer.finalise();
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		auto & scene = *m_camera->getScene();
		m_sceneUbo.update( scene, *m_camera, false );
	}
}
