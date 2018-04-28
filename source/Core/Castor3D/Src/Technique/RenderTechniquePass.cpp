#include "RenderTechniquePass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename MapType >
		void doSortAlpha( MapType & input
			, Camera const & camera
			, RenderPass::DistanceSortedNodeMap & output )
		{
			for ( auto & itPipelines : input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4r worldMeshMatrix = renderNode.sceneNode.getDerivedTransformationMatrix().getInverse().transpose();
					Point3r worldCameraPosition = camera.getParent()->getDerivedPosition();
					Point3r meshCameraPosition = worldCameraPosition * worldMeshMatrix;
					renderNode.data.sortByDistance( meshCameraPosition );
					meshCameraPosition -= renderNode.sceneNode.getPosition();
					output.emplace( point::lengthSquared( meshCameraPosition ), makeDistanceNode( renderNode ) );
				}
			}
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, Scene const & scene
		, Camera * camera
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ category, name, *scene.getEngine(), ignored }
		, m_scene{ scene }
		, m_camera{ camera }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
	{
	}

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, Scene & scene
		, Camera * camera
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ category, name, *scene.getEngine(), oit, ignored }
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

	renderer::ColourBlendState RenderTechniquePass::createBlendState( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, uint32_t attachesCount )
	{
		renderer::ColourBlendStateAttachment attach;

		switch ( colourBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.srcColorBlendFactor = renderer::BlendFactor::eOne;
			attach.dstColorBlendFactor = renderer::BlendFactor::eZero;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = renderer::BlendFactor::eOne;
			attach.dstColorBlendFactor = renderer::BlendFactor::eOne;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = renderer::BlendFactor::eZero;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcColour;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = renderer::BlendFactor::eSrcColour;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcColour;
			break;

		default:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = renderer::BlendFactor::eSrcColour;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcColour;
			break;
		}

		switch ( alphaBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.srcAlphaBlendFactor = renderer::BlendFactor::eOne;
			attach.dstAlphaBlendFactor = renderer::BlendFactor::eZero;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = renderer::BlendFactor::eOne;
			attach.dstAlphaBlendFactor = renderer::BlendFactor::eOne;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = renderer::BlendFactor::eZero;
			attach.dstAlphaBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			attach.srcColorBlendFactor = renderer::BlendFactor::eZero;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = renderer::BlendFactor::eSrcAlpha;
			attach.dstAlphaBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			attach.srcColorBlendFactor = renderer::BlendFactor::eSrcAlpha;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			break;

		default:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = renderer::BlendFactor::eSrcAlpha;
			attach.dstAlphaBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			attach.srcColorBlendFactor = renderer::BlendFactor::eSrcAlpha;
			attach.dstColorBlendFactor = renderer::BlendFactor::eInvSrcAlpha;
			break;
		}

		renderer::ColourBlendState state;

		for ( auto i = 0u; i < attachesCount; ++i )
		{
			state.attachs.push_back( attach );
		}

		return state;
	}

	void RenderTechniquePass::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
	}

	void RenderTechniquePass::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
	}

	void RenderTechniquePass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet );
	}

	void RenderTechniquePass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet );
	}

	void RenderTechniquePass::doUpdate( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		doUpdateNodes( m_renderQueue.getCulledRenderNodes()
			, *m_camera
			, shadowMaps
			, jitter
			, info );
		doUpdateUbos( *m_camera
			, jitter );
	}

	void RenderTechniquePass::doUpdateNodes( SceneCulledRenderNodes & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, RenderInfo & info )const
	{
		if ( !nodes.staticNodes.backCulled.empty()
			|| !nodes.instancedStaticNodes.backCulled.empty()
			|| !nodes.skinnedNodes.backCulled.empty()
			|| !nodes.instancedSkinnedNodes.backCulled.empty()
			|| !nodes.morphingNodes.backCulled.empty()
			|| !nodes.billboardNodes.backCulled.empty() )
		{
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled, camera, shadowMaps );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, camera, shadowMaps );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, camera, shadowMaps, info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, camera, shadowMaps, info );
		}
	}

	bool RenderTechniquePass::doInitialise( Size const & CU_PARAM_UNUSED( size ) )
	{
		m_finished = getEngine()->getRenderSystem()->getCurrentDevice()->createSemaphore();

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

	void RenderTechniquePass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetFrontPipelines();
	
		if ( pipelines.find( flags ) == pipelines.end() )
		{
			renderer::DepthStencilState dsState;
			dsState.depthWriteEnable = !m_opaque;
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eFront;
			auto & pipeline = *pipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( { m_camera->getViewport().getSize().getWidth(), m_camera->getViewport().getSize().getHeight(), 0, 0 } );
			pipeline.setScissor( { 0, 0, m_camera->getViewport().getSize().getWidth(), m_camera->getViewport().getSize().getHeight() } );

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				auto textureBindings = doCreateTextureBindings( flags );
				auto uboLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				auto texLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( textureBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( uboLayout ) );
				layouts.emplace_back( std::move( texLayout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				pipeline.initialise( getRenderPass(), renderer::PrimitiveTopology::eTriangleList );
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}

	void RenderTechniquePass::doPrepareBackPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetBackPipelines();
	
		if ( pipelines.find( flags ) == pipelines.end() )
		{
			renderer::DepthStencilState dsState;
			dsState.depthWriteEnable = !m_opaque;
			renderer::RasterisationState rsState;
			auto & pipeline = *pipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( { m_camera->getViewport().getSize().getWidth(), m_camera->getViewport().getSize().getHeight(), 0, 0 } );
			pipeline.setScissor( { 0, 0, m_camera->getViewport().getSize().getWidth(), m_camera->getViewport().getSize().getHeight() } );

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				auto textureBindings = doCreateTextureBindings( flags );
				auto uboLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				auto texLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( textureBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( uboLayout ) );
				layouts.emplace_back( std::move( texLayout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				pipeline.initialise( getRenderPass(), renderer::PrimitiveTopology::eTriangleList );
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}

	renderer::DescriptorSetLayoutBindingArray RenderTechniquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		renderer::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eDiffuse ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eSpecular ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eGloss ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eNormal ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eOpacity ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eHeight ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eAmbientOcclusion ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eEmissive ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eTransmittance ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		return textureBindings;
	}
}
