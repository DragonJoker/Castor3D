#include "RenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename NodeType >
		std::unique_ptr< DistanceRenderNodeBase > MakeDistanceNode( NodeType & p_node )
		{
			return std::make_unique< DistanceRenderNode< NodeType > >( p_node );
		}

		template< typename MapType >
		void DoSortAlpha( MapType & p_input
			, Camera const & p_camera
			, RenderPass::DistanceSortedNodeMap & p_output )
		{
			for ( auto & l_itPipelines : p_input )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
					Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
					Point3r l_ptCameraLocal = l_position * l_mtxMeshGlobal;
					l_renderNode.m_data.SortByDistance( l_ptCameraLocal );
					l_ptCameraLocal -= l_renderNode.m_sceneNode.GetPosition();
					p_output.emplace( point::length_squared( l_ptCameraLocal ), MakeDistanceNode( l_renderNode ) );
				}
			}
		}

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

			if ( CheckFlag( p_textureFlags, TextureChannel::eReflection )
				&& !p_program.FindUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel ) )
			{
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );
			}
		}

		inline BlendState DoCreateBlendState( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
		{
			BlendState l_state;
			bool l_blend = false;

			switch ( p_colourBlendMode )
			{
			case BlendMode::eNoBlend:
				l_state.SetRgbSrcBlend( BlendOperand::eOne );
				l_state.SetRgbDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eOne );
				l_state.SetRgbDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eZero );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			case BlendMode::eInterpolative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			default:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;
			}

			switch ( p_alphaBlendMode )
			{
			case BlendMode::eNoBlend:
				l_state.SetAlphaSrcBlend( BlendOperand::eOne );
				l_state.SetAlphaDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eOne );
				l_state.SetAlphaDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eZero );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eZero );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			case BlendMode::eInterpolative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			default:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;
			}

			l_state.EnableBlend( l_blend );
			return l_state;
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_opaque
		, bool p_multisampling
		, bool p_environment
		, SceneNode const * p_ignored )
		: RenderPass{ p_name, *p_scene.GetEngine(), p_opaque, p_multisampling, p_ignored }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
		, m_sceneNode{ m_sceneUbo }
		, m_environment{ p_environment }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	void RenderTechniquePass::Render( RenderInfo & p_info, bool p_shadows )
	{
		auto & l_nodes = m_renderQueue.GetRenderNodes();
		DepthMapArray l_depthMaps;

		if ( p_shadows )
		{
			DoGetDepthMaps( l_depthMaps );
		}

		DoRenderNodes( l_nodes, *m_camera, l_depthMaps, p_info );
	}

	void RenderTechniquePass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )
	{
		if ( !p_nodes.m_staticNodes.m_backCulled.empty()
			|| !p_nodes.m_instancedNodes.m_backCulled.empty()
			|| !p_nodes.m_skinningNodes.m_backCulled.empty()
			|| !p_nodes.m_morphingNodes.m_backCulled.empty()
			|| !p_nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_projectionUniform->SetValue( p_camera.GetViewport().GetProjection() );
			m_viewUniform->SetValue( p_camera.GetView() );
			m_matrixUbo.Update();

			if ( m_opaque || m_multisampling )
			{
				DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_frontCulled, p_camera, p_depthMaps );
				DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_frontCulled, p_camera, p_depthMaps );
				DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_frontCulled, p_camera, p_depthMaps );
				DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_frontCulled, p_camera, p_depthMaps );
				DoRenderBillboards( p_nodes.m_billboardNodes.m_frontCulled, p_camera, p_depthMaps );

				DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_backCulled, p_camera, p_depthMaps, p_info );
				DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_backCulled, p_camera, p_depthMaps, p_info );
				DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_backCulled, p_camera, p_depthMaps, p_info );
				DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_backCulled, p_camera, p_depthMaps, p_info );
				DoRenderBillboards( p_nodes.m_billboardNodes.m_backCulled, p_camera, p_depthMaps, p_info );
			}
			else
			{
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticNodes.m_frontCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_skinningNodes.m_frontCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_morphingNodes.m_frontCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboardNodes.m_frontCulled, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_camera, p_depthMaps );
					}
				}
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticNodes.m_backCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_skinningNodes.m_backCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_morphingNodes.m_backCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboardNodes.m_backCulled, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_camera, p_depthMaps, p_info );
					}
				}
			}
		}
	}

	void RenderTechniquePass::DoRenderByDistance( DistanceSortedNodeMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )
	{
		for ( auto & l_it : p_nodes )
		{
			l_it.second->GetPipeline().Apply();
			UpdatePipeline( l_it.second->GetPipeline() );

			DoBindPass( l_it.second->GetPassNode()
				, l_it.second->GetSceneNode()
				, l_it.second->GetPassNode().m_pass
				, *p_camera.GetScene()
				, l_it.second->GetPipeline()
				, p_depthMaps );

			l_it.second->Render();

			DoUnbindPass( l_it.second->GetPassNode()
				, l_it.second->GetSceneNode()
				, l_it.second->GetPassNode().m_pass
				, *p_camera.GetScene()
				, l_it.second->GetPipeline()
				, p_depthMaps );
		}
	}

	void RenderTechniquePass::DoRenderByDistance( DistanceSortedNodeMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )
	{
		DoRenderByDistance( p_nodes, p_camera, p_depthMaps );
		p_info.m_visibleObjectsCount += uint32_t( p_nodes.size() );
		p_info.m_drawCalls += uint32_t( p_nodes.size() );
	}

	bool RenderTechniquePass::DoInitialise( Size const & CU_PARAM_UNUSED( p_size ) )
	{
		if ( m_camera )
		{
			m_renderQueue.Initialise( m_scene, *m_camera );
		}
		else
		{
			m_renderQueue.Initialise( m_scene );
		}

		return true;
	}

	void RenderTechniquePass::DoCleanup()
	{
	}

	void RenderTechniquePass::DoUpdate( RenderQueueArray & p_queues )
	{
		p_queues.push_back( m_renderQueue );
	}

	void RenderTechniquePass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eLighting );

		if ( m_environment )
		{
			AddFlag( p_programFlags, ProgramFlag::eEnvironmentMapping );
		}
	}

	String RenderTechniquePass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return String{};
	}

	String RenderTechniquePass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );
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
		auto c3d_mapEnvironment( l_writer.GetUniform< SamplerCube >( ShaderProgram::MapEnvironment, CheckFlag( p_textureFlags, TextureChannel::eReflection ) ) );

		auto c3d_fheightScale( l_writer.GetUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		auto l_parallaxMapping = DeclareParallaxMappingFunc( l_writer, p_textureFlags, p_programFlags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			auto l_envAmbient = l_writer.GetLocale( cuT( "l_envAmbient" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			auto l_envDiffuse = l_writer.GetLocale( cuT( "l_envDiffuse" ), vec3( 1.0_f, 1.0_f, 1.0_f ) );
			Float l_fAlpha;

			if ( !m_opaque )
			{
				l_fAlpha = l_writer.GetLocale( cuT( "l_fAlpha" ), c3d_fMatOpacity );
			}

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 0.0f );
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto l_viewDir = -l_writer.GetLocale( cuT( "l_viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				l_texCoord.xy() = l_parallaxMapping( l_texCoord.xy(), l_viewDir );
			}

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );

			if ( CheckFlag( p_textureFlags, TextureChannel::eReflection ) )
			{
				auto l_i = l_writer.GetLocale( cuT( "l_i" ), vtx_position - c3d_v3CameraPosition );
				auto l_r = l_writer.GetLocale( cuT( "l_r" ), reflect( l_i, l_v3Normal ) );
				auto l_environment = l_writer.GetLocale( cuT( "l_environment" ), texture( c3d_mapEnvironment, l_r ).xyz() );
				l_envAmbient = l_environment * 1.4;
				l_envDiffuse = l_environment * 2.0;
			}

			OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting( l_worldEye
				, l_fMatShininess
				, c3d_iShadowReceiver
				, FragmentInput( vtx_position, l_v3Normal )
				, l_output );

			ComputePostLightingMapContributions( l_writer, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque )
			{
				l_fAlpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			pxl_v4FragColor.xyz() = l_v3Ambient * l_envAmbient
				+ l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() * l_envDiffuse )
				+ l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() )
				+ l_v3Emissive;

			if ( m_opaque )
			{
				pxl_v4FragColor.a() = 1.0_f;
			}
			else
			{
				pxl_v4FragColor.a() = l_fAlpha;
			}

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.y() );
			}
		} );

		return l_writer.Finalise();
	}

	void RenderTechniquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & l_fog = m_scene.GetFog();
		m_sceneNode.m_fogType.SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_sceneNode.m_fogDensity.SetValue( l_fog.GetDensity() );
		}

		m_sceneNode.m_ambientLight.SetValue( rgba_float( m_scene.GetAmbientLight() ) );
		{
			auto & l_cache = m_scene.GetLightCache();
			auto l_lock = make_unique_lock( l_cache );
			m_sceneNode.m_lightsCount.GetValue( 0 )[size_t( LightType::eSpot )] = l_cache.GetLightsCount( LightType::eSpot );
			m_sceneNode.m_lightsCount.GetValue( 0 )[size_t( LightType::ePoint )] = l_cache.GetLightsCount( LightType::ePoint );
			m_sceneNode.m_lightsCount.GetValue( 0 )[size_t( LightType::eDirectional )] = l_cache.GetLightsCount( LightType::eDirectional );
		}
		m_sceneNode.m_backgroundColour.SetValue( rgba_float( m_scene.GetBackgroundColour() ) );
		m_sceneNode.m_cameraPos.SetValue( m_camera->GetParent()->GetDerivedPosition() );
		m_sceneNode.m_cameraFarPlane.SetValue( m_camera->GetViewport().GetFar() );
		m_sceneNode.m_sceneUbo.Update();
	}

	void RenderTechniquePass::DoPrepareFrontPipeline( ShaderProgram & p_program
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

			if ( !m_opaque )
			{
				l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			}

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling && !m_opaque );
			auto & l_pipeline = *m_frontPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
					, std::move( l_msState )
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
				{
					l_pipeline.AddUniformBuffer( m_matrixUbo );
					l_pipeline.AddUniformBuffer( m_modelMatrixUbo );
					l_pipeline.AddUniformBuffer( m_sceneUbo );
					l_pipeline.AddUniformBuffer( m_passUbo );
					l_pipeline.AddUniformBuffer( m_modelUbo );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						l_pipeline.AddUniformBuffer( m_billboardUbo );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
					{
						l_pipeline.AddUniformBuffer( m_skinningUbo );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						l_pipeline.AddUniformBuffer( m_morphingUbo );
					}
				} ) );
		}
	}

	void RenderTechniquePass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
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

			if ( !m_opaque )
			{
				l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			}

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling && !m_opaque );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
				, std::move( l_rsState )
				, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
				, std::move( l_msState )
				, p_program
				, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
				{
					l_pipeline.AddUniformBuffer( m_matrixUbo );
					l_pipeline.AddUniformBuffer( m_modelMatrixUbo );
					l_pipeline.AddUniformBuffer( m_sceneUbo );
					l_pipeline.AddUniformBuffer( m_passUbo );
					l_pipeline.AddUniformBuffer( m_modelUbo );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						l_pipeline.AddUniformBuffer( m_billboardUbo );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
					{
						l_pipeline.AddUniformBuffer( m_skinningUbo );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						l_pipeline.AddUniformBuffer( m_morphingUbo );
					}
				} ) );
		}
	}
}
