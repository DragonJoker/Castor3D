#include "TransparentPass.hpp"

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
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel, 6u );
			}
		}

		BlendState DoCreateBlendState()
		{
			BlendState l_bdState;
			l_bdState.EnableIndependantBlend( true );
			l_bdState.EnableBlend( true, 0u );
			l_bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			l_bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eOne, BlendOperand::eOne, 0u );
			l_bdState.EnableBlend( true, 1u );
			l_bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			l_bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eZero, BlendOperand::eInvSrcColour, 1u );
			return l_bdState;
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

		for ( auto & l_map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( l_map ) );
		}
	}

	void TransparentPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontPipelines.find( p_flags );

		if ( l_it == m_frontPipelines.end() )
		{
			DoUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );
			auto & l_pipeline = *m_frontPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, DoCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
			{
				DoCompletePipeline( p_flags, l_pipeline );
			} ) );
		}
	}

	void TransparentPass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backPipelines.find( p_flags );

		if ( l_it == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program
				, p_flags.m_textureFlags
				, p_flags.m_programFlags
				, p_flags.m_sceneFlags );

			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, DoCreateBlendState()
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
			{
				DoCompletePipeline( p_flags, l_pipeline );
			} ) );
		}
	}

	GLSL::Shader TransparentPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto material = l_writer.DeclAttribute< Int >( ShaderProgram::Material, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		SkinningUbo::Declare( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );
		UBO_SCENE( l_writer );
		UBO_MODEL( l_writer );

		// Outputs
		auto vtx_position = l_writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.DeclOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.DeclOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.DeclOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.DeclOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = l_writer.DeclOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.DeclLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v4Normal = l_writer.DeclLocale( cuT( "l_v4Normal" ), vec4( normal, 0.0 ) );
			auto l_v4Tangent = l_writer.DeclLocale( cuT( "l_v4Tangent" ), vec4( tangent, 0.0 ) );
			auto l_v3Texture = l_writer.DeclLocale( cuT( "l_v3Texture" ), texture );
			auto l_mtxModel = l_writer.DeclLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				l_mtxModel = SkinningUbo::ComputeTransform( l_writer, p_programFlags );
				vtx_material = c3d_materialIndex;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
				vtx_material = material;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
				vtx_material = c3d_materialIndex;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.DeclLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
				l_v4Normal = vec4( l_v4Normal.xyz() * l_time + normal2.xyz() * c3d_fTime, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.xyz() * l_time + tangent2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_position = l_v4Vertex.xyz();
			l_v4Vertex = c3d_mtxView * l_v4Vertex;
			auto l_mtxNormal = l_writer.DeclLocale( cuT( "l_mtxNormal" )
				, transpose( inverse( mat3( l_mtxModel ) ) ) );

			if ( p_invertNormals )
			{
				vtx_normal = normalize( l_mtxNormal * -l_v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( l_mtxNormal * l_v4Normal.xyz() );
			}

			vtx_tangent = normalize( l_mtxNormal * l_v4Tangent.xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * l_v4Vertex;

			auto l_tbn = l_writer.DeclLocale( cuT( "l_tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = l_tbn * vtx_position;
			vtx_tangentSpaceViewPosition = l_tbn * c3d_v3CameraPosition;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	GLSL::Shader TransparentPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_MODEL( l_writer );

		// Fragment Intputs
		auto vtx_position = l_writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.DeclInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.DeclInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.DeclInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.DeclInput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = l_writer.DeclInput< Int >( cuT( "vtx_material" ) );

		LegacyMaterials l_materials{ l_writer };
		l_materials.Declare();

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.DeclUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = l_writer.DeclUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapDiffuse( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_fresnelBias = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.10_f );
		auto c3d_fresnelScale = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.25_f );
		auto c3d_fresnelPower = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" )
			, CheckFlag( p_textureFlags, TextureChannel::eReflection ) || CheckFlag( p_textureFlags, TextureChannel::eRefraction )
			, 0.30_f );
		auto c3d_mapEnvironment( l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment
			, CheckFlag( p_textureFlags, TextureChannel::eReflection )
			|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) ) );

		auto c3d_fheightScale( l_writer.DeclUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		GLSL::Utils l_utils{ l_writer };
		l_utils.DeclareApplyGamma();
		l_utils.DeclareRemoveGamma();
		l_utils.DeclareLineariseDepth();

		auto l_parallaxMapping = DeclareParallaxMappingFunc( l_writer, p_textureFlags, p_programFlags );

		// Fragment Outputs
		auto pxl_v4Accumulation( l_writer.DeclFragData< Vec4 >( GetTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_v4Revealage( l_writer.DeclFragData< Float >( GetTextureName( WbTexture::eRevealage ), 1 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.DeclLocale( cuT( "l_v3Normal" ), normalize( vtx_normal ) );
			auto l_v3Ambient = l_writer.DeclLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.DeclLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.DeclLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fMatShininess = l_writer.DeclLocale( cuT( "l_fMatShininess" ), l_materials.GetShininess( vtx_material ) );
			auto l_gamma = l_writer.DeclLocale( cuT( "l_gamma" ), l_materials.GetGamma( vtx_material ) );
			auto l_diffuse = l_writer.DeclLocale( cuT( "l_diffuse" ), l_utils.RemoveGamma(l_gamma, l_materials.GetDiffuse( vtx_material ) ) );
			auto l_emissive = l_writer.DeclLocale( cuT( "l_emissive" ), l_diffuse * l_materials.GetEmissive( vtx_material ) );
			auto l_worldEye = l_writer.DeclLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto l_envAmbient = l_writer.DeclLocale( cuT( "l_envAmbient" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto l_envDiffuse = l_writer.DeclLocale( cuT( "l_envDiffuse" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto l_texCoord = l_writer.DeclLocale( cuT( "l_texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto l_viewDir = -l_writer.DeclLocale( cuT( "l_viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				l_texCoord.xy() = l_parallaxMapping( l_texCoord.xy(), l_viewDir );
			}

			ComputePreLightingMapContributions( l_writer
				, l_v3Normal
				, l_fMatShininess
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );
			OutputComponents l_output{ l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting( l_worldEye
				, l_fMatShininess
				, c3d_shadowReceiver
				, FragmentInput( vtx_position, l_v3Normal )
				, l_output );
			ComputePostLightingMapContributions( l_writer
				, l_diffuse
				, l_v3Specular
				, l_emissive
				, l_gamma
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );

			auto l_colour = l_writer.DeclLocale< Vec4 >( cuT( "l_colour" ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
				|| CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
			{
				auto l_incident = l_writer.DeclLocale( cuT( "l_i" ), normalize( vtx_position - c3d_v3CameraPosition ) );
				auto l_reflectedColour = l_writer.DeclLocale( cuT( "l_reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto l_refractedColour = l_writer.DeclLocale( cuT( "l_refractedColour" ), l_diffuse / 2.0 );

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection ) )
				{
					auto l_reflect = l_writer.DeclLocale( cuT( "l_reflect" )
						, reflect( l_incident, l_v3Normal ) );
					l_reflectedColour = texture( c3d_mapEnvironment, l_reflect ).xyz() * length( pxl_v4Accumulation.xyz() );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					auto l_refract = l_writer.DeclLocale( cuT( "l_refract" ), refract( l_incident, l_v3Normal, l_materials.GetRefractionRatio( vtx_material ) ) );
					l_refractedColour = texture( c3d_mapEnvironment, l_refract ).xyz() * l_diffuse / length( l_diffuse );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
					&& !CheckFlag( p_textureFlags, TextureChannel::eRefraction ) )
				{
					l_colour.xyz() = l_reflectedColour * l_diffuse / length( l_diffuse );
				}
				else
				{
					auto l_refFactor = l_writer.DeclLocale( cuT( "l_refFactor" )
						, c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( l_incident, l_v3Normal ), c3d_fresnelPower ) );
					l_colour.xyz() = mix( l_refractedColour, l_reflectedColour, l_refFactor );
				}
			}
			else
			{
				l_colour.xyz() = l_writer.Paren( l_v3Ambient + l_v3Diffuse + l_emissive ) * l_diffuse
					+ l_v3Specular * l_materials.GetSpecular( vtx_material );
			}

			auto l_alpha = l_writer.DeclLocale( cuT( "l_alpha" ), l_materials.GetOpacity( vtx_material ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				l_alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto l_wvPosition = l_writer.DeclLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				l_fog.ApplyFog( l_colour, length( l_wvPosition ), l_wvPosition.y() );
			}

			//// Naive
			//auto l_depth = l_utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			//auto l_weight = l_writer.DeclLocale( cuT( "l_weight" ), 1.0_f - l_depth );

			// (10)
			auto l_depth = l_utils.LineariseDepth( gl_FragCoord.z(), c3d_mtxInvProjection );
			auto l_weight = l_writer.DeclLocale( cuT( "l_weight" )
				, max( pow( 1.0_f - l_depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto l_weight = l_writer.DeclLocale( cuT( "l_weight" )
			//	, max( min( 0.03_f / l_writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto l_weight = l_writer.DeclLocale( cuT( "l_weight" )
			//	, max( min( 10.0_f / l_writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto l_weight = l_writer.DeclLocale( cuT( "l_weight" )
			//	, max( min( 10.0_f / l_writer.Paren( pow( GLSL::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( GLSL::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = l_writer.DeclLocale( cuT( "a" )
			//	, min( l_alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = l_writer.DeclLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto l_weight = l_writer.DeclLocale( cuT( "l_weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_v4Accumulation = vec4( l_colour.xyz() * l_alpha, l_alpha ) * l_weight;
			pxl_v4Revealage = l_alpha;
		} );

		return l_writer.Finalise();
	}

	void TransparentPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & l_scene = *m_camera->GetScene();
		m_sceneUbo.Update( l_scene, *m_camera, true );
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

		if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
		{
			p_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
		}

		if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
		{
			p_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
		}
	}
}
