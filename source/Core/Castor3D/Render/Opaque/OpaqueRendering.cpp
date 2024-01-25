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
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses
		, ProgressBar * progress )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Opaque" ) }
		, m_materialsCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + cuT( "/MaterialsCounts1" ) )
			: nullptr ) }
		, m_materialsIndirectCounts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBuffer< castor::Point3ui >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + cuT( "/MaterialsCounts2" ) )
			: nullptr ) }
		, m_materialsStarts{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBuffer< uint32_t >( m_device
				, getEngine()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + cuT( "/MaterialsStarts" ) )
			: nullptr ) }
		, m_pixelsXY{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? makeBuffer< castor::Point2ui >( m_device
				, getOwner()->getTargetExtent().width * getOwner()->getTargetExtent().height
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, getOwner()->getName() + cuT( "/PixelsXY" ) )
			: nullptr ) }
		, m_opaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled(); } } }
		, m_deferredOpaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsDeferredOpaquePassEnabled(); } } }
		, m_visibilityOpaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsVisibilityOpaquePassEnabled(); } } }
		, m_visibilityReorder{ ( ( previous.hasVisibility() && VisibilityResolvePass::useCompute() )
			? castor::makeUnique< VisibilityReorderPass >( m_graph
				, crg::FramePassArray{ &previous.getLastPass() }
				, m_device
				, previous.getVisibility().sampledViewId
				, * m_materialsCounts
				, * m_materialsIndirectCounts
				, *m_materialsStarts
				, *m_pixelsXY
				, crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled() || doIsVisibilityOpaquePassEnabled(); } })
			: nullptr ) }
		, m_ssao{ doCreateSsaoPass( progress, previous.getLastPass(), previousPasses ) }
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
		, m_visibilityOpaquePassDesc{ ( m_deferredVisibilityResolveDesc
			? &doCreateVisibilityOpaquePass( progress, *m_deferredVisibilityResolveDesc, previousPasses )
			: nullptr ) }
	{
		m_graph.addGroupOutput( getOwner()->getTargetResult().front() );
		m_graph.addGroupOutput( getOwner()->getTargetResult().back() );
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

		if ( m_visibilityOpaquePass )
		{
			m_visibilityOpaquePass->update( updater );
		}
	}

	void OpaqueRendering::update( GpuUpdater & updater )const
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

		if ( m_visibilityOpaquePass )
		{
			m_visibilityOpaquePass->countNodes( updater.info );
		}
	}

	void OpaqueRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_ssao->accept( visitor );

		if ( m_opaquePass
			&& m_opaquePass->areValidPassFlags( visitor.getFlags().pass ) )
		{
			m_opaquePass->accept( visitor );
		}

		if ( m_deferredOpaquePass
			&& m_deferredOpaquePass->areValidPassFlags( visitor.getFlags().pass ) )
		{
			m_deferredOpaquePass->accept( visitor );
		}

		if ( m_visibilityOpaquePass
			&& m_visibilityOpaquePass->areValidPassFlags( visitor.getFlags().pass ) )
		{
			m_visibilityOpaquePass->accept( visitor );
		}
	}

	Engine * OpaqueRendering::getEngine()const noexcept
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & OpaqueRendering::getLastPass()const noexcept
	{
		return m_visibilityOpaquePassDesc
			? *m_visibilityOpaquePassDesc
			: *m_deferredOpaquePassDesc;
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

	crg::FramePass & OpaqueRendering::doCreateVisibilityResolve( ProgressBar * progress
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses
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

		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( isDeferredLighting ? castor::MbString{ "DeferredVisibility" } : castor::MbString{ "Visibility" }
			, [this, targetResult, targetDepth, progress, isDeferredLighting, &previous]( crg::FramePass const & framePass
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
				auto resultIt = framePass.images.begin();
				auto diffuseIt = std::next( resultIt );
				renderPassDesc.implicitAction( diffuseIt->view(), crg::RecordContext::clearAttachment( *diffuseIt ) );

				auto res = castor::make_unique< VisibilityResolvePass >( getOwner()
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
					, castor::move( renderPassDesc )
					, castor::move( techniquePassDesc ) );

				if ( isDeferredLighting )
				{
					m_deferredOpaquePass = res.get();
				}
				else
				{
					m_opaquePass = res.get();
				}

				getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		uint32_t index = 0u;
		result.addInputStorageView( previous.getVisibility().targetViewId
			, index );
		++index;

		if ( isDeferredLighting )
		{
			result.addInputStorageView( getOwner()->getSssDiffuse().targetViewId
				, index );
			++index;
		}
		else
		{
			result.addClearableOutputStorageView( getOwner()->getDiffuse().targetViewId
				, index );
			++index;
		}

		if ( m_ssao )
		{
			result.addDependency( m_ssao->getLastPass() );
			result.addImplicitColourView( m_ssao->getResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		if ( VisibilityResolvePass::useCompute() )
		{
			result.addDependency( m_visibilityReorder->getLastPass() );
			result.addInputStorageBuffer( { m_materialsCounts->getBuffer(), "MaterialsCounts" }
				, index
				, 0u
				, uint32_t( m_materialsCounts->getBuffer().getSize() ) );
			++index;
			result.addInputStorageBuffer( { m_materialsStarts->getBuffer(), "MaterialsStarts" }
				, index
				, 0u
				, uint32_t( m_materialsStarts->getBuffer().getSize() ) );
			++index;
			result.addInputStorageBuffer( { m_pixelsXY->getBuffer(), "PixelsXY" }
				, index
				, 0u
				, uint32_t( m_pixelsXY->getBuffer().getSize() ) );
			++index;
			result.addInOutStorageView( targetResult, index );
			++index;

			if ( isDeferredLighting )
			{
				result.addInOutStorageView( getOwner()->getScattering().targetViewId, index );
			}
			else
			{
				result.addClearableOutputStorageView( getOwner()->getScattering().targetViewId, index );
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

	crg::FramePass & OpaqueRendering::doCreateVisibilityOpaquePass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBarLocal( progress, cuT( "Creating visibility opaque pass" ) );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "PostVisibilityNodesPass"
			, [this, targetResult, targetDepth, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising visibility opaque pass" ) );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getCameraUbo()
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
				auto res = castor::make_unique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, castor::move( renderPassDesc )
					, castor::move( techniquePassDesc ) );
				m_visibilityOpaquePass = res.get();
				getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
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

		result.addInOutDepthStencilView( targetDepth );
		result.addInOutColourView( targetResult );
		result.addInOutColourView( getOwner()->getScattering().targetViewId );

		return result;
	}

	crg::FramePass & OpaqueRendering::doCreateOpaquePass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses
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

		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( isDeferredLighting ? castor::MbString{ "DeferredNodesPass" } : castor::MbString{ "NodesPass" }
			, [this, targetResult, targetDepth, progress, isDeferredLighting]( crg::FramePass const & framePass
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
					auto diffuseIt = framePass.images.rbegin();
					renderPassDesc.implicitAction( diffuseIt->view(), crg::RecordContext::clearAttachment( *diffuseIt ) );
				}

				auto res = castor::make_unique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, castor::move( renderPassDesc )
					, castor::move( techniquePassDesc ) );

				if ( isDeferredLighting )
				{
					m_deferredOpaquePass = res.get();
				}
				else
				{
					m_opaquePass = res.get();
				}

				getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
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

	bool OpaqueRendering::doIsVisibilityOpaquePassEnabled()const
	{
		return m_visibilityOpaquePass && m_visibilityOpaquePass->isPassEnabled();
	}
}
