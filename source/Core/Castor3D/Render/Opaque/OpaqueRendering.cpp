#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Opaque/VisibilityReorderPass.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Prepass/PrepassRendering.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"
#include "Castor3D/Render/Ssao/SsaoPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementSmartPtr( castor3d, OpaqueRendering )

namespace castor3d
{
	OpaqueRendering::OpaqueRendering( RenderTechnique & parent
		, RenderDevice const & device
		, QueueData const & queueData
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses
		, SsaoConfig const & ssaoConfig
		, ProgressBar * progress
		, Texture const * normal )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Opaque" ) }
		, m_materialsCounts1{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + "/MaterialsCounts1" )
			: nullptr ) }
		, m_materialsCounts2{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + "/MaterialsCounts2" )
			: nullptr ) }
		, m_materialsStarts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + "/MaterialsStarts" )
			: nullptr ) }
		, m_pixelsXY{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< castor::Point2ui >( m_device
				, getOwner()->getTargetExtent().width * getOwner()->getTargetExtent().height
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + "/PixelsXY" )
			: nullptr ) }
		, m_visibilityPipelinesIds{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? castor::makeUnique< ShaderBuffer >( *getOwner()->getEngine()
				, m_device
				, MaxObjectNodesCount * sizeof( uint32_t )
				, getOwner()->getName() + "/MaterialsPipelinesIds" )
			: nullptr ) }
		, m_visibilityReorder{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? castor::makeUnique< VisibilityReorderPass >( m_graph
				, crg::FramePassArray{ &previous.getLastPass() }
				, m_device
				, previous.getVisibility().sampledViewId
				, *m_visibilityPipelinesIds.get()
				, *m_materialsCounts1
				, *m_materialsCounts2
				, *m_materialsStarts
				, *m_pixelsXY )
			: nullptr ) }
		, m_ssao{ doCreateSsaoPass( progress, previous.getLastPass(), previousPasses ) }
		, m_visibilityResolveDesc{ ( previous.hasVisibility()
			? &doCreateVisibilityResolve( progress, previous, { &m_ssao->getLastPass() } )
			: nullptr ) }
		, m_opaquePassDesc{ ( m_visibilityResolveDesc
			? m_visibilityResolveDesc
			: &doCreateOpaquePass( progress, previous.getLastPass(), previousPasses ) ) }
		, m_opaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled(); } } }
	{
		if ( m_visibilityPipelinesIds )
		{
			getEngine()->registerBuffer( *m_visibilityPipelinesIds );
		}

		m_graph.addGroupOutput( getOwner()->getTargetResult().front() );
		m_graph.addGroupOutput( getOwner()->getTargetResult().back() );
	}

	OpaqueRendering::~OpaqueRendering()
	{
		if ( m_visibilityPipelinesIds )
		{
			getEngine()->unregisterBuffer( *m_visibilityPipelinesIds );
		}

		m_visibilityReorder.reset();
	}

	SsaoPassUPtr OpaqueRendering::doCreateSsaoPass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray previousPasses )const
	{
		previousPasses.push_back( &lastPass );
		return castor::makeUnique< SsaoPass >( m_graph
			, m_device
			, progress
			, previousPasses
			, makeSize( getOwner()->getTargetExtent() )
			, getOwner()->getSsaoConfig()
			, getOwner()->getDepthObj()
			, getOwner()->getNormal()
			, getOwner()->getCameraUbo() );
	}

	uint32_t OpaqueRendering::countInitialisationSteps()
	{
		uint32_t result = 0u;
		result += SsaoPass::countInitialisationSteps();
		result += 1;// visibility pass;
		return result;
	}

	void OpaqueRendering::update( CpuUpdater & updater )
	{
		if ( !m_opaquePass && !m_visibilityResolve )
		{
			return;
		}

		if ( getOwner()->getSsaoConfig().enabled )
		{
			m_ssao->update( updater );
		}

		auto & scene = *updater.scene;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( m_opaquePass )
		{
			m_opaquePass->update( updater );
		}
	}

	void OpaqueRendering::update( GpuUpdater & updater )
	{
		if ( !m_opaquePass && !m_visibilityResolve )
		{
			return;
		}

		if ( m_opaquePass )
		{
			m_opaquePass->countNodes( updater.info );
		}
	}

	void OpaqueRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_ssao->accept( visitor );

		if ( m_opaquePass
			&& m_opaquePass->areValidPassFlags( visitor.getFlags().components ) )
		{
			m_opaquePass->accept( visitor );
		}
	}

	Engine * OpaqueRendering::getEngine()const
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & OpaqueRendering::getLastPass()const
	{
		return *m_opaquePassDesc;
	}

	Texture const & OpaqueRendering::getSsaoResult()const
	{
		return m_ssao->getResult();
	}

	Texture const & OpaqueRendering::getDiffuseLightingResult()const
	{
		return getOwner()->getResult();
	}

	Texture const & OpaqueRendering::getBaseColourResult()const
	{
		return getOwner()->getResult();
	}

	bool OpaqueRendering::isEnabled()const
	{
		return m_opaquePassEnabled();
	}

	crg::FramePass & OpaqueRendering::doCreateVisibilityResolve( ProgressBar * progress
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating visibility resolve pass" );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "Visibility"
			, [this, targetResult, targetDepth, progress, &previous]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising visibility resolve pass" );
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting();
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();

				if ( !VisibilityResolvePass::useCompute )
				{
					auto resultIt = framePass.images.begin();
					renderPassDesc.implicitAction( resultIt->view(), crg::RecordContext::clearAttachment( *resultIt ) );
				}

				auto res = std::make_unique< VisibilityResolvePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, cuT( "Resolve" )
					, previous.getVisibilityPass()
					, targetResult
					, targetDepth
					, m_visibilityPipelinesIds.get()
					, std::move( renderPassDesc )
					, std::move( techniquePassDesc ) );
				m_opaquePass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		uint32_t index = 0u;
		result.addInputStorageView( previous.getVisibility().targetViewId
			, index++ );

		if constexpr ( VisibilityResolvePass::useCompute )
		{
			result.addDependency( m_visibilityReorder->getLastPass() );
			result.addInputStorageBuffer( { m_materialsCounts1->getBuffer(), "MaterialsCount" }
				, index++
				, 0u
				, uint32_t( m_materialsCounts1->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_materialsStarts->getBuffer(), "MaterialsStarts" }
				, index++
				, 0u
				, uint32_t( m_materialsStarts->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_pixelsXY->getBuffer(), "PixelsXY" }
				, index++
				, 0u
				, uint32_t( m_pixelsXY->getBuffer().getSize() ) );
			result.addOutputStorageView( targetResult, index++ );
			result.addOutputStorageView( getOwner()->getScattering().targetViewId, index++ );
		}
		else
		{
			result.addInOutColourView( targetResult );
			result.addOutputColourView( getOwner()->getScattering().targetViewId );
		}

		return result;
	}

	crg::FramePass & OpaqueRendering::doCreateOpaquePass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating opaque pass" );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "NodesPass"
			, [this, targetResult, targetDepth, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising opaque pass" );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags );
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();
				auto res = std::make_unique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, std::move( renderPassDesc )
					, std::move( techniquePassDesc ) );
				m_opaquePass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addDependencies( previousPasses );
		result.addDependency( m_ssao->getLastPass() );
		result.addSampledView( m_ssao->getResult().sampledViewId, 0u );
		result.addInOutDepthStencilView( targetDepth );
		result.addInOutColourView( targetResult );
		result.addOutputColourView( getOwner()->getScattering().targetViewId );
		return result;
	}

	bool OpaqueRendering::doIsOpaquePassEnabled()const
	{
		CU_Require( m_opaquePass );
		return m_opaquePass->isPassEnabled();
	}
}
