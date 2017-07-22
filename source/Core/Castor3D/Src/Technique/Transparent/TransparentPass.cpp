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

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//************************************************************************************************

	namespace
	{
		inline void DoUpdateProgram( ShaderProgram & p_program
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )
		{
			if ( GetShadowType( p_sceneFlags ) != GLSL::ShadowType::eNone
				&& !p_program.FindUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel ) )
			{
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint
					, ShaderType::ePixel, 6u );
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::ePbrMetallicRoughness )
				|| CheckFlag( p_programFlags, ProgramFlag::ePbrMetallicRoughness ) )
			{
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf
					, ShaderType::ePixel );
			}
		}

		BlendState DoCreateBlendState()
		{
			BlendState bdState;
			bdState.EnableIndependantBlend( true );
			bdState.EnableBlend( true, 0u );
			bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			bdState.EnableBlend( true, 1u );
			bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			return bdState;
		}
	}

	//************************************************************************************************

	String GetTextureName( WbTexture p_texture )
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

	PixelFormat GetTextureFormat( WbTexture p_texture )
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

	AttachmentPoint GetTextureAttachmentPoint( WbTexture p_texture )
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

	uint32_t GetTextureAttachmentIndex( WbTexture p_texture )
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
		: Castor3D::RenderTechniquePass{ cuT( "weighted_blend_transparent" )
			, p_scene
			, p_camera
			, true
			, false
			, nullptr
			, p_config }
		, m_directionalShadowMap{ *p_scene.GetEngine() }
		, m_spotShadowMap{ *p_scene.GetEngine() }
		, m_pointShadowMap{ *p_scene.GetEngine() }
	{
	}

	TransparentPass::~TransparentPass()
	{
	}

	void TransparentPass::Render( RenderInfo & p_info, bool p_shadows )
	{
		m_scene.GetLightCache().BindLights();
		DoRender( p_info, p_shadows );
		m_scene.GetLightCache().UnbindLights();
	}

	void TransparentPass::AddShadowProducer( Light & p_light )
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

	bool TransparentPass::InitialiseShadowMaps()
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

	void TransparentPass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void TransparentPass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_camera, p_queues );
		m_spotShadowMap.Update( *m_camera, p_queues );
		m_directionalShadowMap.Update( *m_camera, p_queues );
	}

	void TransparentPass::RenderShadowMaps()
	{
		m_directionalShadowMap.Render();
		m_pointShadowMap.Render();
		m_spotShadowMap.Render();
	}

	void TransparentPass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_directionalShadowMap.GetTexture() ) );
		p_depthMaps.push_back( std::ref( m_spotShadowMap.GetTexture() ) );

		for ( auto & map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( map ) );
		}

		auto & scene = *GetEngine()->GetRenderSystem()->GetTopScene();
	}

	void TransparentPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto it = m_frontPipelines.find( p_flags );

		if ( it == m_frontPipelines.end() )
		{
			DoUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.SetDepthTest( true );
			dsState.SetDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, DoCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				DoCompletePipeline( p_flags, pipeline );
			} ) );
		}
	}

	void TransparentPass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto it = m_backPipelines.find( p_flags );

		if ( it == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState dsState;
			dsState.SetDepthTest( true );
			dsState.SetDepthMask( WritingMask::eZero );
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, DoCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				DoCompletePipeline( p_flags, pipeline );
			} ) );
		}
	}

	GLSL::Shader TransparentPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
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
		SkinningUbo::Declare( writer, p_programFlags );
		UBO_MORPHING( writer, p_programFlags );
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
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			v4Vertex = c3d_mtxView * v4Vertex;
			auto mtxNormal = writer.DeclLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( mtxModel ) ) ) );

			if ( p_invertNormals )
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

			auto tbn = writer.DeclLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}

	GLSL::Shader TransparentPass::DoGetLegacyPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
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

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_fresnelBias = writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );

		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = legacy::CreateLightingModel( writer
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();
		utils.DeclareLineariseDepth();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.DeclFragData< Vec4 >( GetTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.DeclFragData< Float >( GetTextureName( WbTexture::eRevealage ), 1 ) );

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
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );

			auto colour = writer.DeclLocale< Vec4 >( cuT( "colour" ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
				|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.DeclLocale( cuT( "i" ), normalize( vtx_position - c3d_v3CameraPosition ) );
				auto reflectedColour = writer.DeclLocale( cuT( "reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto refractedColour = writer.DeclLocale( cuT( "refractedColour" ), diffuse / 2.0 );

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection ) )
				{
					auto reflected = writer.DeclLocale( cuT( "reflected" )
						, reflect( incident, v3Normal ) );
					reflectedColour = texture( c3d_mapEnvironment, reflected ).xyz() * length( pxl_v4Accumulation.xyz() );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					auto refracted = writer.DeclLocale( cuT( "refracted" )
						, refract( incident, v3Normal, materials.GetRefractionRatio( vtx_material ) ) );
					refractedColour = texture( c3d_mapEnvironment, refracted ).xyz() * diffuse / length( diffuse );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
					&& !CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					colour.xyz() = reflectedColour * diffuse / length( diffuse );
				}
				else
				{
					auto refFactor = writer.DeclLocale( cuT( "refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( incident, v3Normal ), c3d_fresnelPower ) );
					colour.xyz() = mix( refractedColour, reflectedColour, refFactor );
				}
			}
			else
			{
				colour.xyz() = writer.Paren( v3Ambient + v3Diffuse + emissive ) * diffuse
					+ v3Specular * materials.GetSpecular( vtx_material );
			}

			auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto wvPosition = writer.DeclLocale( cuT( "wvPosition" ), writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				fog.ApplyFog( colour, length( wvPosition ), wvPosition.y() );
			}

			//// Naive
			//auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.DeclLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.DeclLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.DeclLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.DeclLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.Finalise();
	}

	GLSL::Shader TransparentPass::DoGetPbrMRPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
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
			auto c3d_sLights = writer.DeclUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.DeclUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapAlbedo( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAlbedo
			, CheckFlag( p_textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapRoughness
			, CheckFlag( p_textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapMetallic( writer.DeclUniform< Sampler2D >( ShaderProgram::MapMetallic
			, CheckFlag( p_textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );
		auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::mr::CreateLightingModel( writer
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();
		utils.DeclareLineariseDepth();
		utils.DeclareGetMapNormal();
		utils.DeclareFresnelSchlick();
		utils.DeclareComputeMetallicIBL();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.DeclFragData< Vec4 >( GetTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.DeclFragData< Float >( GetTextureName( WbTexture::eRevealage ), 1 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto ambient = writer.DeclLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto metalness = writer.DeclLocale( cuT( "metallic" ), materials.GetMetallic( vtx_material ) );
			auto roughness = writer.DeclLocale( cuT( "roughness" ), materials.GetRoughness( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto albedo = writer.DeclLocale( cuT( "albedo" ), utils.RemoveGamma(gamma, materials.GetAlbedo( vtx_material ) ) );
			auto emissive = writer.DeclLocale( cuT( "emissive" ), diffuse * materials.GetEmissive( vtx_material ) );
			auto worldEye = writer.DeclLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.DeclLocale( cuT( "envAmbient" ), vec3( 1.0_f ) );
			auto envDiffuse = writer.DeclLocale( cuT( "envDiffuse" ), vec3( 1.0_f ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.DeclLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::mr::ComputePreLightingMapContributions( writer
				, normal
				, metalness
				, roughness
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			diffuse = lighting->ComputeCombinedLighting( worldEye
				, albedo
				, metalness
				, roughness
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, normal ) );
			pbr::mr::ComputePostLightingMapContributions( writer
				, albedo
				, emissive
				, gamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );

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
			auto colour = writer.DeclLocale( cuT( "colour" )
				, albedo + emissive + ambient );

			auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.DeclLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.DeclLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.DeclLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.DeclLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.Finalise();
	}

	GLSL::Shader TransparentPass::DoGetPbrSGPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
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
			auto c3d_sLights = writer.DeclUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = writer.DeclUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapDiffuse( writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse
			, CheckFlag( p_textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapSpecular( writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular
			, CheckFlag( p_textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapGlossiness( writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss
			, CheckFlag( p_textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapNormal( writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal
			, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity
			, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapAmbientOcclusion( writer.DeclUniform< Sampler2D >( ShaderProgram::MapAmbientOcclusion
			, CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive
			, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapEnvironment( writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_fheightScale( writer.DeclUniform< Float >( cuT( "c3d_fheightScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );
		auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
		auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
		auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );

		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = pbr::sg::CreateLightingModel( writer
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareApplyGamma();
		utils.DeclareRemoveGamma();
		utils.DeclareLineariseDepth();
		utils.DeclareGetMapNormal();
		utils.DeclareFresnelSchlick();
		utils.DeclareComputeSpecularIBL();

		auto parallaxMapping = DeclareParallaxMappingFunc( writer, p_textureFlags, p_programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( writer.DeclFragData< Vec4 >( GetTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( writer.DeclFragData< Float >( GetTextureName( WbTexture::eRevealage ), 1 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.DeclLocale( cuT( "normal" ), normalize( vtx_normal ) );
			auto ambient = writer.DeclLocale( cuT( "ambient" ), c3d_v4AmbientLight.xyz() );
			auto light = writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto specular = writer.DeclLocale( cuT( "specular" ), materials.GetSpecular( vtx_material ) );
			auto glossiness = writer.DeclLocale( cuT( "glossiness" ), materials.GetGlossiness( vtx_material ) );
			auto gamma = writer.DeclLocale( cuT( "gamma" ), materials.GetGamma( vtx_material ) );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), utils.RemoveGamma(gamma, materials.GetDiffuse( vtx_material ) ) );
			auto emissive = writer.DeclLocale( cuT( "emissive" ), diffuse * materials.GetEmissive( vtx_material ) );
			auto worldEye = writer.DeclLocale( cuT( "worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto envAmbient = writer.DeclLocale( cuT( "envAmbient" ), vec3( 1.0_f ) );
			auto envDiffuse = writer.DeclLocale( cuT( "envDiffuse" ), vec3( 1.0_f ) );
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), vtx_texture );
			auto occlusion = writer.DeclLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( CheckFlag( p_textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto viewDir = -writer.DeclLocale( cuT( "viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			pbr::sg::ComputePreLightingMapContributions( writer
				, normal
				, specular
				, glossiness
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			light = lighting->ComputeCombinedLighting( worldEye
				, diffuse
				, specular
				, glossiness
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, normal ) );
			pbr::sg::ComputePostLightingMapContributions( writer
				, diffuse
				, emissive
				, gamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );

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
			auto colour = writer.DeclLocale( cuT( "colour" )
				, diffuse + emissive + ambient );

			auto alpha = writer.DeclLocale( cuT( "alpha" ), materials.GetOpacity( vtx_material ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto weight = writer.DeclLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto weight = writer.DeclLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.DeclLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.DeclLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.DeclLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_v4Accumulation = vec4( colour.xyz() * alpha, alpha ) * weight;
			pxl_v4Revealage = alpha;
		} );

		return writer.Finalise();
	}

	void TransparentPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & scene = *m_camera->GetScene();
		m_sceneUbo.Update( scene, *m_camera, true );
	}

	void TransparentPass::DoCompletePipeline( PipelineFlags const & p_flags
		, RenderPipeline & p_pipeline )
	{
		p_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
		p_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
		p_pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
		p_pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

		if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
		{
			p_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
		}

		if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
			&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
		{
			p_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
		}

		if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
		{
			p_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
		}
	}
}
