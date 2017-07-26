#include "RenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename NodeType >
		std::unique_ptr< DistanceRenderNodeBase > MakeDistanceNode( NodeType & node )
		{
			return std::make_unique< DistanceRenderNode< NodeType > >( node );
		}

		template< typename MapType >
		void DoSortAlpha( MapType & input
			, Camera const & camera
			, RenderPass::DistanceSortedNodeMap & output )
		{
			for ( auto & itPipelines : input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4r mtxMeshGlobal = renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
					Point3r position = camera.GetParent()->GetDerivedPosition();
					Point3r ptCameraLocal = position * mtxMeshGlobal;
					renderNode.m_data.SortByDistance( ptCameraLocal );
					ptCameraLocal -= renderNode.m_sceneNode.GetPosition();
					output.emplace( point::length_squared( ptCameraLocal ), MakeDistanceNode( renderNode ) );
				}
			}
		}

		inline void DoUpdateProgram( ShaderProgram & program
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			if ( GetShadowType( sceneFlags ) != GLSL::ShadowType::eNone
				&& !program.FindUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel ) )
			{
				program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional
					, ShaderType::ePixel );
				program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot
					, ShaderType::ePixel );
				program.CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint
					, ShaderType::ePixel, 6u );
			}

			if ( ( CheckFlag( programFlags, ProgramFlag::ePbrMetallicRoughness )
					|| CheckFlag( programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
				&& CheckFlag( programFlags, ProgramFlag::eLighting ) )
			{
				program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance
					, ShaderType::ePixel );
				program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered
					, ShaderType::ePixel );
				program.CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf
					, ShaderType::ePixel );
			}
		}

		inline BlendState DoCreateBlendState( BlendMode colourBlendMode
			, BlendMode alphaBlendMode )
		{
			BlendState state;
			bool blend = false;

			switch ( colourBlendMode )
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

			switch ( alphaBlendMode )
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

	RenderTechniquePass::RenderTechniquePass( String const & name
		, Scene & scene
		, Camera * camera
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ name, *scene.GetEngine(), ignored }
		, m_scene{ scene }
		, m_camera{ camera }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
	{
	}

	RenderTechniquePass::RenderTechniquePass( String const & name
		, Scene & scene
		, Camera * camera
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ name, *scene.GetEngine(), oit, ignored }
		, m_scene{ scene }
		, m_camera{ camera }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	void RenderTechniquePass::DoRender( RenderInfo & info, bool shadows )
	{
		auto & nodes = m_renderQueue.GetRenderNodes();
		DepthMapArray depthMaps;

		if ( shadows )
		{
			DoGetDepthMaps( depthMaps );
		}

		DoRenderNodes( nodes, *m_camera, depthMaps, info );
	}

	void RenderTechniquePass::DoRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		if ( !nodes.m_staticNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !nodes.m_skinnedNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !nodes.m_morphingNodes.m_backCulled.empty()
			|| !nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_timer->Start();
			m_matrixUbo.Update( camera.GetView()
				, camera.GetViewport().GetProjection() );
			RenderPass::DoRender( nodes.m_instantiatedStaticNodes.m_frontCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_staticNodes.m_frontCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_skinnedNodes.m_frontCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_instantiatedSkinnedNodes.m_frontCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_morphingNodes.m_frontCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_billboardNodes.m_frontCulled, camera, depthMaps );

			RenderPass::DoRender( nodes.m_instantiatedStaticNodes.m_backCulled, camera, depthMaps, info );
			RenderPass::DoRender( nodes.m_staticNodes.m_backCulled, camera, depthMaps, info );
			RenderPass::DoRender( nodes.m_skinnedNodes.m_backCulled, camera, depthMaps, info );
			RenderPass::DoRender( nodes.m_instantiatedSkinnedNodes.m_backCulled, camera, depthMaps );
			RenderPass::DoRender( nodes.m_morphingNodes.m_backCulled, camera, depthMaps, info );
			RenderPass::DoRender( nodes.m_billboardNodes.m_backCulled, camera, depthMaps, info );
			m_timer->Stop();
		}
	}

	bool RenderTechniquePass::DoInitialise( Size const & CU_PARAM_UNUSED( size ) )
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

	void RenderTechniquePass::DoUpdate( RenderQueueArray & queues )
	{
		queues.push_back( m_renderQueue );
	}

	void RenderTechniquePass::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		AddFlag( programFlags, ProgramFlag::eLighting );

		if ( m_environment )
		{
			AddFlag( programFlags, ProgramFlag::eEnvironmentMapping );
		}
	}

	GLSL::Shader RenderTechniquePass::DoGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	void RenderTechniquePass::DoUpdatePipeline( RenderPipeline & pipeline )const
	{
		m_sceneUbo.Update( m_scene, *m_camera );
	}

	void RenderTechniquePass::DoPrepareFrontPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_frontPipelines.find( flags );

		if ( it == m_frontPipelines.end() )
		{
			DoUpdateProgram( program
				, flags.m_textureFlags
				, flags.m_programFlags
				, flags.m_sceneFlags );
			DepthStencilState dsState;
			dsState.SetDepthTest( true );

			if ( !m_opaque )
			{
				dsState.SetDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, DoCreateBlendState( flags.m_colourBlendMode, flags.m_alphaBlendMode )
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}
				} ) );
		}
	}

	void RenderTechniquePass::DoPrepareBackPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_backPipelines.find( flags );

		if ( it == m_backPipelines.end() )
		{
			DoUpdateProgram( program
				, flags.m_textureFlags
				, flags.m_programFlags
				, flags.m_sceneFlags );
			DepthStencilState dsState;
			dsState.SetDepthTest( true );

			if ( !m_opaque )
			{
				dsState.SetDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, DoCreateBlendState( flags.m_colourBlendMode, flags.m_alphaBlendMode )
					, MultisampleState{}
				, program
				, flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}
				} ) );
		}
	}
}
