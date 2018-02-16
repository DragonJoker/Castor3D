#include "RenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename NodeType >
		std::unique_ptr< DistanceRenderNodeBase > MakeDistanceNode( NodeType & node )
		{
			return std::make_unique< DistanceRenderNode< NodeType > >( node );
		}

		template< typename MapType >
		void doSortAlpha( MapType & input
			, Camera const & camera
			, RenderPass::DistanceSortedNodeMap & output )
		{
			for ( auto & itPipelines : input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4r mtxMeshGlobal = renderNode.m_sceneNode.getDerivedTransformationMatrix().getInverse().transpose();
					Point3r position = camera.getParent()->getDerivedPosition();
					Point3r ptCameraLocal = position * mtxMeshGlobal;
					renderNode.m_data.sortByDistance( ptCameraLocal );
					ptCameraLocal -= renderNode.m_sceneNode.getPosition();
					output.emplace( point::lengthSquared( ptCameraLocal ), MakeDistanceNode( renderNode ) );
				}
			}
		}

		inline void doUpdateProgram( ShaderProgram & program
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )
		{
			if ( getShadowType( sceneFlags ) != ShadowType::eNone
				&& !program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot, renderer::ShaderStageFlag::eFragment ) )
			{
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowDirectional
					, renderer::ShaderStageFlag::eFragment );
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot
					, renderer::ShaderStageFlag::eFragment, shader::SpotShadowMapCount );
				program.createUniform< UniformType::eSampler >( shader::Shadow::MapShadowPoint
					, renderer::ShaderStageFlag::eFragment, shader::PointShadowMapCount );
			}

			if ( ( checkFlag( passFlags, PassFlag::ePbrMetallicRoughness )
					|| checkFlag( passFlags, PassFlag::ePbrSpecularGlossiness ) )
				&& checkFlag( programFlags, ProgramFlag::eLighting ) )
			{
				program.createUniform< UniformType::eSampler >( cuT( "c3d_mapIrradiance" )
					, renderer::ShaderStageFlag::eFragment );
				program.createUniform< UniformType::eSampler >( cuT( "c3d_mapPrefiltered" )
					, renderer::ShaderStageFlag::eFragment );
				program.createUniform< UniformType::eSampler >( cuT( "c3d_mapBrdf" )
					, renderer::ShaderStageFlag::eFragment );
			}
		}

		inline BlendState doCreateBlendState( BlendMode colourBlendMode
			, BlendMode alphaBlendMode )
		{
			BlendState state;
			bool blend = false;

			switch ( colourBlendMode )
			{
			case BlendMode::eNoBlend:
				state.setRgbSrcBlend( BlendOperand::eOne );
				state.setRgbDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				blend = true;
				state.setRgbSrcBlend( BlendOperand::eOne );
				state.setRgbDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				blend = true;
				state.setRgbSrcBlend( BlendOperand::eZero );
				state.setRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			case BlendMode::eInterpolative:
				blend = true;
				state.setRgbSrcBlend( BlendOperand::eSrcColour );
				state.setRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			default:
				blend = true;
				state.setRgbSrcBlend( BlendOperand::eSrcColour );
				state.setRgbDstBlend( BlendOperand::eInvSrcColour );
				break;
			}

			switch ( alphaBlendMode )
			{
			case BlendMode::eNoBlend:
				state.setAlphaSrcBlend( BlendOperand::eOne );
				state.setAlphaDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				blend = true;
				state.setAlphaSrcBlend( BlendOperand::eOne );
				state.setAlphaDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				blend = true;
				state.setAlphaSrcBlend( BlendOperand::eZero );
				state.setAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.setRgbSrcBlend( BlendOperand::eZero );
				state.setRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			case BlendMode::eInterpolative:
				blend = true;
				state.setAlphaSrcBlend( BlendOperand::eSrcAlpha );
				state.setAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.setRgbSrcBlend( BlendOperand::eSrcAlpha );
				state.setRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			default:
				blend = true;
				state.setAlphaSrcBlend( BlendOperand::eSrcAlpha );
				state.setAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				state.setRgbSrcBlend( BlendOperand::eSrcAlpha );
				state.setRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;
			}

			state.enableBlend( blend );
			return state;
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( String const & name
		, Scene const & scene
		, Camera * camera
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ name, *scene.getEngine(), ignored }
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
		: RenderPass{ name, *scene.getEngine(), oit, ignored }
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

	void RenderTechniquePass::doUpdate( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		doRenderNodes( m_renderQueue.getRenderNodes()
			, *m_camera
			, shadowMaps
			, jitter
			, info );
	}

	void RenderTechniquePass::doRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, RenderInfo & info )const
	{
		if ( !nodes.m_staticNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !nodes.m_skinnedNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !nodes.m_morphingNodes.m_backCulled.empty()
			|| !nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_timer->start();
			auto jitterProjSpace = jitter * 2.0_r;
			jitterProjSpace[0] /= camera.getWidth();
			jitterProjSpace[1] /= camera.getHeight();
			m_matrixUbo.update( camera.getView()
				, camera.getViewport().getProjection()
				, jitterProjSpace );
			RenderPass::doUpdate( nodes.m_instantiatedStaticNodes.m_frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_staticNodes.m_frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_skinnedNodes.m_frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_instantiatedSkinnedNodes.m_frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_morphingNodes.m_frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_billboardNodes.m_frontCulled, camera, shadowMaps );

			RenderPass::doUpdate( nodes.m_instantiatedStaticNodes.m_backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.m_staticNodes.m_backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.m_skinnedNodes.m_backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.m_instantiatedSkinnedNodes.m_backCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.m_morphingNodes.m_backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.m_billboardNodes.m_backCulled, camera, shadowMaps, info );
			m_timer->stop();
		}
	}

	bool RenderTechniquePass::doInitialise( Size const & CU_PARAM_UNUSED( size ) )
	{
		if ( m_camera )
		{
			m_renderQueue.initialise( m_scene, *m_camera );
		}
		else
		{
			m_renderQueue.initialise( m_scene );
		}

		return true;
	}

	void RenderTechniquePass::doCleanup()
	{
	}

	void RenderTechniquePass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void RenderTechniquePass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eLighting );

		if ( m_environment )
		{
			addFlag( programFlags, ProgramFlag::eEnvironmentMapping );
		}
	}

	glsl::Shader RenderTechniquePass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	void RenderTechniquePass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		m_sceneUbo.update( m_scene, *m_camera );
	}

	void RenderTechniquePass::doPrepareFrontPipeline( ShaderProgram & program
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

			if ( !m_opaque )
			{
				dsState.setDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, doCreateBlendState( flags.m_colourBlendMode, flags.m_alphaBlendMode )
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
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
				} ) );
		}
	}

	void RenderTechniquePass::doPrepareBackPipeline( ShaderProgram & program
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

			if ( !m_opaque )
			{
				dsState.setDepthMask( WritingMask::eZero );
			}

			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, doCreateBlendState( flags.m_colourBlendMode, flags.m_alphaBlendMode )
					, MultisampleState{}
				, program
				, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
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
				} ) );
		}
	}
}
