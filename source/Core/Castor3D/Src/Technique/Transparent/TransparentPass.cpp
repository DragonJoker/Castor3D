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
#include <GlslShadow.hpp>
#include <GlslMaterial.hpp>
#include <GlslUtils.hpp>
#include <GlslPhongLighting.hpp>
#include <GlslMetallicBrdfLighting.hpp>
#include <GlslSpecularBrdfLighting.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		inline void doUpdateProgram( ShaderProgram & p_program
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			if ( getShadowType( sceneFlags ) != GLSL::ShadowType::eNone
				&& !p_program.findUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel ) )
			{
				p_program.createUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional
					, ShaderType::ePixel );
				p_program.createUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot
					, ShaderType::ePixel );
				p_program.createUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint
					, ShaderType::ePixel, 6u );
			}

			if ( checkFlag( programFlags, ProgramFlag::ePbrMetallicRoughness )
				|| checkFlag( programFlags, ProgramFlag::ePbrMetallicRoughness ) )
			{
				p_program.createUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance
					, ShaderType::ePixel );
				p_program.createUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered
					, ShaderType::ePixel );
				p_program.createUniform< UniformType::eSampler >( ShaderProgram::MapBrdf
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

	String getTextureName( WbTexture p_texture )
	{
		static std::array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapAccumulation" ),
				cuT( "c3d_mapRevealage" ),
			}
		};

		return Values[size_t( p_texture )];
	}

	PixelFormat getTextureFormat( WbTexture p_texture )
	{
		static std::array< PixelFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				PixelFormat::eD24S8,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eL16F32F,
			}
		};

		return Values[size_t( p_texture )];
	}

	AttachmentPoint getTextureAttachmentPoint( WbTexture p_texture )
	{
		static std::array< AttachmentPoint, size_t( WbTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eDepth,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( p_texture )];
	}

	uint32_t getTextureAttachmentIndex( WbTexture p_texture )
	{
		static std::array< uint32_t, size_t( WbTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
			}
		};

		return Values[size_t( p_texture )];
	}

	//************************************************************************************************

	TransparentPass::TransparentPass( Scene & p_scene
		, Camera * p_camera
		, SsaoConfig const & p_config )
		: castor3d::RenderTechniquePass{ cuT( "weighted_blend_transparent" )
			, p_scene
			, p_camera
			, true
			, false
			, nullptr
			, p_config }
		, m_directionalShadowMap{ *p_scene.getEngine() }
		, m_spotShadowMap{ *p_scene.getEngine() }
		, m_pointShadowMap{ *p_scene.getEngine() }
	{
	}

	TransparentPass::~TransparentPass()
	{
	}

	void TransparentPass::render( RenderInfo & p_info, bool p_shadows )
	{
		m_scene.getLightCache().bindLights();
		doRender( p_info, p_shadows );
		m_scene.getLightCache().unbindLights();
	}

	void TransparentPass::addShadowProducer( Light & p_light )
	{
		if ( p_light.isShadowProducer() )
		{
			switch ( p_light.getLightType() )
			{
			case LightType::eDirectional:
				m_directionalShadowMap.addLight( p_light );
				break;

			case LightType::ePoint:
				m_pointShadowMap.addLight( p_light );
				break;

			case LightType::eSpot:
				m_spotShadowMap.addLight( p_light );
				break;
			}
		}
	}

	bool TransparentPass::initialiseShadowMaps()
	{
		m_scene.getLightCache().forEach( [this]( Light & p_light )
		{
			addShadowProducer( p_light );
		} );

		bool result = m_directionalShadowMap.initialise();

		if ( result )
		{
			result = m_spotShadowMap.initialise();
		}

		if ( result )
		{
			result = m_pointShadowMap.initialise();
		}

		ENSURE( result );
		return result;
	}

	void TransparentPass::cleanupShadowMaps()
	{
		m_pointShadowMap.cleanup();
		m_spotShadowMap.cleanup();
		m_directionalShadowMap.cleanup();
	}

	void TransparentPass::updateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.update( *m_camera, p_queues );
		m_spotShadowMap.update( *m_camera, p_queues );
		m_directionalShadowMap.update( *m_camera, p_queues );
	}

	void TransparentPass::renderShadowMaps()
	{
		m_directionalShadowMap.render();
		m_pointShadowMap.render();
		m_spotShadowMap.render();
	}

	void TransparentPass::doGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_directionalShadowMap.getTexture() ) );
		p_depthMaps.push_back( std::ref( m_spotShadowMap.getTexture() ) );

		for ( auto & map : m_pointShadowMap.getTextures() )
		{
			p_depthMaps.push_back( std::ref( map ) );
		}

		auto & scene = *getEngine()->getRenderSystem()->getTopScene();
	}

	void TransparentPass::doPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto it = m_frontPipelines.find( p_flags );

		if ( it == m_frontPipelines.end() )
		{
			doUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( p_flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, doCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				doCompletePipeline( p_flags, pipeline );
			} ) );
		}
	}

	void TransparentPass::doPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto it = m_backPipelines.find( p_flags );

		if ( it == m_backPipelines.end() )
		{
			doUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, doCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				doCompletePipeline( p_flags, pipeline );
			} ) );
		}
	}

	GLSL::Shader TransparentPass::doGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.declAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.declAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.declAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( ShaderProgram::Material, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( ShaderProgram::Normal2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Tangent2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );
		UBO_SCENE( writer );
		UBO_MODEL( writer );

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
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" ), vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" ), vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" ), texture );
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
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			v4Vertex = c3d_mtxView * v4Vertex;
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
			gl_Position = c3d_mtxProjection * v4Vertex;

			auto tbn = writer.declLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	GLSL::Shader TransparentPass::doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
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

		LegacyMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.declUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapDiffuse( writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( writer.declUniform< Sampler2D >( ShaderProgram::MapSpecular, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( writer.declUniform< Sampler2D >( ShaderProgram::MapHeight, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( writer.declUniform< Sampler2D >( ShaderProgram::MapGloss, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_fresnelBias = writer.declUniform< Float >( cuT( "c3d_fresnelBias" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = writer.declUniform< Float >( cuT( "c3d_fresnelScale" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = writer.declUniform< Float >( cuT( "c3d_fresnelPower" )
			, checkFlag( textureFlags, TextureChannel::eReflection ) || checkFlag( textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );

		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" ), checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = legacy::createLightingModel( writer
			, getShadowType( sceneFlags ) );
		GLSL::Fog fog{ getFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();

		auto parallaxMapping = declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto v3Normal = writer.declLocale( cuT( "v3Normal" ), normalize( vtx_normal ) );
			auto v3Ambient = writer.declLocale( cuT( "v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto v3Diffuse = writer.declLocale( cuT( "v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto v3Specular = writer.declLocale( cuT( "v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto fMatShininess = writer.declLocale( cuT( "fMatShininess" ), materials.getShininess( vtx_material ) );
			auto gamma = writer.declLocale( cuT( "gamma" ), materials.getGamma( vtx_material ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" ), utils.removeGamma(gamma, materials.getDiffuse( vtx_material ) ) );
			auto emissive = writer.declLocale( cuT( "emissive" ), diffuse * materials.getEmissive( vtx_material ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" ), vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			legacy::computePreLightingMapContributions( writer
				, v3Normal
				, fMatShininess
				, textureFlags
				, programFlags
				, sceneFlags );
			OutputComponents output{ v3Diffuse, v3Specular };
			lighting->computeCombinedLighting( worldEye
				, fMatShininess
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, v3Normal )
				, output );
			legacy::computePostLightingMapContributions( writer
				, diffuse
				, v3Specular
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );

			auto colour = writer.declLocale< Vec4 >( cuT( "colour" ) );

			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "i" ), normalize( vtx_position - c3d_v3CameraPosition ) );
				auto reflectedColour = writer.declLocale( cuT( "reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto refractedColour = writer.declLocale( cuT( "refractedColour" ), diffuse / 2.0 );

				if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					auto reflected = writer.declLocale( cuT( "reflected" )
						, reflect( incident, v3Normal ) );
					reflectedColour = texture( c3d_mapEnvironment, reflected ).xyz() * length( pxl_v4Accumulation.xyz() );
				}

				if ( checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					auto refracted = writer.declLocale( cuT( "refracted" )
						, refract( incident, v3Normal, materials.getRefractionRatio( vtx_material ) ) );
					refractedColour = texture( c3d_mapEnvironment, refracted ).xyz() * diffuse / length( diffuse );
				}

				if ( checkFlag( textureFlags, TextureChannel::eReflection )
					&& !checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					colour.xyz() = reflectedColour * diffuse / length( diffuse );
				}
				else
				{
					auto refFactor = writer.declLocale( cuT( "refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( incident, v3Normal ), c3d_fresnelPower ) );
					colour.xyz() = mix( refractedColour, reflectedColour, refFactor );
				}
			}
			else
			{
				colour.xyz() = writer.paren( v3Ambient + v3Diffuse + emissive ) * diffuse
					+ v3Specular * materials.getSpecular( vtx_material );
			}

			auto alpha = writer.declLocale( cuT( "alpha" ), materials.getOpacity( vtx_material ) );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			if ( getFogType( sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto wvPosition = writer.declLocale( cuT( "wvPosition" ), writer.paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.applyFog( colour, length( wvPosition ), wvPosition.y() );
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

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

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.finalise();
	}

	GLSL::Shader TransparentPass::doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
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

		PbrMRMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.declUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapAlbedo( writer.declUniform< Sampler2D >( ShaderProgram::MapAlbedo
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declUniform< Sampler2D >( ShaderProgram::MapRoughness
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapMetallic( writer.declUniform< Sampler2D >( ShaderProgram::MapMetallic
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapAmbientOcclusion( writer.declUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );
		auto c3d_mapIrradiance = writer.declUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.declUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.declUniform< Sampler2D >( ShaderProgram::MapBrdf );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::mr::createLightingModel( writer
			, getShadowType( sceneFlags ) );
		GLSL::Fog fog{ getFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeMetallicIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto diffuse = writer.declLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto metalness = writer.declLocale( cuT( "metallic" ), materials.getMetallic( vtx_material ) );
			auto roughness = writer.declLocale( cuT( "roughness" ), materials.getRoughness( vtx_material ) );
			auto gamma = writer.declLocale( cuT( "gamma" ), materials.getGamma( vtx_material ) );
			auto albedo = writer.declLocale( cuT( "albedo" ), utils.removeGamma(gamma, materials.getAlbedo( vtx_material ) ) );
			auto emissive = writer.declLocale( cuT( "emissive" ), diffuse * materials.getEmissive( vtx_material ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" ), vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" ), vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::mr::computePreLightingMapContributions( writer
				, normal
				, metalness
				, roughness
				, textureFlags
				, programFlags
				, sceneFlags );
			diffuse = lighting->computeCombinedLighting( worldEye
				, albedo
				, metalness
				, roughness
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, normal ) );
			pbr::mr::computePostLightingMapContributions( writer
				, albedo
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );

			ambient *= occlusion * utils.computeMetallicIBL( normal
				, vtx_position
				, albedo
				, metalness
				, roughness
				, c3d_v3CameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf
				, 0_i );
			auto colour = writer.declLocale( cuT( "colour" )
				, albedo + emissive + ambient );

			auto alpha = writer.declLocale( cuT( "alpha" ), materials.getOpacity( vtx_material ) );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

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

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.finalise();
	}

	GLSL::Shader TransparentPass::doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
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

		PbrSGMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.declUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapDiffuse( writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapSpecular( writer.declUniform< Sampler2D >( ShaderProgram::MapSpecular
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapGlossiness( writer.declUniform< Sampler2D >( ShaderProgram::MapGloss
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapNormal( writer.declUniform< Sampler2D >( ShaderProgram::MapNormal
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapAmbientOcclusion( writer.declUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declUniform< Sampler2D >( ShaderProgram::MapEmissive
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, checkFlag( textureFlags, TextureChannel::eReflection )
			|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_heightScale( writer.declUniform< Float >( cuT( "c3d_heightScale" )
			, checkFlag( textureFlags, TextureChannel::eHeight ), 0.1_f ) );
		auto c3d_mapIrradiance = writer.declUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.declUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.declUniform< Sampler2D >( ShaderProgram::MapBrdf );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::sg::createLightingModel( writer
			, getShadowType( sceneFlags ) );
		GLSL::Fog fog{ getFogType( sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeSpecularIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto light = writer.declLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto specular = writer.declLocale( cuT( "specular" ), materials.getSpecular( vtx_material ) );
			auto glossiness = writer.declLocale( cuT( "glossiness" ), materials.getGlossiness( vtx_material ) );
			auto gamma = writer.declLocale( cuT( "gamma" ), materials.getGamma( vtx_material ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" ), utils.removeGamma(gamma, materials.getDiffuse( vtx_material ) ) );
			auto emissive = writer.declLocale( cuT( "emissive" ), diffuse * materials.getEmissive( vtx_material ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" ), vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" ), vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::sg::computePreLightingMapContributions( writer
				, normal
				, specular
				, glossiness
				, textureFlags
				, programFlags
				, sceneFlags );
			light = lighting->computeCombinedLighting( worldEye
				, diffuse
				, specular
				, glossiness
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, normal ) );
			pbr::sg::computePostLightingMapContributions( writer
				, diffuse
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );

			ambient *= occlusion * utils.computeSpecularIBL( normal
				, vtx_position
				, diffuse
				, specular
				, glossiness
				, c3d_v3CameraPosition
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf
				, 0_i );
			auto colour = writer.declLocale( cuT( "colour" )
				, diffuse + emissive + ambient );

			auto alpha = writer.declLocale( cuT( "alpha" ), materials.getOpacity( vtx_material ) );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

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

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.finalise();
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & scene = *m_camera->getScene();
		m_sceneUbo.update( scene, *m_camera, true );
	}

	void TransparentPass::doCompletePipeline( PipelineFlags const & p_flags
		, RenderPipeline & p_pipeline )
	{
		p_pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
		p_pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
		p_pipeline.addUniformBuffer( m_sceneUbo.getUbo() );
		p_pipeline.addUniformBuffer( m_modelUbo.getUbo() );

		if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
		{
			p_pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
		}

		if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
			&& !checkFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
		{
			p_pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
		}

		if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
		{
			p_pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
		}
	}
}
