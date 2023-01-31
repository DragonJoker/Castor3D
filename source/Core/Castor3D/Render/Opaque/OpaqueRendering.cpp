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
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/DeferredRendering.hpp"
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

CU_ImplementCUSmartPtr( castor3d, OpaqueRendering )

namespace castor3d
{
	OpaqueRendering::OpaqueRendering( RenderTechnique & parent
		, RenderDevice const & device
		, QueueData const & queueData
		, PrepassRendering const & previous
		, crg::FramePassArray const & previousPasses
		, SsaoConfig const & ssaoConfig
		, ProgressBar * progress
		, TexturePtr normal
		, bool deferred )
		: castor::OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getRenderTarget().getGraph().createPassGroup( "Opaque" ) }
		, m_opaquePassResult{ ( deferred
			? castor::makeUnique< OpaquePassResult >( parent.getResources()
				, m_device
				, normal )
			: nullptr ) }
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
		, m_visibilityResolveDesc{ ( previous.hasVisibility()
			? &doCreateVisibilityResolve( progress, previous, previousPasses )
			: nullptr ) }
		, m_ssao{ ( deferred
			? ( m_visibilityResolveDesc
				? doCreateSsaoPass( progress, *m_visibilityResolveDesc, {} )
				: nullptr )
			: doCreateSsaoPass( progress, previous.getLastPass(), previousPasses ) ) }
		, m_opaquePassDesc{ ( m_visibilityResolveDesc
			? m_visibilityResolveDesc 
			: ( deferred
				? &doCreateDeferredOpaquePass( progress, previous.getLastPass(), previousPasses )
				: &doCreateForwardOpaquePass( progress, previous.getLastPass(), previousPasses ) ) ) }
		, m_opaquePassEnabled{ crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsOpaquePassEnabled(); } } }
		, m_deferredRendering{ ( deferred
			? castor::makeUnique< DeferredRendering >( m_graph
				, crg::FramePassArray{ m_opaquePassDesc }
				, m_ssao->getLastPass()
				, m_device
				, progress
				, m_device.renderSystem.getPrefilteredBrdfTexture()
				, getOwner()->getTargetDepth()
				, previous.getDepthObj()
				, *m_opaquePassResult
				, getOwner()->getTargetResult()
				, getOwner()->getDirectionalShadowPassResult()
				, getOwner()->getPointShadowPassResult()
				, getOwner()->getSpotShadowPassResult()
				, getOwner()->getLpvResult()
				, getOwner()->getLlpvResult()
				, getOwner()->getFirstVctBounce()
				, getOwner()->getSecondaryVctBounce()
				, m_ssao->getResult()
				, getOwner()->getSize()
				, *getOwner()
				, getOwner()->getSceneUbo()
				, getOwner()->getRenderTarget().getHdrConfigUbo()
				, getOwner()->getGpInfoUbo()
				, getOwner()->getLpvConfigUbo()
				, getOwner()->getLlpvConfigUbo()
				, getOwner()->getVctConfigUbo()
				, getOwner()->getSsaoConfig()
				, m_opaquePassEnabled )
			: nullptr ) }
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

		m_deferredRendering.reset();
		m_opaquePassResult.reset();
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
			, getOwner()->getGpInfoUbo() );
	}

	uint32_t OpaqueRendering::countInitialisationSteps()
	{
		uint32_t result = 0u;
		result += SsaoPass::countInitialisationSteps();
		result += 2;// m_opaquePass;
		result += DeferredRendering::countInitialisationSteps();
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
		auto & camera = *updater.camera;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( m_opaquePass )
		{
			m_opaquePass->update( updater );
		}

		if ( m_deferredRendering )
		{
			updater.camera = &camera;
			m_deferredRendering->update( updater );
		}
	}

	void OpaqueRendering::update( GpuUpdater & updater )
	{
		if ( !m_opaquePass && !m_visibilityResolve )
		{
			return;
		}

		if ( m_deferredRendering )
		{
			m_deferredRendering->update( updater );
		}

		if ( !m_visibilityResolveDesc )
		{
			//countNodes( *m_opaquePass, updater.info );
		}
	}

	void OpaqueRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_ssao->accept( visitor );

		if ( m_opaquePass
			&& m_opaquePass->areValidPassFlags( visitor.getFlags().components ) )
		{
			m_opaquePass->accept( visitor );

			if ( m_deferredRendering )
			{
				m_deferredRendering->accept( visitor );
			}
		}
	}

	Engine * OpaqueRendering::getEngine()const
	{
		return getOwner()->getEngine();
	}

	crg::FramePass const & OpaqueRendering::getLastPass()const
	{
		if ( m_deferredRendering )
		{
			return m_deferredRendering->getLastPass();
		}

		return *m_opaquePassDesc;
	}

	Texture const & OpaqueRendering::getSsaoResult()const
	{
		return m_ssao->getResult();
	}

	Texture const & OpaqueRendering::getDiffuseLightingResult()const
	{
		if ( m_deferredRendering )
		{
			return m_deferredRendering->getLightDiffuse();
		}

		return getOwner()->getResult();
	}

	Texture const & OpaqueRendering::getScatteringLightingResult()const
	{
		if ( m_deferredRendering )
		{
			return m_deferredRendering->getLightScattering();
		}

		return getOwner()->getResult();
	}

	Texture const & OpaqueRendering::getBaseColourResult()const
	{
		if ( m_opaquePassResult )
		{
			m_opaquePassResult->create();
			return ( *m_opaquePassResult )[DsTexture::eColMtl];
		}

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
		auto & result = m_graph.createPass( "Visibility"
			, [this, progress, &previous]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising visibility resolve pass" );
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getMatrixUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true );

				if ( !VisibilityResolvePass::useCompute )
				{
					auto dataIt = framePass.images.begin();
					auto nmlOccIt = std::next( dataIt );
					auto colRghIt = std::next( nmlOccIt );
					auto spcMtlIt = std::next( colRghIt );
					auto emsTrnIt = std::next( spcMtlIt );
					auto clrCotIt = std::next( emsTrnIt );
					auto crTsIrIt = std::next( clrCotIt );
					auto sheenIt = std::next( crTsIrIt );
					renderPassDesc.implicitAction( nmlOccIt->view(), crg::RecordContext::clearAttachment( *nmlOccIt ) )
						.implicitAction( colRghIt->view(), crg::RecordContext::clearAttachment( *colRghIt ) )
						.implicitAction( spcMtlIt->view(), crg::RecordContext::clearAttachment( *spcMtlIt ) )
						.implicitAction( emsTrnIt->view(), crg::RecordContext::clearAttachment( *emsTrnIt ) )
						.implicitAction( clrCotIt->view(), crg::RecordContext::clearAttachment( *clrCotIt ) )
						.implicitAction( crTsIrIt->view(), crg::RecordContext::clearAttachment( *crTsIrIt ) )
						.implicitAction( sheenIt->view(), crg::RecordContext::clearAttachment( *sheenIt ) );
				}

				auto res = std::make_unique< VisibilityResolvePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, cuT( "Resolve" )
					, previous.getVisibilityPass()
					, m_visibilityPipelinesIds.get()
					, std::move( renderPassDesc )
					, RenderTechniquePassDesc{ false, getOwner()->getSsaoConfig() } );
				m_opaquePass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		uint32_t index = 0u;
		result.addInputStorageView( previous.getVisibility().targetViewId
			, index++ );

		auto & opaquePassResult = *m_opaquePassResult;
		opaquePassResult.create();

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
			result.addOutputStorageView( opaquePassResult[DsTexture::eNmlOcc].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eColMtl].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eSpcRgh].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eEmsTrn].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eClrCot].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eCrTsIr].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eSheen].targetViewId
				, index++ );
		}
		else
		{
			result.addOutputColourView( opaquePassResult[DsTexture::eNmlOcc].targetViewId
				, getClearValue( DsTexture::eNmlOcc ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eColMtl].targetViewId
				, getClearValue( DsTexture::eColMtl ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eSpcRgh].targetViewId
				, getClearValue( DsTexture::eSpcRgh ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eEmsTrn].targetViewId
				, getClearValue( DsTexture::eEmsTrn ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eClrCot].targetViewId
				, getClearValue( DsTexture::eClrCot ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eCrTsIr].targetViewId
				, getClearValue( DsTexture::eCrTsIr ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eSheen].targetViewId
				, getClearValue( DsTexture::eSheen ) );
		}

		return result;
	}

	crg::FramePass & OpaqueRendering::doCreateForwardOpaquePass( ProgressBar * progress
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
					, getOwner()->getMatrixUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true );
				techniquePassDesc.ssao( m_ssao->getResult() )
					.lpvConfigUbo( getOwner()->getLpvConfigUbo() )
					.llpvConfigUbo( getOwner()->getLlpvConfigUbo() )
					.vctConfigUbo( getOwner()->getVctConfigUbo() )
					.lpvResult( getOwner()->getLpvResult() )
					.llpvResult( getOwner()->getLlpvResult() )
					.vctFirstBounce( getOwner()->getFirstVctBounce() )
					.vctSecondaryBounce( getOwner()->getSecondaryVctBounce() );
				auto res = std::make_unique< ForwardRenderTechniquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
					, ForwardRenderTechniquePass::Type
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
		return result;
	}

	crg::FramePass & OpaqueRendering::doCreateDeferredOpaquePass( ProgressBar * progress
		, crg::FramePass const & lastPass
		, crg::FramePassArray const & previousPasses )
	{
		stepProgressBar( progress, "Creating opaque pass" );
		auto targetResult = getOwner()->getTargetResult();
		auto targetDepth = getOwner()->getTargetDepth();
		auto & result = m_graph.createPass( "NodesPass"
			, [this, progress, targetResult, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising opaque pass" );
				RenderTechniquePassDesc techniquePassDesc{ false, getOwner()->getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ getOwner()->getTargetExtent()
					, getOwner()->getMatrixUbo()
					, getOwner()->getSceneUbo()
					, getOwner()->getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true );
				auto nmlOccIt = std::next( framePass.images.begin(), 1u );
				auto colRghIt = std::next( nmlOccIt );
				auto spcMtlIt = std::next( colRghIt );
				auto emsTrnIt = std::next( spcMtlIt );
				renderPassDesc.implicitAction( nmlOccIt->view(), crg::RecordContext::clearAttachment( *nmlOccIt ) )
					.implicitAction( colRghIt->view(), crg::RecordContext::clearAttachment( *colRghIt ) )
					.implicitAction( spcMtlIt->view(), crg::RecordContext::clearAttachment( *spcMtlIt ) )
					.implicitAction( emsTrnIt->view(), crg::RecordContext::clearAttachment( *emsTrnIt ) );
				auto res = std::make_unique< OpaquePass >( getOwner()
					, framePass
					, context
					, runnableGraph
					, m_device
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
		auto & opaquePassResult = *m_opaquePassResult;
		result.addInOutDepthStencilView( targetDepth );
		result.addOutputColourView( opaquePassResult[DsTexture::eNmlOcc].targetViewId
			, getClearValue( DsTexture::eNmlOcc ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eColMtl].targetViewId
			, getClearValue( DsTexture::eColMtl ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eSpcRgh].targetViewId
			, getClearValue( DsTexture::eSpcRgh ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eEmsTrn].targetViewId
			, getClearValue( DsTexture::eEmsTrn ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eClrCot].targetViewId
			, getClearValue( DsTexture::eClrCot ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eCrTsIr].targetViewId
			, getClearValue( DsTexture::eCrTsIr ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eSheen].targetViewId
			, getClearValue( DsTexture::eSheen ) );

		CU_Require( !m_ssao );
		m_ssao = doCreateSsaoPass( progress, result, {} );
		return result;
	}

	bool OpaqueRendering::doIsOpaquePassEnabled()const
	{
		return m_opaquePass->isPassEnabled();
	}
}
