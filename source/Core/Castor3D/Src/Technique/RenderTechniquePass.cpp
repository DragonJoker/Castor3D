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
		, SceneNode const * p_ignored
		, SsaoConfig const & p_config )
		: RenderPass{ p_name, *p_scene.GetEngine(), p_opaque, p_multisampling, p_ignored }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
		, m_sceneNode{}
		, m_environment{ p_environment }
		, m_ssaoConfig{ p_config }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	void RenderTechniquePass::DoRender( RenderInfo & p_info, bool p_shadows )
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
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );

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

			EnvironmentMap * l_envMap = nullptr;
			DoBindPass( l_it.second->GetSceneNode()
				, l_it.second->GetPassNode()
				, *p_camera.GetScene()
				, l_it.second->GetPipeline()
				, p_depthMaps
				, l_it.second->GetModelUbo()
				, l_envMap );

			l_it.second->Render();

			DoUnbindPass( l_it.second->GetSceneNode()
				, l_it.second->GetPassNode()
				, *p_camera.GetScene()
				, l_it.second->GetPipeline()
				, p_depthMaps
				, l_envMap );
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

	void RenderTechniquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		m_sceneUbo.Update( m_scene, *m_camera );
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
					l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
					{
						l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
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
					l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
					{
						l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}
				} ) );
		}
	}
}
