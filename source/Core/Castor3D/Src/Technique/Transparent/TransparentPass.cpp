#include "TransparentPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Skybox.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		inline void doUpdateProgram( ShaderProgram & program
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			if ( getShadowType( sceneFlags ) != ShadowType::eNone
				&& !program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot, ShaderType::ePixel ) )
			{
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowDirectional
					, ShaderType::ePixel );
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot
					, ShaderType::ePixel, shader::SpotShadowMapCount );
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowPoint
					, ShaderType::ePixel, shader::PointShadowMapCount );
			}

			if ( checkFlag( passFlags, PassFlag::ePbrMetallicRoughness )
				|| checkFlag( passFlags, PassFlag::ePbrSpecularGlossiness ) )
			{
				program.createUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance
					, ShaderType::ePixel );
				program.createUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered
					, ShaderType::ePixel );
				program.createUniform< UniformType::eSampler >( ShaderProgram::MapBrdf
					, ShaderType::ePixel );
			}
		}

		BlendState doCreateBlendState()
		{
			BlendState bdState;
			bdState.enableIndependantBlend( true );
			bdState.enableBlend( true, 0u );
			bdState.setAlphaBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			bdState.setRgbBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			bdState.enableBlend( true, 1u );
			bdState.setAlphaBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			bdState.setRgbBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			return bdState;
		}
	}

	//************************************************************************************************

	String getTextureName( WbTexture texture )
	{
		static std::array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapAccumulation" ),
				cuT( "c3d_mapRevealage" ),
			}
		};

		return Values[size_t( texture )];
	}

	PixelFormat getTextureFormat( WbTexture texture )
	{
		static std::array< PixelFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				PixelFormat::eD24S8,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eL16F32F,
			}
		};

		return Values[size_t( texture )];
	}

	AttachmentPoint getTextureAttachmentPoint( WbTexture texture )
	{
		static std::array< AttachmentPoint, size_t( WbTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eDepth,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( texture )];
	}

	uint32_t getTextureAttachmentIndex( WbTexture texture )
	{
		static std::array< uint32_t, size_t( WbTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
			}
		};

		return Values[size_t( texture )];
	}

	//************************************************************************************************

	TransparentPass::TransparentPass( Scene & scene
		, Camera * camera
		, SsaoConfig const & config )
		: castor3d::RenderTechniquePass{ cuT( "weighted_blend_transparent" )
			, scene
			, camera
			, true
			, false
			, nullptr
			, config }
	{
	}

	TransparentPass::~TransparentPass()
	{
	}

	void TransparentPass::render( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		m_scene.getLightCache().bindLights();
		doRender( info
			, shadowMaps
			, jitter );
		m_scene.getLightCache().unbindLights();
	}

	void TransparentPass::doPrepareFrontPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_frontPipelines.find( flags );

		if ( it == m_frontPipelines.end() )
		{
			doUpdateProgram( program
				, flags.m_passFlags
				, flags.m_textureFlags
				, flags.m_programFlags
				, flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, doCreateBlendState()
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
			{
				doCompletePipeline( flags, pipeline );
			} ) );
		}
	}

	void TransparentPass::doPrepareBackPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_backPipelines.find( flags );

		if ( it == m_backPipelines.end() )
		{
			doUpdateProgram( program
				, flags.m_passFlags
				, flags.m_textureFlags
				, flags.m_programFlags
				, flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, doCreateBlendState()
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
			{
				doCompletePipeline( flags, pipeline );
			} ) );
		}
	}

	glsl::Shader TransparentPass::doGetVertexShaderSource( PassFlags const & passFlags
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
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_time );
				curVertex = vec4( curVertex.xyz() * time + position2.xyz() * c3d_time, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_time, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_time, 1.0 );
				v3Texture = v3Texture * writer.paren( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			vtx_texture = v3Texture;
			curVertex = mtxModel * curVertex;
			vtx_position = curVertex.xyz();
			auto prvVertex = writer.declLocale( cuT( "prvVertex" )
				, c3d_prvView * curVertex );
			curVertex = c3d_curView * curVertex;
			auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( mtxModel ) ) ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
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

			auto tbn = writer.declLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
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

	glsl::Shader TransparentPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
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
		utils.declareLineariseDepth();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 2 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, matDiffuse * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, vec3( c3d_cameraPosition.x(), c3d_cameraPosition.y(), c3d_cameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
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

			auto colour = writer.declLocale< Vec4 >( cuT( "colour" ) );

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
					reflectedColour = texture( c3d_mapEnvironment, reflected ).xyz() * length( pxl_accumulation.xyz() );
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
					colour.xyz() = reflectedColour * matDiffuse / length( matDiffuse );
				}
				else
				{
					auto refFactor = writer.declLocale( cuT( "refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( incident, normal ), c3d_fresnelPower ) );
					colour.xyz() = mix( refractedColour, reflectedColour, refFactor );
				}
			}
			else
			{
				colour.xyz() = writer.paren( ambient + lightDiffuse ) * matDiffuse
					+ lightSpecular * matSpecular
					+ matEmissive;
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				auto wvPosition = writer.declLocale( cuT( "wvPosition" )
					, writer.paren( c3d_curView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.applyFog( colour, length( wvPosition ), wvPosition.y() );
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_revealage = alpha;
		} );

		return writer.finalise();
	}

	glsl::Shader TransparentPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
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
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( ShaderProgram::Lights, 1u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( ShaderProgram::Lights, 1u );
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
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeMetallicIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic() );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, utils.removeGamma( matGamma, material.m_albedo() ) );
			auto matEmissive = writer.declLocale( cuT( "emissive" )
				, matAlbedo * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, vec3( c3d_cameraPosition.x(), c3d_cameraPosition.y(), c3d_cameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
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
				, sceneFlags
				, passFlags );
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
			shader::pbr::mr::computePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );

			ambient *= occlusion * utils.computeMetallicIBL( normal
				, vtx_position
				, matAlbedo
				, matMetallic
				, matRoughness
				, c3d_cameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf );
			auto colour = writer.declLocale( cuT( "colour" )
				, lightDiffuse * matAlbedo + lightSpecular + matEmissive + ambient );

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_revealage = alpha;
		} );

		return writer.finalise();
	}

	glsl::Shader TransparentPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
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
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeSpecularIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, matDiffuse * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, vec3( c3d_cameraPosition.x(), c3d_cameraPosition.y(), c3d_cameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
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
			auto colour = writer.declLocale( cuT( "colour" )
				, lightDiffuse * matDiffuse + lightSpecular + matEmissive + ambient );

			auto alpha = writer.declLocale( cuT( "alpha" ), material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_revealage = alpha;
		} );

		return writer.finalise();
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		auto & scene = *m_camera->getScene();
		m_sceneUbo.update( scene, *m_camera, true );
	}

	void TransparentPass::doCompletePipeline( PipelineFlags const & flags
		, RenderPipeline & pipeline )
	{
		pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
		pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
		pipeline.addUniformBuffer( m_sceneUbo.getUbo() );
		pipeline.addUniformBuffer( m_modelUbo.getUbo() );

		if ( checkFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
		{
			pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
		}

		if ( checkFlag( flags.m_programFlags, ProgramFlag::eSkinning )
			&& !checkFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
		{
			pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
		}

		if ( checkFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
		{
			pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
		}
	}
}
