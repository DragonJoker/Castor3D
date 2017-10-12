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
	String const OpaquePass::Output1 = cuT( "c3d_output1" );
	String const OpaquePass::Output2 = cuT( "c3d_output2" );
	String const OpaquePass::Output3 = cuT( "c3d_output3" );
	String const OpaquePass::Output4 = cuT( "c3d_output4" );
	String const OpaquePass::Output5 = cuT( "c3d_output5" );

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
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		doRender( info
			, shadowMaps
			, jitter );
	}

	void OpaquePass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( sceneFlags, SceneFlag::eShadowFilterPcf );
	}

	glsl::Shader OpaquePass::doGetVertexShaderSource( PassFlags const & passFlags
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
		UBO_SCENE( writer );
		UBO_MODEL( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

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
					, transpose( inverse( mat3( mtxModel ) ) ) );
				//auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				//	, mat3( c3d_mtxNormal ) );
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
				p = vec4( glsl::fma( p.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				n = vec4( glsl::fma( n.xyz(), time, normal2.xyz() * c3d_time ), 1.0 );
				t = vec4( glsl::fma( t.xyz(), time, tangent2.xyz() * c3d_time ), 1.0 );
				tex = glsl::fma( tex, time, texture2 * c3d_time );
			}

			vtx_texture = tex;
			p = mtxModel * p;
			vtx_position = p.xyz();
			auto prvVertex = writer.declLocale( cuT( "prvVertex" )
				, c3d_prvView * p );
			p = c3d_curView * p;
			auto mtxNormal = writer.getBuiltin< Mat3 >( cuT( "mtxNormal" ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( mtxNormal * -n.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * n.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * t.xyz() );
			vtx_tangent = normalize( glsl::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_projection * p;
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

	glsl::Shader OpaquePass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
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
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
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
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

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
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != ComparisonFunc::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

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
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, material.m_diffuse() );
			auto specular = writer.declLocale( cuT( "specular" )
				, material.m_specular() );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess() );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive() ) );
			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma() );
			shader::legacy::computePreLightingMapContributions( writer
				, normal
				, matShininess
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::legacy::computePostLightingMapContributions( writer
				, diffuse
				, specular
				, emissive
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

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( diffuse, matShininess );
			out_c3dOutput3 = vec4( specular, ambientOcclusion );
			out_c3dOutput4 = vec4( emissive, transmittance );

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput5.xyz() = vec3( curPosition - prvPosition, writer.cast< Float >( vtx_material ) );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
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
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
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
		auto c3d_heightScale = writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

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
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != ComparisonFunc::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

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
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, material.m_albedo() );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness() );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic() );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
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
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );
			
			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matAlbedo, 0.0_f );
			out_c3dOutput3 = vec4( matMetallic, matRoughness, 0.0_f, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, transmittance );

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput5.xyz() = vec3( curPosition - prvPosition, writer.cast< Float >( vtx_material ) );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
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
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ? index++ : 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && alphaFunc != ComparisonFunc::eAlways ) );
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
		auto c3d_heightScale = writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

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
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != ComparisonFunc::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

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
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, material.m_diffuse() );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness() );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );

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
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matDiffuse, matGlossiness );
			out_c3dOutput3 = vec4( matSpecular, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, transmittance );

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput5.xyz() = vec3( curPosition - prvPosition, writer.cast< Float >( vtx_material ) );
		} );

		return writer.finalise();
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		auto & scene = *m_camera->getScene();
		m_sceneUbo.update( scene, *m_camera, false );
	}
}
