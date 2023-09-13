#include "Castor3D/Render/Prepass/PrepassRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Prepass/DepthPass.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePass.hpp>
#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementSmartPtr( castor3d, PrepassRendering )

namespace castor3d
{
	PrepassRendering::PrepassRendering( RenderTechnique & parent
		, RenderDevice const & device
		, QueueData const & queueData
		, crg::FramePassArray const & previousPasses
		, ProgressBar * progress
		, bool visbuffer )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ parent.getGraph().createPassGroup( "Prepass" ) }
		, m_result{ parent.getResources()
			, device
			, makeSize( parent.getTargetExtent() )
			, visbuffer && m_device.hasBindless() }
		, m_visibilityPassDesc{ ( hasVisibility()
			? &doCreateVisibilityPass( progress, previousPasses )
			: nullptr ) }
		, m_depthPassDesc{ hasVisibility()
			? m_visibilityPassDesc
			: &doCreateDepthPass( progress, previousPasses ) }
		, m_depthRange{ makeBuffer< int32_t >( m_device
			, 2u
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, parent.getName() + "/DepthRange" ) }
		, m_computeDepthRangeDesc{ &doCreateComputeDepthRange( progress ) }
	{
		m_result.create();
		m_graph.addGroupOutput( getOwner()->getTargetDepth().front() );
		m_graph.addGroupOutput( getOwner()->getTargetDepth().back() );
		m_graph.addGroupOutput( m_result[PpTexture::eDepthObj].wholeViewId );
		m_graph.addGroupOutput( m_result[PpTexture::eVisibility].wholeViewId );
	}

	PrepassRendering::~PrepassRendering()
	{
	}

	uint32_t PrepassRendering::countInitialisationSteps()
	{
		uint32_t result = 0u;
		result += 2;// m_depthPass;
		result += 2;// m_computeDepthRange;
		return result;
	}

	void PrepassRendering::update( CpuUpdater & updater )
	{
		if ( !m_depthPass && !m_visibilityPass )
		{
			return;
		}

		if ( hasVisibility() )
		{
			m_visibilityPass->update( updater );
		}
		else
		{
			m_depthPass->update( updater );
		}
	}

	void PrepassRendering::update( GpuUpdater & updater )
	{
	}

	void PrepassRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Technique DepthObj"
			, m_result[PpTexture::eDepthObj]
			, m_graph.getFinalLayoutState( m_result[PpTexture::eDepthObj].sampledViewId ).layout
			, TextureFactors{}.invert( true ) );

		if ( hasVisibility() )
		{
			m_visibilityPass->accept( visitor );
		}
		else
		{
			m_depthPass->accept( visitor );
		}
	}

	Engine * PrepassRendering::getEngine()const
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & PrepassRendering::getLastPass()const
	{
		if ( hasVisibility() )
		{
			return *m_visibilityPassDesc;
		}

		return *m_depthPassDesc;
	}

	crg::FramePass const & PrepassRendering::getDepthRangePass()const
	{
		return *m_computeDepthRangeDesc;
	}

	bool PrepassRendering::hasVisibility()const
	{
		return bool( m_result[PpTexture::eVisibility] );
	}

	crg::FramePass & PrepassRendering::doCreateVisibilityPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating depth/visibility pass" );
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "VisibilityPass"
			, [this, progress, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto dataIt = std::next( depthObjIt );
				auto velocityIt = std::next( dataIt );
				auto normalIt = std::next( velocityIt );
				stepProgressBar( progress, "Initialising depth/visibility pass" );
				auto res = std::make_unique< VisibilityPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, targetDepth
					, RenderNodesPassDesc{ getExtent( targetDepth.front() )
							, getOwner()->getCameraUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						// Normally, ( ComponentModeFlag::eOpacity | ComponentModeFlag::eHeight | ComponentModeFlag::eNormals ) would be enough,
						// but to have the pipeline ID order synchronization with visibility resolve,
						// allow the same flags.
						.componentModeFlags( VisibilityResolvePass::getComponentsMask() )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( dataIt->view(), crg::RecordContext::clearAttachment( *dataIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *normalIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.hasVelocity( true ) );
				m_visibilityPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( targetDepth
			, defaultClearDepthStencil );
		result.addOutputColourView( m_result[PpTexture::eDepthObj].targetViewId
			, getClearValue( PpTexture::eDepthObj ) );
		result.addOutputColourView( m_result[PpTexture::eVisibility].targetViewId
			, opaqueBlackClearColor );
		result.addOutputColourView( getOwner()->getRenderTarget().getVelocity().targetViewId );
		result.addOutputColourView( getOwner()->getNormal().targetViewId
			, transparentBlackClearColor );
		return result;
	}

	crg::FramePass & PrepassRendering::doCreateDepthPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating forward depth pass" );
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "Depth"
			, [this, progress, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto velocityIt = std::next( depthObjIt );
				auto normalIt = std::next( velocityIt );
				stepProgressBar( progress, "Initialising forward depth pass" );
				auto res = std::make_unique< DepthPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, targetDepth
					, getOwner()->getSsaoConfig()
					, RenderNodesPassDesc{ getExtent( targetDepth.front() )
							, getOwner()->getCameraUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						.componentModeFlags( ComponentModeFlag::eOpacity
							| ComponentModeFlag::eGeometry
							| ComponentModeFlag::eNormals
							| ComponentModeFlag::eOcclusion )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *normalIt ) ) );
				m_depthPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( targetDepth
			, defaultClearDepthStencil );
		result.addOutputColourView( m_result[PpTexture::eDepthObj].targetViewId
			, getClearValue( PpTexture::eDepthObj ) );
		result.addOutputColourView( getOwner()->getRenderTarget().getVelocity().targetViewId );
		result.addOutputColourView( getOwner()->getNormal().targetViewId
			, transparentBlackClearColor );
		return result;
	}

	crg::FramePass & PrepassRendering::doCreateComputeDepthRange( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating compute depth range pass" );
		auto & result = m_graph.createPass( "ComputeDepthRange"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising compute depth range pass" );
				auto res = std::make_unique< ComputeDepthRange >( framePass
					, context
					, runnableGraph
					, m_device
					, m_needsDepthRange );
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *m_depthPassDesc );
		result.addInputStorageView( m_result[PpTexture::eDepthObj].sampledViewId
			, ComputeDepthRange::eInput );
		result.addOutputStorageBuffer( { m_depthRange->getBuffer(), "DepthRange" }
			, ComputeDepthRange::eOutput
			, 0u
			, m_depthRange->getBuffer().getSize() );
		return result;
	}
}
