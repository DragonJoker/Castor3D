#include "RenderTechniquePass.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Mesh/Submesh.hpp"
#include "Miscellaneous/ShadowMapPassDirectional.hpp"
#include "Miscellaneous/ShadowMapPassPoint.hpp"
#include "Miscellaneous/ShadowMapPassSpot.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
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
					p_output.insert( { point::distance_squared( l_ptCameraLocal ), l_renderNode } );
				}
			}
		}

		inline void DoUpdateProgram( ShaderProgram & p_program
			, ProgramFlags const & p_programFlags )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::eShadows )
				&& !p_program.FindFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel ) )
			{
				p_program.CreateFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel );
				p_program.CreateFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel );
				p_program.CreateFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel );
			}
		}

		void DoFillShaderDepthMaps( RenderPipeline & p_pipeline
			, DepthMapArray & p_depthMaps )
		{
			if ( !p_depthMaps.empty() )
			{
				auto l_index = p_pipeline.GetTexturesCount() + 1;

				if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eShadows ) )
				{
					auto & l_directional = p_pipeline.GetDirectionalShadowMapsVariable();
					auto & l_spot = p_pipeline.GetSpotShadowMapsVariable();
					auto & l_point = p_pipeline.GetPointShadowMapsVariable();

					for ( auto & l_depthMap : p_depthMaps )
					{
						if ( l_depthMap.get().GetType() == TextureType::eTwoDimensions )
						{
							l_depthMap.get().SetIndex( l_index );
							l_directional.SetValue( l_index++ );
						}
						else if ( l_depthMap.get().GetType() == TextureType::eTwoDimensionsArray )
						{
							l_depthMap.get().SetIndex( l_index );
							l_spot.SetValue( l_index++ );
						}
						else if ( l_depthMap.get().GetType() == TextureType::eCube )
						{
							l_depthMap.get().SetIndex( l_index );
							l_point.SetValue( l_index++ );
						}
						else if ( l_depthMap.get().GetType() == TextureType::eCubeArray )
						{
							l_depthMap.get().SetIndex( l_index );
							l_point.SetValue( l_index++ );
						}
					}
				}
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
		, RenderTarget & p_renderTarget
		, RenderTechnique & p_technique
		, bool p_opaque
		, bool p_multisampling )
		: RenderPass{ p_name, *p_renderTarget.GetEngine(), p_opaque, p_multisampling }
		, m_target{ p_renderTarget }
		, m_technique{ p_technique }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	void RenderTechniquePass::Render( uint32_t p_frameTime, uint32_t & p_visible, bool p_shadows )
	{
		auto & l_nodes = m_renderQueue.GetRenderNodes();
		auto & l_scene = *m_target.GetScene();
		DepthMapArray l_depthMaps;

		if ( p_shadows )
		{
			DoGetDepthMaps( l_depthMaps );
		}

		DoRenderNodes( l_nodes, *m_target.GetCamera(), l_depthMaps, p_visible );
		p_visible += uint32_t( p_visible );
	}

	void RenderTechniquePass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		if ( !p_nodes.m_staticGeometries.m_backCulled.empty()
			|| !p_nodes.m_instancedGeometries.m_backCulled.empty()
			|| !p_nodes.m_animatedGeometries.m_backCulled.empty()
			|| !p_nodes.m_billboards.m_backCulled.empty() )
		{
			if ( m_opaque || m_multisampling )
			{
				DoRenderInstancedSubmeshes( p_nodes.m_instancedGeometries.m_frontCulled, p_camera, p_depthMaps );
				DoRenderStaticSubmeshes( p_nodes.m_staticGeometries.m_frontCulled, p_camera, p_depthMaps );
				DoRenderAnimatedSubmeshes( p_nodes.m_animatedGeometries.m_frontCulled, p_camera, p_depthMaps );
				DoRenderBillboards( p_nodes.m_billboards.m_frontCulled, p_camera, p_depthMaps );

				DoRenderInstancedSubmeshes( p_nodes.m_instancedGeometries.m_backCulled, p_camera, p_depthMaps, p_count );
				DoRenderStaticSubmeshes( p_nodes.m_staticGeometries.m_backCulled, p_camera, p_depthMaps, p_count );
				DoRenderAnimatedSubmeshes( p_nodes.m_animatedGeometries.m_backCulled, p_camera, p_depthMaps, p_count );
				DoRenderBillboards( p_nodes.m_billboards.m_backCulled, p_camera, p_depthMaps, p_count );
			}
			else
			{
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_frontCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_frontCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_frontCulled, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_camera, p_depthMaps );
					}
				}
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_backCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_backCulled, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_backCulled, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_camera, p_depthMaps, p_count );
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
			l_it.second.get().m_pass.m_pipeline.Apply();
			UpdatePipeline( p_camera, l_it.second.get().m_pass.m_pipeline, p_depthMaps );
			l_it.second.get().Render( p_depthMaps );
		}
	}

	void RenderTechniquePass::DoRenderByDistance( DistanceSortedNodeMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		for ( auto & l_it : p_nodes )
		{
			l_it.second.get().m_pass.m_pipeline.Apply();
			UpdatePipeline( p_camera, l_it.second.get().m_pass.m_pipeline, p_depthMaps );
			l_it.second.get().Render( p_depthMaps );
			++p_count;
		}
	}

	void RenderTechniquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_technique.GetDirectionalShadowMap() ) );
		p_depthMaps.push_back( std::ref( m_technique.GetSpotShadowMap() ) );
		p_depthMaps.push_back( std::ref( m_technique.GetPointShadowMap() ) );
	}

	bool RenderTechniquePass::DoInitialise( Size const & CU_PARAM_UNUSED( p_size ) )
	{
		auto & l_scene = *m_target.GetScene();
		auto l_camera = m_target.GetCamera();

		if ( l_camera )
		{
			m_renderQueue.Initialise( l_scene, *m_target.GetCamera() );
		}
		else
		{
			m_renderQueue.Initialise( l_scene );
		}

		return true;
	}

	void RenderTechniquePass::DoCleanup()
	{
	}

	void RenderTechniquePass::DoUpdate()
	{
		m_renderQueue.Update();
	}

	void RenderTechniquePass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eLighting );
	}

	String RenderTechniquePass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return String{};
	}

	String RenderTechniquePass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );
		UBO_MODEL( l_writer );

		// Fragment Intputs
		auto vtx_worldSpacePosition = l_writer.GetInput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
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

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::eColour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::eAmbient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, CheckFlag( p_programFlags, ProgramFlag::eShadows ) ? ShadowType::ePoisson : ShadowType::eNone );
		GLSL::Fog l_fog{ p_sceneFlags, l_writer };

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			Float l_fAlpha;

			if ( !m_opaque )
			{
				l_fAlpha = l_writer.GetLocale( cuT( "l_fAlpha" ), c3d_fMatOpacity );
			}

			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );

			OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting( l_worldEye
				, l_fMatShininess
				, c3d_iShadowReceiver
				, FragmentInput( vtx_worldSpacePosition, l_v3Normal )
				, l_output );

			ComputePostLightingMapContributions( l_writer, l_v3Ambient, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) && !m_opaque )
			{
				l_fAlpha = texture( c3d_mapOpacity, vtx_texture.xy() ).r() * c3d_fMatOpacity;
			}

			if ( m_opaque )
			{
				pxl_v4FragColor = vec4( l_writer.Paren( l_v3Ambient * c3d_v4MatAmbient.xyz() )
						+ l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() )
						+ l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() )
						+ l_v3Emissive
					, 1.0_f );
			}
			else
			{
				pxl_v4FragColor = vec4( l_fAlpha * l_writer.Paren(
					l_writer.Paren( l_v3Ambient * c3d_v4MatAmbient.xyz() )
						+ l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() )
						+ l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() )
						+ l_v3Emissive )
					, l_fAlpha );
			}

			if ( p_sceneFlags != 0 )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.y() );
			}
		} );

		return l_writer.Finalise();
	}

	void RenderTechniquePass::DoUpdatePipeline( RenderPipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		auto & l_camera = *m_target.GetCamera();
		l_camera.GetScene()->GetLightCache().FillShader( l_sceneUbo );
		l_camera.GetScene()->GetFog().FillShader( l_sceneUbo );
		l_camera.GetScene()->FillShader( l_sceneUbo );
		DoFillShaderDepthMaps( p_pipeline, p_depthMaps );
	}

	void RenderTechniquePass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontPipelines.find( p_flags );

		if ( l_it == m_frontPipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			DepthStencilState l_dsState;

			if ( !m_opaque )
			{
				l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			}

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling && !m_opaque );
			m_frontPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
					, std::move( l_msState )
					, p_program
					, p_flags ) );
		}
	}

	void RenderTechniquePass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backPipelines.find( p_flags );

		if ( l_it == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			DepthStencilState l_dsState;

			if ( !m_opaque )
			{
				l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			}

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling && !m_opaque );
			m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
				, std::move( l_rsState )
				, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
				, std::move( l_msState )
				, p_program
				, p_flags ) );
		}
	}
}
