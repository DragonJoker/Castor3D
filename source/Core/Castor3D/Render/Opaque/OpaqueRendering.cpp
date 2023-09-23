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
#include "Castor3D/Render/Opaque/SubsurfaceScatteringPass.hpp"
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
		, m_materialsCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + "/MaterialsCounts1" )
			: nullptr ) }
		, m_materialsIndirectCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? makeBuffer< castor::Point3ui >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
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
				, MaxPipelines * sizeof( uint32_t )
				, getOwner()->getName() + "/MaterialsPipelinesIds" )
			: nullptr ) }
		, m_visibilityReorder{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute )
			? castor::makeUnique< VisibilityReorderPass >( m_graph
				, crg::FramePassArray{ &previous.getLastPass() }
				, m_device
				, previous.getVisibility().sampledViewId
				, *m_visibilityPipelinesIds.get()
				, * m_materialsCounts
				, * m_materialsIndirectCounts
				, *m_materialsStarts
				, *m_pixelsXY )
			: nullptr ) }
		, m_ssao{ doCreateSsaoPass( progress, previous.getLastPass(), previousPasses ) }
		, m_opaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled(); } } }
		, m_deferredOpaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsDeferredOpaquePassEnabled(); } } }
		, m_visibilityResolveDesc{ ( previous.hasVisibility()
			? &doCreateVisibilityResolve( progress, previous, { &m_ssao->getLastPass() }, false )
			: nullptr ) }
		, m_opaquePassDesc{ ( m_visibilityResolveDesc
			? m_visibilityResolveDesc
			: &doCreateOpaquePass( progress, previous.getLastPass(), previousPasses, false ) ) }
		, m_subsurfaceScattering{ castor::makeUnique< SubsurfaceScatteringPass >( m_graph
			, *m_opaquePassDesc
			, m_device
			, progress
			, *getOwner()->getRenderTarget().getScene()
			, getOwner()->getCameraUbo()
			, getOwner()->getDepthObj()
			, getOwner()->getDiffuse()
			, m_deferredOpaquePassEnabled ) }
		, m_deferredVisibilityResolveDesc{ ( m_visibilityResolveDesc
			? &doCreateVisibilityResolve( progress, previous, { &m_subsurfaceScattering->getLastPass() }, true )
			: nullptr ) }
		, m_deferredOpaquePassDesc{ ( m_deferredVisibilityResolveDesc
			? m_deferredVisibilityResolveDesc
			: &doCreateOpaquePass( progress, m_subsurfaceScattering->getLastPass(), previousPasses, true ) ) }
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
		if ( !m_opaquePass )
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

		if ( m_deferredOpaquePass )
		{
			m_deferredOpaquePass->update( updater );
		}
	}

	void OpaqueRendering::update( GpuUpdater & updater )
	{
		if ( !m_opaquePass )
		{
			return;
		}

		if ( m_opaquePass )
		{
			m_opaquePass->countNodes( updater.info );
		}

		if ( m_deferredOpaquePass )
		{
			m_deferredOpaquePass->countNodes( updater.info );
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

		if ( m_deferredOpaquePass
			&& m_deferredOpaquePass->areValidPassFlags( visitor.getFlags().components ) )
		{
			m_deferredOpaquePass->accept( visitor );
		}
	}

	Engine * OpaqueRendering::getEngine()const
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & OpaqueRendering::getLastPass()const
	{
		return *m_deferredOpaquePassDesc;
	}

	Texture const & OpaqueRendering::getSsaoResult()const
	{
		return m_ssao->getResult();
	}

	Texture const & OpaqueRendering::getSssDiffuse()const
	{
		return m_subsurfaceScattering->getResult();
	}

	bool OpaqueRendering::isEnabled()const
	{
		return m_opaquePassEnabled()
			|| m_deferredOpaquePassEnabled();
	}

	crg::FramePass & OpaqueRendering::doCreateVisibilityResolve( ProgressBar * progress
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses
		, bool isDeferredLighting )
	{
		if ( isDeferredLighting )
		{
			stepProgressBar( progress, "Creating deferred visibility resolve pass" );
		}
		else
		{
			stepProgressBar( progress, "Creating visibility resolve pass" );
		}

		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( isDeferredLighting ? std::string{ "DeferredVisibility" } : std::string{ "Visibility" }
			, [this, targetResult, targetDepth, progress, isDeferredLighting, &previous]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				if ( isDeferredLighting )
				{
					stepProgressBar( progress, "Initialising deferred visibility resolve pass" );
				}
				else
				{
					stepProgressBar( progress, "Initialising visibility resolve pass" );
				}

				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.deferredLightingMode( isDeferredLighting ? DeferredLightingMode::eDeferredOnly : DeferredLightingMode::eDeferLighting );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();
				auto resultIt = framePass.images.begin();
				auto diffuseIt = std::next( resultIt );
				renderPassDesc.implicitAction( diffuseIt->view(), crg::RecordContext::clearAttachment( *diffuseIt ) );

				auto res = std::make_unique< VisibilityResolvePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, ( isDeferredLighting
						? castor::String{ cuT( "DeferredResolve" ) }
						: castor::String{ cuT( "Resolve" ) } )
					, previous.getVisibilityPass()
					, targetResult
					, targetDepth
					, m_visibilityPipelinesIds.get()
					, std::move( renderPassDesc )
					, std::move( techniquePassDesc ) );

				if ( isDeferredLighting )
				{
					m_deferredOpaquePass = res.get();
				}
				else
				{
					m_opaquePass = res.get();
				}

				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		uint32_t index = 0u;
		result.addInputStorageView( previous.getVisibility().targetViewId
			, index++ );

		if ( isDeferredLighting )
		{
			result.addInputStorageView( getOwner()->getSssDiffuse().targetViewId
				, index++ );
		}
		else
		{
			result.addClearableOutputStorageView( getOwner()->getDiffuse().targetViewId
				, index++ );
		}

		if ( m_ssao )
		{
			result.addDependency( m_ssao->getLastPass() );
			result.addImplicitColourView( m_ssao->getResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		if constexpr ( VisibilityResolvePass::useCompute )
		{
			result.addDependency( m_visibilityReorder->getLastPass() );
			result.addInputStorageBuffer( { m_materialsCounts->getBuffer(), "MaterialsCounts" }
				, index++
				, 0u
				, uint32_t( m_materialsCounts->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_materialsStarts->getBuffer(), "MaterialsStarts" }
				, index++
				, 0u
				, uint32_t( m_materialsStarts->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_pixelsXY->getBuffer(), "PixelsXY" }
				, index++
				, 0u
				, uint32_t( m_pixelsXY->getBuffer().getSize() ) );
			result.addInOutStorageView( targetResult, index++ );

			if ( isDeferredLighting )
			{
				result.addInOutStorageView( getOwner()->getScattering().targetViewId, index++ );
			}
			else
			{
				result.addOutputStorageView( getOwner()->getScattering().targetViewId, index++ );
			}
		}
		else
		{
			result.addInOutColourView( targetResult );

			if ( isDeferredLighting )
			{
				result.addInOutColourView( getOwner()->getScattering().targetViewId );
			}
			else
			{
				result.addOutputColourView( getOwner()->getScattering().targetViewId );
			}
		}

		return result;
	}

	crg::FramePass & OpaqueRendering::doCreateOpaquePass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses
		, bool isDeferredLighting )
	{
		if ( isDeferredLighting )
		{
			stepProgressBar( progress, "Creating deferred opaque pass" );
		}
		else
		{
			stepProgressBar( progress, "Creating opaque pass" );
		}

		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( isDeferredLighting ? std::string{ "DeferredNodesPass" } : std::string{ "NodesPass" }
			, [this, targetResult, targetDepth, progress, isDeferredLighting]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				if ( isDeferredLighting )
				{
					stepProgressBar( progress, "Initialising deferred opaque pass" );
				}
				else
				{
					stepProgressBar( progress, "Initialising opaque pass" );
				}

				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					.deferredLightingMode( isDeferredLighting ? DeferredLightingMode::eDeferredOnly : DeferredLightingMode::eDeferLighting );
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();

				if ( !isDeferredLighting )
				{
					auto diffuseIt = framePass.images.rbegin();
					renderPassDesc.implicitAction( diffuseIt->view(), crg::RecordContext::clearAttachment( *diffuseIt ) );
				}

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

				if ( isDeferredLighting )
				{
					m_deferredOpaquePass = res.get();
				}
				else
				{
					m_opaquePass = res.get();
				}

				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addDependencies( previousPasses );

		if ( m_ssao )
		{
			result.addDependency( m_ssao->getLastPass() );
			result.addImplicitColourView( m_ssao->getResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		if ( isDeferredLighting )
		{
			result.addInputStorageView( getOwner()->getSssDiffuse().targetViewId, 0u );
		}

		result.addInOutDepthStencilView( targetDepth );
		result.addInOutColourView( targetResult );

		if ( isDeferredLighting )
		{
			result.addInOutColourView( getOwner()->getScattering().targetViewId );
		}
		else
		{
			result.addOutputColourView( getOwner()->getScattering().targetViewId );
			result.addOutputColourView( getOwner()->getDiffuse().targetViewId );
		}

		return result;
	}

	bool OpaqueRendering::doIsOpaquePassEnabled()const
	{
		CU_Require( m_opaquePass );
		return m_opaquePass && m_opaquePass->isPassEnabled();
	}

	bool OpaqueRendering::doIsDeferredOpaquePassEnabled()const
	{
		CU_Require( m_deferredOpaquePass );
		return m_deferredOpaquePass && m_deferredOpaquePass->isPassEnabled();
	}
}
