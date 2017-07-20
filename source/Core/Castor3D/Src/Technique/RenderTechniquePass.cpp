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
			for ( auto & itPipelines : p_input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4r mtxMeshGlobal = renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
					Point3r position = p_camera.GetParent()->GetDerivedPosition();
					Point3r ptCameraLocal = position * mtxMeshGlobal;
					renderNode.m_data.SortByDistance( ptCameraLocal );
					ptCameraLocal -= renderNode.m_sceneNode.GetPosition();
					p_output.emplace( point::length_squared( ptCameraLocal ), MakeDistanceNode( renderNode ) );
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
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint
					, ShaderType::ePixel, 6u );
			}

			if ( ( CheckFlag( p_programFlags, ProgramFlag::ePbrMetallicRoughness )
					|| CheckFlag( p_programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
				&& CheckFlag( p_programFlags, ProgramFlag::eLighting ) )
			{
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered
					, ShaderType::ePixel );
				p_program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf
					, ShaderType::ePixel );
			}
		}

		inline BlendState DoCreateBlendState( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
		{
			BlendState state;
			bool blend = false;

			switch ( p_colourBlendMode )
			{
			case BlendMode::eNoBlend:
				state.SetRgbSrcBlend( BlendOperand::eOne );
				state.SetRgbDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				blend = true;
				state.SetRgbSrcBlend( BlendOperand::eOne );
				state.SetRgbDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				blend = true;
				state.SetRgbSrcBlend( BlendOperand::eZero );
				state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			case BlendMode::eInterpolative:
				blend = true;
				state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			default:
				blend = true;
				state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;
			}

			switch ( p_alphaBlendMode )
			{
			case BlendMode::eNoBlend:
				state.SetAlphaSrcBlend( BlendOperand::eOne );
				state.SetAlphaDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				blend = true;
				state.SetAlphaSrcBlend( BlendOperand::eOne );
				state.SetAlphaDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				blend = true;
				state.SetAlphaSrcBlend( BlendOperand::eZero );
				state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.SetRgbSrcBlend( BlendOperand::eZero );
				state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			case BlendMode::eInterpolative:
				blend = true;
				state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			default:
				blend = true;
				state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;
			}

			state.EnableBlend( blend );
			return state;
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_environment
		, SceneNode const * p_ignored
		, SsaoConfig const & p_config )
		: RenderPass{ p_name, *p_scene.GetEngine(), p_ignored }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
		, m_sceneNode{}
		, m_environment{ p_environment }
		, m_ssaoConfig{ p_config }
	{
	}

	RenderTechniquePass::RenderTechniquePass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_oit
		, bool p_environment
		, SceneNode const * p_ignored
		, SsaoConfig const & p_config )
		: RenderPass{ p_name, *p_scene.GetEngine(), p_oit, p_ignored }
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
		auto & nodes = m_renderQueue.GetRenderNodes();
		DepthMapArray depthMaps;

		if ( p_shadows )
		{
			DoGetDepthMaps( depthMaps );
		}

		DoRenderNodes( nodes, *m_camera, depthMaps, p_info );
	}
	
	void RenderTechniquePass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		if ( !p_nodes.m_staticNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !p_nodes.m_skinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_morphingNodes.m_backCulled.empty()
			|| !p_nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );
			RenderPass::DoRender( p_nodes.m_instantiatedStaticNodes.m_frontCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_staticNodes.m_frontCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_skinnedNodes.m_frontCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_instantiatedSkinnedNodes.m_frontCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_morphingNodes.m_frontCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_billboardNodes.m_frontCulled, p_camera, p_depthMaps );

			RenderPass::DoRender( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_camera, p_depthMaps, p_info );
			RenderPass::DoRender( p_nodes.m_staticNodes.m_backCulled, p_camera, p_depthMaps, p_info );
			RenderPass::DoRender( p_nodes.m_skinnedNodes.m_backCulled, p_camera, p_depthMaps, p_info );
			RenderPass::DoRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled, p_camera, p_depthMaps );
			RenderPass::DoRender( p_nodes.m_morphingNodes.m_backCulled, p_camera, p_depthMaps, p_info );
			RenderPass::DoRender( p_nodes.m_billboardNodes.m_backCulled, p_camera, p_depthMaps, p_info );
		}
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

	GLSL::Shader RenderTechniquePass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return GLSL::Shader{};
	}

	void RenderTechniquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		m_sceneUbo.Update( m_scene, *m_camera );
	}

	void RenderTechniquePass::DoPrepareFrontPipeline( ShaderProgram & p_program
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

			if ( !m_opaque )
			{
				dsState.SetDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
				{
					pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}
				} ) );
		}
	}

	void RenderTechniquePass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
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

			if ( !m_opaque )
			{
				dsState.SetDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
					, MultisampleState{}
				, p_program
				, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
				{
					pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}
				} ) );
		}
	}
}
