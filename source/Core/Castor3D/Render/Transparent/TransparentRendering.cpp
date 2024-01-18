#include "Castor3D/Render/Transparent/TransparentRendering.hpp"

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
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

CU_ImplementSmartPtr( castor3d, TransparentRendering )

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
		, m_graph{ getOwner()->getGraph().createPassGroup( "Transparent" ) }
		, m_mippedColour{ m_device
			, getOwner()->getResources()
			, getOwner()->getName() + "/MippedColour"
			, 0u
			, makeExtent3D( getOwner()->getSize() )
			, 1u
			, EnvironmentMipLevels
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_transparentPassResult{ ( weightedBlended
			? castor::makeUnique< TransparentPassResult >( getOwner()->getResources()
				, m_device
				, makeSize( getOwner()->getTargetExtent() ) )
			: nullptr ) }
		, m_mipgenPassDesc{ &doCreateMipGenPass( progress
			, previous.getLastPass()
			, std::move( previousPasses ) ) }
		, m_transparentPassDesc{ ( weightedBlended
			? &doCreateWBTransparentPass( progress
				, *m_mipgenPassDesc )
			: &doCreateForwardTransparentPass( progress
				, *m_mipgenPassDesc ) ) }
		, m_weightedBlendRendering{ ( weightedBlended
			? castor::makeUnique< WeightedBlendRendering >( m_graph
				, m_device
				, progress
				, m_enabled
				, *m_transparentPassDesc
				, getOwner()->getDepthObj()
				, *m_transparentPassResult
				, getOwner()->getTargetResult()
				, getOwner()->getSize()
				, getOwner()->getCameraUbo()
				, getOwner()->getSceneUbo()
				, getOwner()->getRenderTarget().getHdrConfigUbo() )
			: nullptr ) }
	{
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
		result += 1;// colour copy pass;
		result += 1;// mips generation pass;
		result += 1;// accuumulation pass;
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

	crg::FramePass const & TransparentRendering::getLastPass()const noexcept
	{
		return m_weightedBlendRendering
			? m_weightedBlendRendering->getLastPass()
			: *m_transparentPassDesc;
	}

	crg::FramePass & TransparentRendering::doCreateMipGenPass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray previousPasses )
	{
		if ( previousPasses.empty() )
		{
			previousPasses.push_back( &lastPass );
		}

		m_mippedColour.create();
		stepProgressBarLocal( progress, "Creating colour copy pass" );
		auto & copy = m_graph.createPass( "ColCopyPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising colour copy pass" );
				auto res = std::make_unique< crg::ImageCopy >( framePass
					, context
					, runnableGraph
					, m_mippedColour.getExtent()
					, crg::ru::Config{ 1u }
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return m_enabled; } ) );
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		copy.addDependencies( previousPasses );
		copy.addTransferInputView( getOwner()->getTargetResult() );
		copy.addTransferOutputView( m_mippedColour.targetViewId );

		stepProgressBarLocal( progress, "Creating mips generation pass" );
		auto & result = m_graph.createPass( "MipsGenPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising mips generation pass" );
				auto res = std::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, runnableGraph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return m_enabled; } ) );
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( copy );
		result.addTransferInOutView( m_mippedColour.targetViewId );
		return result;
	}

	crg::FramePass & TransparentRendering::doCreateForwardTransparentPass( ProgressBar * progress
		, crg::FramePass const & lastPass )
	{
		stepProgressBarLocal( progress, "Creating transparent pass" );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress, targetResult, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising transparent pass" );
				static constexpr bool isOit = false;
				static constexpr bool hasVelocity = false;
				auto res = std::make_unique< ForwardRenderTechniquePass >( getOwner()
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
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addImplicitColourView( m_mippedColour.targetViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutDepthStencilView( targetDepth );
		result.addInOutColourView( targetResult );

		return result;
	}

	crg::FramePass & TransparentRendering::doCreateWBTransparentPass( ProgressBar * progress
		, crg::FramePass const & lastPass )
	{
		stepProgressBarLocal( progress, "Creating transparent pass" );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress, targetResult, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBarLocal( progress, "Initialising transparent pass" );
				castor::String name = cuT( "Accumulation" );
				static constexpr bool isOit = true;
				static constexpr bool hasVelocity = false;
				auto depthIt = framePass.images.begin();
				auto mippedSceneIt = std::next( depthIt );
				auto depthObjIt = std::next( mippedSceneIt );
				auto normalIt = std::next( depthObjIt );
				auto ssaoIt = std::next( normalIt );
				auto accumIt = std::next( ssaoIt );
				auto revealIt = std::next( accumIt );
				auto res = std::make_unique< TransparentPass >( getOwner()
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
						.implicitAction( accumIt->view(), crg::RecordContext::clearAttachment( *accumIt ) )
						.implicitAction( revealIt->view(), crg::RecordContext::clearAttachment( *revealIt ) )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() }
						.ssao( getOwner()->getSsaoResult() )
						.indirect( getOwner()->getIndirectLighting() )
						.clustersConfig( getOwner()->getClustersConfig() )
						.hasVelocity( hasVelocity ) );
				m_transparentPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addInOutDepthStencilView( targetDepth );
		result.addImplicitColourView( m_mippedColour.targetViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( getOwner()->getDepthObj().targetViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( getOwner()->getNormal().targetViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( getOwner()->getSsaoResult().wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		auto & transparentPassResult = *m_transparentPassResult;
		result.addOutputColourView( transparentPassResult[WbTexture::eAccumulation].targetViewId
			, getClearValue( WbTexture::eAccumulation ) );
		result.addOutputColourView( transparentPassResult[WbTexture::eRevealage].targetViewId
			, getClearValue( WbTexture::eRevealage ) );

		return result;
	}
}
