#include "Castor3D/Render/Prepass/PrepassRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Prepass/DepthPass.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( c3d, PrepassRendering )

namespace c3d
{
	PrepassRendering::PrepassRendering( RenderTechnique & parent
		, RenderDevice const & device
		, ProgressBar * progress
		, bool visbuffer )
		: OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ parent.getGraph().createPassGroup( "Prepass" ) }
		, m_result{ parent.getResources()
			, device
			, makeSize( parent.getTargetExtent() )
			, visbuffer && m_device.hasBindless() }
		, m_depthRangeBuffer{ m_device, parent.getResources()
			, parent.getName() + cuT( "/DepthRange" )
			, BufferCreateFlags::eNone
			, 2u * sizeof( int32_t )
			, BufferUsageFlags::eStorageBuffer }
	{
		if ( hasVisibility() )
			doCreateVisibilityPass( progress );
		else
			doCreateDepthPass( progress );
		doCreateComputeDepthRange( progress );

		m_result.create();
		m_graph.addGroupOutput( getOwner()->getTargetDepth().getWholeViewId() );
		m_graph.addGroupOutput( m_result.getWholeViewId( PpTexture::eDepthObj ) );

		if ( hasVisibility() )
		{
			m_graph.addGroupOutput( m_result.getWholeViewId( PpTexture::eVisibility ) );
		}
	}

	uint32_t PrepassRendering::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += 1;// depth pass
		result += 1;// compute depth range
		return result;
	}

	void PrepassRendering::update( CpuUpdater & updater )
	{
		if ( !m_depthPass && !m_visibilityPass )
		{
			return;
		}

		if ( m_visibilityPass )
		{
			m_visibilityPass->update( updater );
		}
		else if ( m_depthPass )
		{
			m_depthPass->update( updater );
		}
	}

	void PrepassRendering::update( GpuUpdater & updater )
	{
	}

	void PrepassRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Technique DepthObj" )
			, m_result.getTexture( PpTexture::eDepthObj )
			, m_graph.getFinalLayoutState( m_result.getSampledViewId( PpTexture::eDepthObj ) ).layout
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

	Engine * PrepassRendering::getEngine()const noexcept
	{
		return getOwner()->getEngine();
	}

	bool PrepassRendering::hasVisibility()const noexcept
	{
		return m_device.hasBindless()
			&& bool( m_result.getTexture( PpTexture::eVisibility ) );
	}

	void PrepassRendering::doCreateVisibilityPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating depth/visibility pass" ) );
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & targetVelocity = getOwner()->getVelocity();
		auto & targetNormal = getOwner()->getNormal();
		auto & pass = m_graph.createPass( "VisibilityPass"
			, [this, progress, &targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.getTargets().begin();
				auto depthObjIt = std::next( depthIt );
				auto dataIt = std::next( depthObjIt );
				auto velocityIt = std::next( dataIt );
				auto normalIt = std::next( velocityIt );
				stepProgressBarLocal( progress, cuT( "Initialising depth/visibility pass" ) );
				auto res = makeRawUnique< VisibilityPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, targetDepth
					, RenderNodesPassDesc{ targetDepth.getExtent()
							, getOwner()->getCameraUbo()
							, getOwner()->getRenderUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						// Normally, ( ComponentModeFlag::eOpacity | ComponentModeFlag::eHeight | ComponentModeFlag::eNormals ) would be enough,
						// but to have the pipeline ID order synchronization with visibility resolve,
						// allow the same flags.
						.componentModeFlags( VisibilityResolvePass::getComponentsMask() )
						.implicitAction( ( *depthIt )->view(), crg::RecordContext::clearAttachment( **depthIt ) )
						.implicitAction( ( *depthObjIt )->view(), crg::RecordContext::clearAttachment( **depthObjIt ) )
						.implicitAction( ( *dataIt )->view(), crg::RecordContext::clearAttachment( **dataIt ) )
						.implicitAction( ( *velocityIt )->view(), crg::RecordContext::clearAttachment( **velocityIt ) )
						.implicitAction( ( *velocityIt )->view(), crg::RecordContext::clearAttachment( **normalIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.hasVelocity( true ) );
				m_visibilityPass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addImplicit( getOwner()->getVertexTransform(), AccessState{} );
		targetDepth.setLastAttach( pass.addOutputDepthStencilTarget( targetDepth.getTargetViewId(), defaultClearDepthStencil ) );
		m_result.setLastAttach( PpTexture::eDepthObj
			, pass.addOutputColourTarget( m_result.getTargetViewId( PpTexture::eDepthObj ), getClearValue( PpTexture::eDepthObj ).color() ) );
		m_result.setLastAttach( PpTexture::eVisibility
			, pass.addOutputColourTarget( m_result.getTargetViewId( PpTexture::eVisibility ), opaqueBlackClearColor ) );
		targetVelocity.setLastAttach( pass.addOutputColourTarget( targetVelocity.getTargetViewId() ) );
		targetNormal.setLastAttach( pass.addOutputColourTarget( targetNormal.getTargetViewId(), transparentBlackClearColor ) );

		for ( auto const & [_, mesh] : getOwner()->getRenderTarget().getScene()->getMeshCache() )
			mesh->registerDependencies( pass );
	}

	void PrepassRendering::doCreateDepthPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating forward depth pass" ) );
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & targetVelocity = getOwner()->getVelocity();
		auto & targetNormal = getOwner()->getNormal();
		auto & pass = m_graph.createPass( "Depth"
			, [this, progress, &targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.getTargets().begin();
				auto depthObjIt = std::next( depthIt );
				auto velocityIt = std::next( depthObjIt );
				auto normalIt = std::next( velocityIt );
				stepProgressBarLocal( progress, cuT( "Initialising forward depth pass" ) );
				auto res = makeRawUnique< DepthPass >( *getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, targetDepth
					, getOwner()->getSsaoConfig()
					, RenderNodesPassDesc{ targetDepth.getExtent()
							, getOwner()->getCameraUbo()
							, getOwner()->getRenderUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller() }
						.safeBand( true )
						.meshShading( true )
						.componentModeFlags( ComponentModeFlag::eOpacity
							| ComponentModeFlag::eGeometry
							| ComponentModeFlag::eNormals
							| ComponentModeFlag::eOcclusion )
						.implicitAction( ( *depthIt )->view(), crg::RecordContext::clearAttachment( **depthIt ) )
						.implicitAction( ( *depthObjIt )->view(), crg::RecordContext::clearAttachment( **depthObjIt ) )
						.implicitAction( ( *velocityIt )->view(), crg::RecordContext::clearAttachment( **velocityIt ) )
						.implicitAction( ( *depthObjIt )->view(), crg::RecordContext::clearAttachment( **normalIt ) ) );
				m_depthPass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addImplicit( getOwner()->getVertexTransform(), AccessState{} );
		targetDepth.setLastAttach( pass.addOutputDepthStencilTarget( targetDepth.getTargetViewId(), defaultClearDepthStencil ) );
		m_result.setLastAttach( PpTexture::eDepthObj
			, pass.addOutputColourTarget( m_result.getTargetViewId( PpTexture::eDepthObj ), getClearValue( PpTexture::eDepthObj ).color() ) );
		targetVelocity.setLastAttach( pass.addOutputColourTarget( targetVelocity.getTargetViewId() ) );
		targetNormal.setLastAttach( pass.addOutputColourTarget( targetNormal.getTargetViewId(), transparentBlackClearColor ) );

		for ( auto const & [_, mesh] : getOwner()->getRenderTarget().getScene()->getMeshCache() )
			mesh->registerDependencies( pass );
	}

	void PrepassRendering::doCreateComputeDepthRange( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating compute depth range pass" ) );
		auto & pass = m_graph.createPass( "ComputeDepthRange"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising compute depth range pass" ) );
				auto res = makeRawUnique< ComputeDepthRange >( framePass
					, context
					, runnableGraph
					, m_device
					, m_needsDepthRange );
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addInputStorage( *m_result.getLastAttach( PpTexture::eDepthObj ), ComputeDepthRange::eInput );
		m_depthRangeBuffer.setLastAttach( pass.addOutputStorageBuffer( m_depthRangeBuffer.bufferViewId, ComputeDepthRange::eOutput ) );
	}
}
