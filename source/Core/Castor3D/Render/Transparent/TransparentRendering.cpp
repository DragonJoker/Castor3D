#include "Castor3D/Render/Transparent/TransparentRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Render/Transparent/WeightedBlendRendering.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePassTimer.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

CU_ImplementSmartPtr( c3d, TransparentRendering )

namespace c3d
{
	TransparentRendering::TransparentRendering( RenderTechnique & parent
		, RenderDevice const & device
		, OpaqueRendering const & previous
		, ProgressBar * progress
		, bool weightedBlended )
		: OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Transparent" ) }
		, m_mippedColour{ m_device
			, getOwner()->getResources()
			, getOwner()->getName() + cuT( "/MippedColour" )
			, { ImageCreateFlags::eNone
				, makeExtent3D( getOwner()->getSize() ), 1u, EnvironmentMipLevels
				, PixelFormat::eR16G16B16A16_SFLOAT
				, ( ImageUsageFlags::eColorAttachment
					| ImageUsageFlags::eSampled ) }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_transparentPassResult{ ( weightedBlended
			? makeUnique< TransparentPassResult >( getOwner()->getResources()
				, m_device
				, makeSize( getOwner()->getTargetExtent() ) )
			: nullptr ) }
	{
		doCreateMipGenPass( progress );
		if ( weightedBlended )
		{
			doCreateWBTransparentPass( progress );
			m_weightedBlendRendering = makeUnique< WeightedBlendRendering >( m_graph
				, m_device
				, progress
				, m_enabled
				, getOwner()->getDepthObj()
				, *m_transparentPassResult
				, getOwner()->getTargetResult()
				, getOwner()->getSize()
				, getOwner()->getCameraUbo()
				, getOwner()->getSceneUbo()
				, getOwner()->getRenderTarget().getRenderUbo() );
		}
		else
		{
			doCreateForwardTransparentPass( progress );
		}

		if ( m_transparentPassResult )
		{
			m_transparentPassResult->create();
		}
	}

	TransparentRendering::~TransparentRendering()noexcept
	{
		m_mippedColour.destroy();
		m_weightedBlendRendering.reset();
		m_transparentPassResult.reset();
	}

	uint32_t TransparentRendering::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += 1;// colour copy pass
		result += 1;// mips generation pass
		result += 1;// accuumulation pass
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
		m_enabled = m_transparentPass->isPassEnabled();
		m_transparentPass->update( updater );
	}

	void TransparentRendering::update( GpuUpdater & updater )
	{
		if ( !m_transparentPass )
		{
			return;
		}

		m_transparentPass->countNodes( updater.info );
	}

	void TransparentRendering::accept( RenderTechniqueVisitor & visitor )
	{
		if ( m_transparentPass
			&& m_transparentPass->areValidPassFlags( visitor.getFlags().pass ) )
		{
			m_transparentPass->accept( visitor );

			if ( m_weightedBlendRendering )
			{
				m_weightedBlendRendering->accept( visitor );
			}
		}
	}

	Engine * TransparentRendering::getEngine()const noexcept
	{
		return getOwner()->getEngine();
	}

	void TransparentRendering::doCreateMipGenPass( ProgressBar * progress )
	{
		m_mippedColour.create();
		stepProgressBarLocal( progress, cuT( "Creating colour copy pass" ) );
		auto & copy = m_graph.createPass( "ColCopyPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising colour copy pass" ) );
				auto res = makeRawUnique< crg::ImageCopy >( framePass
					, context
					, runnableGraph
					, m_mippedColour.getExtent()
					, crg::ru::Config{ 1u }
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return m_enabled; } ) );
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		copy.addInputTransfer( *getOwner()->getTargetResult().getLastAttach() );
		auto mippedAttach = copy.addOutputTransferImage( m_mippedColour.getTargetViewId() );

		stepProgressBarLocal( progress, cuT( "Creating mips generation pass" ) );
		auto & result = m_graph.createPass( "MipsGenPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising mips generation pass" ) );
				auto res = makeRawUnique< crg::GenerateMipmaps >( framePass
					, context
					, runnableGraph
					, ImageLayout::eShaderReadOnly
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return m_enabled; } ) );
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		m_mippedColour.setLastAttach( result.addInOutTransfer( *mippedAttach ) );
	}

	void TransparentRendering::doCreateForwardTransparentPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating transparent pass" ) );
		auto & targetResult = getOwner()->getTargetResult();
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & pass = m_graph.createPass( "NodesPass"
			, [this, progress, &targetResult, &targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising transparent pass" ) );
				static constexpr bool isOit = false;
				static constexpr bool hasVelocity = false;
				auto res = makeRawUnique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, RenderNodesPassDesc{ getOwner()->getTargetExtent()
							, getOwner()->getCameraUbo()
							, getOwner()->getRenderUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller()
							, isOit }
						.safeBand( true )
						.meshShading( true )
						.allowClusteredLighting()
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.ssao( getOwner()->getSsaoResult() )
						.indirect( getOwner()->getIndirectLighting() )
						.hasVelocity( hasVelocity )
					, &m_mippedColour );
				m_transparentPass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addImplicit( *m_mippedColour.getLastAttach(), ImageLayout::eShaderReadOnly );
		pass.addInputDepthStencilTarget( *targetDepth.getLastAttach() );
		targetResult.setLastAttach( pass.addInOutColourTarget( *targetResult.getLastAttach() ) );

		if ( auto frustumClusters = getOwner()->getRenderTarget().getFrustumClusters();
			frustumClusters && getOwner()->getClustersConfig()->enabled )
		{
			pass.addImplicit( *frustumClusters->getReducedLightsAABBBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterGridBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterGridBuffer().getLastAttach(), AccessState{} );
		}
	}

	void TransparentRendering::doCreateWBTransparentPass( ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating transparent pass" ) );
		auto & targetResult = getOwner()->getTargetResult();
		auto & targetDepth = getOwner()->getTargetDepth();
		auto & targetDepthObj = getOwner()->getDepthObj();
		auto & targetNormal = getOwner()->getNormal();
		auto & pass = m_graph.createPass( "NodesPass"
			, [this, progress, &targetResult, &targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising transparent pass" ) );
				String name = cuT( "Accumulation" );
				static constexpr bool isOit = true;
				static constexpr bool hasVelocity = false;
				auto accumIt = framePass.targets.begin();
				auto revealIt = std::next( accumIt );
				auto res = makeRawUnique< TransparentPass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, m_mippedColour
					, getOwner()->getDepthObj()
					, getOwner()->getNormal()
					, targetResult
					, targetDepth
					, RenderNodesPassDesc{ getOwner()->getTargetExtent()
							, getOwner()->getCameraUbo()
							, getOwner()->getRenderUbo()
							, getOwner()->getSceneUbo()
							, getOwner()->getRenderTarget().getCuller()
							, isOit }
						.safeBand( true )
						.meshShading( true )
						.allowClusteredLighting()
						.componentModeFlags( ComponentModeFlag::eOpacity
							| ComponentModeFlag::eAlphaBlending
							| ComponentModeFlag::eColour
							| ComponentModeFlag::eDiffuseLighting
							| ComponentModeFlag::eSpecularLighting
							| ComponentModeFlag::eNormals
							| ComponentModeFlag::eGeometry
							| ComponentModeFlag::eOcclusion
							| ComponentModeFlag::eSpecifics )
						.implicitAction( ( *accumIt )->view(), crg::RecordContext::clearAttachment( **accumIt ) )
						.implicitAction( ( *revealIt )->view(), crg::RecordContext::clearAttachment( **revealIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.ssao( getOwner()->getSsaoResult() )
						.indirect( getOwner()->getIndirectLighting() )
						.clustersConfig( getOwner()->getClustersConfig() )
						.hasVelocity( hasVelocity ) );
				m_transparentPass = res.get();
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		pass.addInOutDepthStencilTarget( *targetDepth.getLastAttach() );
		pass.addImplicit( *m_mippedColour.getLastAttach(), ImageLayout::eShaderReadOnly );
		pass.addImplicit( *targetDepthObj.getLastAttach(), ImageLayout::eShaderReadOnly );
		pass.addImplicit( *targetNormal.getLastAttach(), ImageLayout::eShaderReadOnly );
		pass.addImplicit( *getOwner()->getSsaoResult().getLastAttach(), ImageLayout::eShaderReadOnly );

		if ( auto frustumClusters = getOwner()->getRenderTarget().getFrustumClusters();
			frustumClusters && getOwner()->getClustersConfig()->enabled )
		{
			pass.addImplicit( *frustumClusters->getReducedLightsAABBBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getPointLightClusterGridBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterIndexBuffer().getLastAttach(), AccessState{} );
			pass.addImplicit( *frustumClusters->getSpotLightClusterGridBuffer().getLastAttach(), AccessState{} );
		}

		auto & transparentPassResult = *m_transparentPassResult;
		transparentPassResult.setLastAttach( WbTexture::eAccumulation
			, pass.addOutputColourTarget( transparentPassResult.getTargetViewId( WbTexture::eAccumulation ), getClearValue( WbTexture::eAccumulation ).color() ) );
		transparentPassResult.setLastAttach( WbTexture::eRevealage
			, pass.addOutputColourTarget( transparentPassResult.getTargetViewId( WbTexture::eRevealage ), getClearValue( WbTexture::eRevealage ).color() ) );
	}
}
