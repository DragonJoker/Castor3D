#include "Castor3D/Render/Prepass/PrepassRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Prepass/DepthPass.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePass.hpp>
#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementCUSmartPtr( castor3d, PrepassRendering )

namespace castor3d
{
	//*************************************************************************************************

	PrepassRendering::PrepassRendering( RenderTechnique & parent
		, RenderDevice const & device
		, QueueData const & queueData
		, crg::FramePassArray const & previousPasses
		, ProgressBar * progress
		, bool deferred
		, bool visbuffer )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ parent.getRenderTarget().getGraph().createPassGroup( "Prepass" ) }
		, m_depthObj{ std::make_shared< Texture >( m_device
			, getEngine()->getGraphResourceHandler()
			, parent.getName() + "/DepthObj"
			, 0u
			, parent.getResult().getExtent()
			, 1u
			, 1u
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_STORAGE_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE ) }
		, m_visibility{ ( visbuffer && m_device.hasBindless()
			? std::make_shared< Texture >( m_device
				, getEngine()->getGraphResourceHandler()
				, parent.getName() + "/Visibility"
				, 0u
				, parent.getResult().getExtent()
				, 1u
				, 1u
				, VK_FORMAT_R32G32_UINT
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_STORAGE_BIT )
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK )
			: nullptr ) }
		, m_visibilityPassDesc{ ( hasVisibility()
			? &doCreateVisibilityPass( progress, previousPasses )
			: nullptr ) }
		, m_depthPassDesc{ hasVisibility()
			? m_visibilityPassDesc
			: ( deferred
				? &doCreateDeferredDepthPass( progress, previousPasses )
				: &doCreateForwardDepthPass( progress, previousPasses ) ) }
		, m_depthRange{ makeBuffer< int32_t >( m_device
			, 2u
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, parent.getName() + "/DepthRange" ) }
		, m_computeDepthRangeDesc{ &doCreateComputeDepthRange( progress ) }
	{
		m_depthObj->create();
		m_visibility->create();
	}

	PrepassRendering::~PrepassRendering()
	{
		if ( hasVisibility() )
		{
			m_visibility->destroy();
		}

		m_depthObj->destroy();
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
		if ( !m_depthPass && !m_visibilityPass )
		{
			return;
		}

		if ( hasVisibility() )
		{
			countNodes( *m_visibilityPass, updater.info );
		}
		else
		{
			countNodes( *m_depthPass, updater.info );
		}
	}

	void PrepassRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Technique DepthObj"
			, *m_depthObj
			, m_graph.getFinalLayoutState( m_depthObj->sampledViewId ).layout
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

	crg::FramePass & PrepassRendering::doCreateVisibilityPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto dataIt = std::next( depthObjIt );
				auto velocityIt = std::next( dataIt );
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< VisibilityPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, RenderNodesPassDesc{ getOwner()->getDepth().getExtent()
							, getOwner()->getMatrixUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( dataIt->view(), crg::RecordContext::clearAttachment( *dataIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.hasVelocity( true ) );
				m_visibilityPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( getOwner()->getDepth().targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_depthObj->targetViewId
			, makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ) );
		result.addOutputColourView( m_visibility->targetViewId
			, opaqueBlackClearColor );
		result.addOutputColourView( getOwner()->getRenderTarget().getVelocity()->targetViewId );
		return result;
	}

	crg::FramePass & PrepassRendering::doCreateForwardDepthPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = m_graph.createPass( "Depth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto velocityIt = std::next( depthObjIt );
				auto normalIt = std::next( velocityIt );
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< DepthPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, getOwner()->getSsaoConfig()
					, RenderNodesPassDesc{ getOwner()->getDepth().getExtent()
							, getOwner()->getMatrixUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *normalIt ) )
					, false );
				m_depthPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( getOwner()->getDepth().targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_depthObj->targetViewId
			, makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ) );
		result.addOutputColourView( getOwner()->getRenderTarget().getVelocity()->targetViewId );
		result.addOutputColourView( getOwner()->getNormal().targetViewId
			, getClearValue( DsTexture::eNmlOcc ) );
		return result;
	}

	crg::FramePass & PrepassRendering::doCreateDeferredDepthPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = m_graph.createPass( "Depth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto velocityIt = std::next( depthObjIt );
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< DepthPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, getOwner()->getSsaoConfig()
					, RenderNodesPassDesc{ getOwner()->getDepth().getExtent()
							, getOwner()->getMatrixUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
					, true );
				m_depthPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( getOwner()->getDepth().targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_depthObj->targetViewId
			, makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ) );
		result.addOutputColourView( getOwner()->getRenderTarget().getVelocity()->targetViewId );
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
		result.addInputStorageView( m_depthObj->sampledViewId
			, ComputeDepthRange::eInput );
		result.addOutputStorageBuffer( { m_depthRange->getBuffer(), "DepthRange" }
			, ComputeDepthRange::eOutput
			, 0u
			, m_depthRange->getBuffer().getSize() );
		return result;
	}
}
