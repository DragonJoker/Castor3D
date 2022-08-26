#include "Castor3D/Render/Transparent/TransparentRendering.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Render/Transparent/WeightedBlendRendering.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementCUSmartPtr( castor3d, TransparentRendering )

namespace castor3d
{
	TransparentRendering::TransparentRendering( RenderTechnique & parent
		, RenderDevice const & device
		, QueueData const & queueData
		, OpaqueRendering const & previous
		, crg::FramePassArray const & previousPasses
		, SsaoConfig const & ssaoConfig
		, ProgressBar * progress
		, bool weightedBlended )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getRenderTarget().getGraph().createPassGroup( "Transparent" ) }
		, m_transparentPassResult{ ( weightedBlended
			? castor::makeUnique< TransparentPassResult >( m_graph.getHandler()
				, m_device
				, makeSize( getOwner()->getDepth().getExtent() ) )
			: nullptr ) }
		, m_transparentPassDesc{ ( weightedBlended
			? &doCreateWBTransparentPass( progress
				, previous.getLastPass()
				, previousPasses )
			: &doCreateForwardTransparentPass( progress
				, previous.getLastPass()
				, previousPasses ) ) }
		, m_weightedBlendRendering{ ( weightedBlended
			? castor::makeUnique< WeightedBlendRendering >( m_graph
				, m_device
				, progress
				, *m_transparentPassDesc
				, getOwner()->getDepth()
				, *m_transparentPassResult
				, getOwner()->getResult().wholeViewId
				, getOwner()->getSize()
				, getOwner()->getSceneUbo()
				, getOwner()->getRenderTarget().getHdrConfigUbo()
				, getOwner()->getGpInfoUbo() )
			: nullptr ) }
	{
		if ( m_transparentPassResult )
		{
			m_transparentPassResult->create();
		}
	}

	TransparentRendering::~TransparentRendering()
	{
		m_weightedBlendRendering.reset();
		m_transparentPassResult.reset();
	}

	uint32_t TransparentRendering::countInitialisationSteps()
	{
		uint32_t result = 0u;
		result += 2;// m_transparentPass;
		result += WeightedBlendRendering::countInitialisationSteps();
		return result;
	}

	void TransparentRendering::update( CpuUpdater & updater )
	{
		if ( !m_transparentPass )
		{
			return;
		}

		auto & scene = *updater.scene;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( m_weightedBlendRendering )
		{
			static_cast< TransparentPass & >( *m_transparentPass ).update( updater );
			m_weightedBlendRendering->enable( m_transparentPass->hasNodes() );
		}
		else
		{
			static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).update( updater );
		}
	}

	void TransparentRendering::update( GpuUpdater & updater )
	{
		if ( !m_transparentPass )
		{
			return;
		}

		countNodes( *m_transparentPass, updater.info );
	}

	void TransparentRendering::accept( RenderTechniqueVisitor & visitor )
	{
		if ( m_transparentPass
			&& m_transparentPass->areValidPassFlags( visitor.getFlags().m_passFlags ) )
		{
			m_transparentPass->accept( visitor );

			if ( m_weightedBlendRendering )
			{
				m_weightedBlendRendering->accept( visitor );
			}
		}
	}

	Engine * TransparentRendering::getEngine()const
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & TransparentRendering::getLastPass()const
	{
		return m_weightedBlendRendering
			? m_weightedBlendRendering->getLastPass()
			: *m_transparentPassDesc;
	}

	crg::FramePass & TransparentRendering::doCreateForwardTransparentPass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating transparent pass" );
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising transparent pass" );
				castor::String name = cuT( "Forward" );
				static constexpr bool isOit = false;
				static constexpr bool hasVelocity = false;
				auto res = std::make_unique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, getOwner()->getResult().imageId.data
					, RenderNodesPassDesc{ getOwner()->getResult().getExtent()
							, getOwner()->getMatrixUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller()
							, isOit }
						.safeBand( true )
						.meshShading( true )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.ssao( getOwner()->getSsaoResult() )
						.lpvConfigUbo( getOwner()->getLpvConfigUbo() )
						.llpvConfigUbo( getOwner()->getLlpvConfigUbo() )
						.vctConfigUbo( getOwner()->getVctConfigUbo() )
						.lpvResult( getOwner()->getLpvResult() )
						.llpvResult( getOwner()->getLlpvResult() )
						.vctFirstBounce( getOwner()->getFirstVctBounce() )
						.vctSecondaryBounce( getOwner()->getSecondaryVctBounce() )
						.hasVelocity( hasVelocity ) );
				m_transparentPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addDependencies( previousPasses );
		result.addInOutDepthStencilView( getOwner()->getDepth().targetViewId );
		result.addInOutColourView( getOwner()->getResult().targetViewId );

		return result;
	}

	crg::FramePass & TransparentRendering::doCreateWBTransparentPass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating transparent pass" );
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising transparent pass" );
				castor::String name = cuT( "Accumulation" );
				static constexpr bool isOit = true;
				static constexpr bool hasVelocity = true;
				auto accumIt = std::next( framePass.images.begin() );
				auto revealIt = std::next( accumIt );
				auto res = std::make_unique< TransparentPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, getOwner()->getResult().imageId.data
					, RenderNodesPassDesc{ getOwner()->getResult().getExtent()
							, getOwner()->getMatrixUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller()
							, isOit }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( accumIt->view(), crg::RecordContext::clearAttachment( *accumIt ) )
						.implicitAction( revealIt->view(), crg::RecordContext::clearAttachment( *revealIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.ssao( getOwner()->getSsaoResult() )
						.lpvConfigUbo( getOwner()->getLpvConfigUbo() )
						.llpvConfigUbo( getOwner()->getLlpvConfigUbo() )
						.vctConfigUbo( getOwner()->getVctConfigUbo() )
						.lpvResult( getOwner()->getLpvResult() )
						.llpvResult( getOwner()->getLlpvResult() )
						.vctFirstBounce( getOwner()->getFirstVctBounce() )
						.vctSecondaryBounce( getOwner()->getSecondaryVctBounce() )
						.hasVelocity( hasVelocity ) );
				m_transparentPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addDependencies( previousPasses );
		result.addInOutDepthStencilView( getOwner()->getDepth().targetViewId );
		auto & transparentPassResult = *m_transparentPassResult;
		result.addOutputColourView( transparentPassResult[WbTexture::eAccumulation].targetViewId
			, getClearValue( WbTexture::eAccumulation ) );
		result.addOutputColourView( transparentPassResult[WbTexture::eRevealage].targetViewId
			, getClearValue( WbTexture::eRevealage ) );
		result.addInOutColourView( getOwner()->getRenderTarget().getVelocity()->targetViewId );

		return result;
	}
}
