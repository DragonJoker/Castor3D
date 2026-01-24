#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Opaque/SubsurfaceScatteringPass.hpp"
#include "Castor3D/Render/Opaque/VisibilityReorderPass.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Prepass/PrepassRendering.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"
#include "Castor3D/Render/Ssao/SsaoPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, OpaqueRendering )

namespace c3d
{
	OpaqueRendering::OpaqueRendering( RenderTechnique & parent
		, RenderDevice const & device
		, PrepassRendering const & previous
		, ProgressBar * progress )
		: OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Opaque" ) }
		, m_materialsCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBufferBase( m_device, parent.getResources()
				, getEngine()->getMaxPassTypeCount() * sizeof( uint32_t )
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eIndirectBuffer
				, MemoryPropertyFlags::eDeviceLocal
				, getOwner()->getName() + cuT( "/MaterialsCounts1" ) )
			: nullptr ) }
		, m_materialsIndirectCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBufferBase( m_device, parent.getResources()
				, getEngine()->getMaxPassTypeCount() * sizeof( Point3ui )
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eIndirectBuffer
				, MemoryPropertyFlags::eDeviceLocal
				, getOwner()->getName() + cuT( "/MaterialsCounts2" ) )
			: nullptr ) }
		, m_materialsStarts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBufferBase( m_device, parent.getResources()
				, getEngine()->getMaxPassTypeCount() * sizeof( uint32_t )
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eDeviceLocal
				, getOwner()->getName() + cuT( "/MaterialsStarts" ) )
			: nullptr ) }
		, m_pixelsXY{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBufferBase( m_device, parent.getResources()
				, getOwner()->getTargetExtent().width * getOwner()->getTargetExtent().height * sizeof( Point2ui )
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eDeviceLocal
				, getOwner()->getName() + cuT( "/PixelsXY" ) )
			: nullptr ) }
		, m_opaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled(); } } }
		, m_deferredOpaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsDeferredOpaquePassEnabled(); } } }
		, m_visibilityOpaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsVisibilityOpaquePassEnabled(); } } }
	{
		m_ssao = doCreateSsaoPass( progress );

		if ( previous.hasVisibility() )
		{
			if ( VisibilityResolvePass::useCompute() )
			{
				m_visibilityReorder = makeUnique< VisibilityReorderPass >( m_graph
					, m_device
					, previous.getVisibility()
					, *m_materialsCounts
					, *m_materialsIndirectCounts
					, *m_materialsStarts
					, *m_pixelsXY
					, crg::RunnablePass::IsEnabledCallback{ [this](){ return doIsOpaquePassEnabled() || doIsVisibilityOpaquePassEnabled(); } } );
			}
			doCreateVisibilityResolve( progress, previous, false );
		}
		else
		{
			doCreateOpaquePass( progress, false );
		}

		m_subsurfaceScattering = makeUnique< SubsurfaceScatteringPass >( m_graph
			, m_device
			, progress
			, *getOwner()->getRenderTarget().getScene()
			, getOwner()->getCameraUbo()
			, getOwner()->getDepthObj()
			, getOwner()->getDiffuse()
			, m_deferredOpaquePassEnabled );

		if ( previous.hasVisibility() )
		{
			doCreateVisibilityResolve( progress, previous, true );
			doCreateVisibilityOpaquePass( progress );
		}
		else
		{
			doCreateOpaquePass( progress, true );
		}

		m_graph.addGroupOutput( getOwner()->getTargetResult().getTargetViewId() );
	}

	OpaqueRendering::~OpaqueRendering()noexcept
	{
		if ( m_pixelsXY )
		{
			m_pixelsXY->destroy();
			m_pixelsXY.reset();
		}

		if ( m_materialsStarts )
		{
			m_materialsStarts->destroy();
			m_materialsStarts.reset();
		}

		if ( m_materialsIndirectCounts )
		{
			m_materialsIndirectCounts->destroy();
			m_materialsIndirectCounts.reset();
		}

		if ( m_materialsCounts )
		{
			m_materialsCounts->destroy();
			m_materialsCounts.reset();
		}
	}

	SsaoPassUPtr OpaqueRendering::doCreateSsaoPass( ProgressBar * progress )const
	{
		return makeUnique< SsaoPass >( m_graph
			, m_device
			, progress
			, makeSize( getOwner()->getTargetExtent() )
			, getOwner()->getSsaoConfig()
			, getOwner()->getDepthObj()
			, getOwner()->getNormal()
			, getOwner()->getCameraUbo() );
	}

	uint32_t OpaqueRendering::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += SsaoPass::countInitialisationSteps();
		result += 1;// visibility pass
		return result;
	}

	void OpaqueRendering::update( CpuUpdater & updater )
	{
		if ( !m_opaquePass )
			return;

		if ( getOwner()->getSsaoConfig().enabled )
			m_ssao->update( updater );

		auto & scene = *updater.scene;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( m_opaquePass )
			m_opaquePass->update( updater );

		if ( m_deferredOpaquePass )
			m_deferredOpaquePass->update( updater );

		if ( m_visibilityOpaquePass )
			m_visibilityOpaquePass->update( updater );
	}

	void OpaqueRendering::update( GpuUpdater & updater )const
	{
		if ( !m_opaquePass )
			return;

		if ( m_opaquePass )
			m_opaquePass->countNodes( updater.info );

		if ( m_deferredOpaquePass )
			m_deferredOpaquePass->countNodes( updater.info );

		if ( m_visibilityOpaquePass )
			m_visibilityOpaquePass->countNodes( updater.info );
	}

	void OpaqueRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_ssao->accept( visitor );

		if ( m_opaquePass
			&& m_opaquePass->areValidPassFlags( visitor.getFlags().pass ) )
			m_opaquePass->accept( visitor );

		if ( m_deferredOpaquePass
			&& m_deferredOpaquePass->areValidPassFlags( visitor.getFlags().pass ) )
			m_deferredOpaquePass->accept( visitor );

		if ( m_visibilityOpaquePass
			&& m_visibilityOpaquePass->areValidPassFlags( visitor.getFlags().pass ) )
			m_visibilityOpaquePass->accept( visitor );
	}

	Engine * OpaqueRendering::getEngine()const noexcept
	{
		return getOwner()->getEngine();
	}

	Texture const & OpaqueRendering::getSsaoResult()const noexcept
	{
		return m_ssao->getResult();
	}

	Texture const & OpaqueRendering::getSssDiffuse()const noexcept
	{
		return m_subsurfaceScattering->getResult();
	}

	bool OpaqueRendering::isEnabled()const noexcept
	{
		return m_opaquePassEnabled()
			|| m_deferredOpaquePassEnabled()
			|| m_visibilityOpaquePassEnabled();
	}

	void OpaqueRendering::doCreateVisibilityResolve( ProgressBar * progress
		, PrepassRendering const & previous
		, bool isDeferredLighting )
	{
		if ( isDeferredLighting )
		{
			stepProgressBarLocal( progress, cuT( "Creating deferred visibility resolve pass" ) );
		}
		else
		{
			stepProgressBarLocal( progress, cuT( "Creating visibility resolve pass" ) );
		}

		auto & targetResult = getOwner()->getTargetResult();
		auto & pass = m_graph.createPass( isDeferredLighting ? MbString{ "DeferredVisibilityResolve" } : MbString{ "VisibilityResolve" }
			, [this, &targetResult, progress, isDeferredLighting, &previous]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				if ( isDeferredLighting )
				{
					stepProgressBarLocal( progress, cuT( "Initialising deferred visibility resolve pass" ) );
				}
				else
				{
					stepProgressBarLocal( progress, cuT( "Initialising visibility resolve pass" ) );
				}

				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getRenderUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.parallaxOcclusionFilter( ParallaxOcclusionFilter::eDisabled )
					.deferredLightingFilter( isDeferredLighting ? DeferredLightingFilter::eDeferredOnly : DeferredLightingFilter::eDeferLighting );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();
				if ( !isDeferredLighting )
				{
					auto diffuse = framePass.getOutputs().find( 1u )->second;
					renderPassDesc.implicitAction( diffuse->view(), crg::RecordContext::clearAttachment( *diffuse ) );
				}

				auto res = makeRawUnique< VisibilityResolvePass >( *getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, ( isDeferredLighting
						? String{ cuT( "DeferredResolve" ) }
						: String{ cuT( "Resolve" ) } )
					, previous.getVisibilityPass()
					, targetResult
					, c3d::move( renderPassDesc )
					, c3d::move( techniquePassDesc ) );
				if ( isDeferredLighting )
					m_deferredOpaquePass = res.get();
				else
					m_opaquePass = res.get();

				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		uint32_t index = 0u;
		pass.addInputStorage( *previous.getVisibility().getLastAttach(), index );
		++index;

		if ( isDeferredLighting )
		{
			pass.addInputStorage( *getSssDiffuse().getLastAttach(), index );
			++index;
		}
		else
		{
			auto & diffuse = getOwner()->getDiffuse();
			diffuse.setLastAttach( pass.addClearableOutputStorageImage( diffuse.getTargetViewId(), index ) );
			++index;
		}

		pass.addImplicit( *getOwner()->getDiffusionProfiles().getLastAttach(), ImageLayout::eShaderReadOnly );

		if ( auto frustumClusters = getOwner()->getRenderTarget().getFrustumClusters();
			frustumClusters && getOwner()->getClustersConfig()->enabled )
		{
			pass.addImplicit( *frustumClusters->getReducedLightsAABBBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterGridBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterGridBuffer().getLastAttach(), AccessState{} );
		}

		if ( m_ssao )
			pass.addImplicit( *m_ssao->getResult().getLastAttach(), ImageLayout::eShaderReadOnly );

		auto & targetScattering = getOwner()->getScattering();
		if ( VisibilityResolvePass::useCompute() )
		{
			pass.addInputStorage( *m_materialsCounts->getLastAttach(), index );
			++index;
			pass.addInputStorage( *m_materialsStarts->getLastAttach(), index );
			++index;
			pass.addInputStorage( *m_pixelsXY->getLastAttach(), index );
			++index;
			targetResult.setLastAttach( pass.addInOutStorage( *targetResult.getLastAttach(), index ) );
			++index;
			targetScattering.setLastAttach( isDeferredLighting
				? pass.addInOutStorage( *targetScattering.getLastAttach(), index )
				: pass.addClearableOutputStorageImage( targetScattering.getTargetViewId(), index ) );
		}
		else
		{
			targetResult.setLastAttach( pass.addInOutColourTarget( *targetResult.getLastAttach() ) );
			targetScattering.setLastAttach( isDeferredLighting
				? pass.addInOutColourTarget( *targetScattering.getLastAttach() )
				: pass.addOutputColourTarget( targetScattering.getTargetViewId() ) );
		}
	}

	void OpaqueRendering::doCreateVisibilityOpaquePass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating visibility opaque pass" ) );
		auto & targetResult = getOwner()->getTargetResult();
		auto & targetScattering = getOwner()->getScattering();
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & pass = m_graph.createPass( "PostVisibilityNodesPass"
			, [this, &targetResult, &targetDepth, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising visibility opaque pass" ) );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getRenderUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.parallaxOcclusionFilter( ParallaxOcclusionFilter::eEnabled )
					.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags );
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();
				auto res = makeRawUnique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, c3d::move( renderPassDesc )
					, c3d::move( techniquePassDesc ) );
				m_visibilityOpaquePass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );

		pass.addInputDepthStencilTarget( *targetDepth.getLastAttach() );
		targetResult.setLastAttach( pass.addInOutColourTarget( *targetResult.getLastAttach() ) );
		targetScattering.setLastAttach( pass.addInOutColourTarget( *targetScattering.getLastAttach() ) );

		if ( auto frustumClusters = getOwner()->getRenderTarget().getFrustumClusters();
			frustumClusters && getOwner()->getClustersConfig()->enabled )
		{
			pass.addImplicit( *frustumClusters->getReducedLightsAABBBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterGridBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterGridBuffer().getLastAttach(), AccessState{} );
		}

		if ( m_ssao )
			pass.addImplicit( *m_ssao->getResult().getLastAttach(), ImageLayout::eShaderReadOnly );
	}

	void OpaqueRendering::doCreateOpaquePass( ProgressBar * progress
		, bool isDeferredLighting )
	{
		if ( isDeferredLighting )
		{
			stepProgressBarLocal( progress, cuT( "Creating deferred opaque pass" ) );
		}
		else
		{
			stepProgressBarLocal( progress, cuT( "Creating opaque pass" ) );
		}

		auto & targetResult = getOwner()->getTargetResult();
		auto & targetScattering = getOwner()->getScattering();
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & pass = m_graph.createPass( isDeferredLighting ? MbString{ "DeferredNodesPass" } : MbString{ "NodesPass" }
			, [this, &targetResult, &targetDepth, progress, isDeferredLighting]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				if ( isDeferredLighting )
				{
					stepProgressBarLocal( progress, cuT( "Initialising deferred opaque pass" ) );
				}
				else
				{
					stepProgressBarLocal( progress, cuT( "Initialising opaque pass" ) );
				}

				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
					, getOwner()->getRenderUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					.deferredLightingFilter( isDeferredLighting ? DeferredLightingFilter::eDeferredOnly : DeferredLightingFilter::eDeferLighting );
				techniquePassDesc.ssao( m_ssao->getResult() )
					.indirect( getOwner()->getIndirectLighting() )
					.clustersConfig( getOwner()->getClustersConfig() )
					.outputScattering();

				if ( !isDeferredLighting )
				{
					auto diffuse = *framePass.getTargets().rbegin();
					renderPassDesc.implicitAction( diffuse->view(), crg::RecordContext::clearAttachment( *diffuse ) );
				}

				auto res = makeRawUnique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, c3d::move( renderPassDesc )
					, c3d::move( techniquePassDesc ) );
				if ( isDeferredLighting )
					m_deferredOpaquePass = res.get();
				else
					m_opaquePass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addImplicit( *getOwner()->getDiffusionProfiles().getLastAttach(), ImageLayout::eShaderReadOnly );

		if ( m_ssao )
			pass.addImplicit( *m_ssao->getResult().getLastAttach(), ImageLayout::eShaderReadOnly );

		if ( auto frustumClusters = getOwner()->getRenderTarget().getFrustumClusters();
			frustumClusters && getOwner()->getClustersConfig()->enabled )
		{
			pass.addImplicit( *frustumClusters->getReducedLightsAABBBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterGridBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterGridBuffer().getLastAttach(), AccessState{} );
		}

		if ( isDeferredLighting )
			pass.addInputStorage( *getSssDiffuse().getLastAttach(), 0u );
		pass.addInputDepthStencilTarget( *targetDepth.getLastAttach() );

		targetResult.setLastAttach( pass.addInOutColourTarget( *targetResult.getLastAttach() ) );

		if ( isDeferredLighting )
		{
			targetScattering.setLastAttach( pass.addInOutColourTarget( *targetScattering.getLastAttach() ) );
		}
		else
		{
			auto & diffuse = getOwner()->getDiffuse();
			targetScattering.setLastAttach( pass.addOutputColourTarget( getOwner()->getScattering().getTargetViewId() ) );
			diffuse.setLastAttach( pass.addOutputColourTarget( diffuse.getTargetViewId() ) );
		}
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

	bool OpaqueRendering::doIsVisibilityOpaquePassEnabled()const
	{
		return m_visibilityOpaquePass && m_visibilityOpaquePass->isPassEnabled();
	}
}
