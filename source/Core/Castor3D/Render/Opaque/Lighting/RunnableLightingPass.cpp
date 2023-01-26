#include "Castor3D/Render/Opaque/Lighting/RunnableLightingPass.hpp"

#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	//*********************************************************************************************

	RunnableLightingPass::RunnableLightingPass( LightingPass const & lightingPass
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Scene const & scene
		, LightPassResult const & lpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, crg::ImageId const & targetColourResult )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doInitialise(); }
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT ); } )
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); }
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback([&lightingPass](){ return lightingPass.isEnabled(); } ) }
			, crg::ru::Config{ 1u, true }
				.implicitAction( lpResult[LpTexture::eDiffuse].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eDiffuse].targetViewId, getClearValue( LpTexture::eDiffuse ) ) )
				.implicitAction( lpResult[LpTexture::eSpecular].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eSpecular].targetViewId, getClearValue( LpTexture::eSpecular ) ) )
				.implicitAction( lpResult[LpTexture::eScattering].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eScattering].targetViewId, getClearValue( LpTexture::eScattering ) ) )
				.implicitAction( lpResult[LpTexture::eCoatingSpecular].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eCoatingSpecular].targetViewId, getClearValue( LpTexture::eCoatingSpecular ) ) )
				.implicitAction( lpResult[LpTexture::eSheen].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eSheen].targetViewId, getClearValue( LpTexture::eSheen ) ) ) }
		, m_device{ device }
		, m_scene{ scene }
		, m_lpResult{ lpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_targetColourResult{ targetColourResult }
	{
	}

	void RunnableLightingPass::clear()
	{
		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->clear();
		}
	}

	void RunnableLightingPass::enableLight( Camera const & camera
		, Light const & light )
	{
		if ( m_renderPasses.empty() )
		{
			m_pendingLights[&light] = &camera;
			return;
		}

		for ( auto lightingModelId : m_scene.getLightingModelsID() )
		{
			if ( m_scene.hasObjects( lightingModelId ) )
			{
				auto & pipeline = doFindPipeline( light
					, lightingModelId );
				pipeline.addLight( camera, light );
			}
		}
	}

	void RunnableLightingPass::disableLight( Camera const & camera
		, Light const & light )
	{
		if ( m_renderPasses.empty() )
		{
			auto it = m_pendingLights.find( &light );

			if ( it != m_pendingLights.end() )
			{
				m_pendingLights.erase( it );
			}

			return;
		}

		for ( auto lightingModelId : m_scene.getLightingModelsID() )
		{
			if ( m_scene.hasObjects( lightingModelId ) )
			{
				auto & pipeline = doFindPipeline( light
					, lightingModelId );
				pipeline.removeLight( camera, light );
			}
		}
	}

	void RunnableLightingPass::resetCommandBuffer()
	{
		crg::RunnablePass::resetCommandBuffer();
		crg::RunnablePass::reRecordCurrent();
	}

	bool RunnableLightingPass::hasEnabledLights()const
	{
		return getEnabledLightsCount() != 0u;
	}

	uint32_t RunnableLightingPass::getEnabledLightsCount()const
	{
		uint32_t result{};

		for ( auto & pipeline : m_pipelines )
		{
			result += pipeline.second->getLightCount();
		}

		return result + uint32_t( m_pendingLights.size() );
	}

	void RunnableLightingPass::doInitialise()
	{
		if ( m_renderPasses.empty() )
		{
			m_renderPasses.resize( 4u );
			m_renderPasses[getLightRenderPassIndex( false, LightType::eDirectional )] = doCreateRenderPass( false, false, m_lpResult );
			m_renderPasses[getLightRenderPassIndex( false, LightType::eMax )] = doCreateRenderPass( false, true, m_lpResult );
			m_renderPasses[getLightRenderPassIndex( true, LightType::eDirectional )] = doCreateRenderPass( true, false, m_lpResult );
			m_renderPasses[getLightRenderPassIndex( true, LightType::eMax )] = doCreateRenderPass( true, true, m_lpResult );
		}

		if ( m_stencilRenderPasses.empty() )
		{
			m_stencilRenderPasses.emplace_back( doCreateStencilRenderPass( false, m_lpResult ) );
			m_stencilRenderPasses.emplace_back( doCreateStencilRenderPass( true, m_lpResult ) );
		}

		for ( auto & pending : m_pendingLights )
		{
			enableLight( *pending.second, *pending.first );
		}

		m_pendingLights.clear();
	}

	void RunnableLightingPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		uint32_t pipelineIndex = 0u;

		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->recordInto( context, commandBuffer, pipelineIndex );
		}
	}

	LightRenderPass RunnableLightingPass::doCreateRenderPass( bool blend
		, bool hasStencil
		, LightPassResult const & lpResult )
	{
		castor::String name = ( hasStencil
				? castor::String{ cuT( "Stencil" ) }
				: castor::String{ cuT( "" ) } )
			+ ( blend
				? castor::String{ cuT( "Blend" ) }
				: castor::String{ cuT( "First" ) } );
		std::array< VkImageLayout, 6u > layouts{ ( ( blend || hasStencil ) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED ) };
		VkAttachmentLoadOp loadOp = blend
			? VK_ATTACHMENT_LOAD_OP_LOAD
			: VK_ATTACHMENT_LOAD_OP_CLEAR;
		VkAttachmentLoadOp stencilLoadOp = hasStencil
			? VK_ATTACHMENT_LOAD_OP_LOAD
			: VK_ATTACHMENT_LOAD_OP_DONT_CARE;

		ashes::VkAttachmentDescriptionArray attaches{
			{ 0u
				, lpResult[LpTexture::eDepth].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_STORE
				, stencilLoadOp
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[0u]
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eDiffuse].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[1u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eSpecular].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[2u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eScattering].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[3u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eCoatingSpecular].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[4u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eSheen].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[4u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } };
		ashes::VkAttachmentReferenceArray references{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 4u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 5u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };

		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, references
			, {}
			, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, {} } );

		ashes::VkSubpassDependencyArray dependencies{
			{ VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo rpCreateInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		LightRenderPass result{};
		result.renderPass = m_device->createRenderPass( "LightPass" + name
			, std::move( rpCreateInfo ) );

		std::vector< VkImageView > viewAttaches{ lpResult[LpTexture::eDepth].targetView
			, lpResult[LpTexture::eDiffuse].targetView
			, lpResult[LpTexture::eSpecular].targetView
			, lpResult[LpTexture::eScattering].targetView
			, lpResult[LpTexture::eCoatingSpecular].targetView
			, lpResult[LpTexture::eSheen].targetView };
		auto fbCreateInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
			, VkRenderPass{}
			, uint32_t{}
			, nullptr
			, uint32_t{}
			, uint32_t{}
			, uint32_t{} );
		fbCreateInfo.renderPass = *result.renderPass;
		fbCreateInfo.attachmentCount = uint32_t( viewAttaches.size() );
		fbCreateInfo.pAttachments = viewAttaches.data();
		fbCreateInfo.width = lpResult[LpTexture::eDiffuse].getExtent().width;
		fbCreateInfo.height = lpResult[LpTexture::eDiffuse].getExtent().height;
		fbCreateInfo.layers = 1u;
		result.framebuffer = result.renderPass->createFrameBuffer( "LightPass" + name
			, std::move( fbCreateInfo ) );

		result.clearValues.push_back( getClearValue( LpTexture::eDepth ) );
		result.clearValues.push_back( getClearValue( LpTexture::eDiffuse ) );
		result.clearValues.push_back( getClearValue( LpTexture::eSpecular ) );
		result.clearValues.push_back( getClearValue( LpTexture::eScattering ) );
		result.clearValues.push_back( getClearValue( LpTexture::eCoatingSpecular ) );
		result.clearValues.push_back( getClearValue( LpTexture::eSheen ) );

		result.attaches.push_back( { lpResult[LpTexture::eDepth].targetViewId
			, layouts[0]
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eDiffuse].targetViewId
			, layouts[1]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eSpecular].targetViewId
			, layouts[2]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eScattering].targetViewId
			, layouts[3]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eCoatingSpecular].targetViewId
			, layouts[4]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eSheen].targetViewId
			, layouts[5]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );

		return result;
	}

	LightRenderPass RunnableLightingPass::doCreateStencilRenderPass( bool blend
		, LightPassResult const & lpResult )
	{
		castor::String name = ( blend
				? castor::String{ cuT( "Blend" ) }
				: castor::String{ cuT( "First" ) } );
		std::array< VkImageLayout, 1u > layouts{ ( blend ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) };

		ashes::VkAttachmentDescriptionArray attaches{
			{ 0u
				, lpResult[LpTexture::eDepth].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_NONE_KHR
				, VK_ATTACHMENT_LOAD_OP_CLEAR
				, VK_ATTACHMENT_STORE_OP_STORE
				, layouts[0u]
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };
		ashes::VkAttachmentReferenceArray references;

		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, references
			, {}
			, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, {} } );

		ashes::VkSubpassDependencyArray dependencies{
			{ VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo rpCreateInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		LightRenderPass result{};
		result.renderPass = m_device->createRenderPass( "StencilPass" + name
			, std::move( rpCreateInfo ) );

		std::vector< VkImageView > viewAttaches{ lpResult[LpTexture::eDepth].targetView };
		auto fbCreateInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
			, VkRenderPass{}
			, uint32_t{}
			, nullptr
			, uint32_t{}
			, uint32_t{}
			, uint32_t{} );
		fbCreateInfo.renderPass = *result.renderPass;
		fbCreateInfo.attachmentCount = uint32_t( viewAttaches.size() );
		fbCreateInfo.pAttachments = viewAttaches.data();
		fbCreateInfo.width = lpResult[LpTexture::eDepth].getExtent().width;
		fbCreateInfo.height = lpResult[LpTexture::eDepth].getExtent().height;
		fbCreateInfo.layers = 1u;
		result.framebuffer = result.renderPass->createFrameBuffer( "StencilPass" + name
			, std::move( fbCreateInfo ) );

		result.clearValues.push_back( defaultClearDepthStencil );

		result.attaches.push_back( { lpResult[LpTexture::eDepth].targetViewId
			, layouts[0]
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );

		return result;
	}

	LightsPipeline & RunnableLightingPass::doFindPipeline( Light const & light
		, LightingModelID lightingModelId )
	{
		LightPipelineConfig config{ lightingModelId
			, m_scene.getFlags()
			, light };
		auto hash = config.makeHash();
		auto it = m_pipelines.find( hash );

		if ( it == m_pipelines.end() )
		{
			it = m_pipelines.emplace( hash
				, std::make_unique< LightsPipeline >( m_scene
					, m_pass
					, m_context
					, m_graph
					, m_device
					, config
					, m_lpResult
					, ( config.lightType == LightType::eDirectional
						? m_smDirectionalResult
						: ( config.lightType == LightType::ePoint
							? m_smPointResult
							: m_smSpotResult ) )
					, m_renderPasses
					, m_stencilRenderPasses
					, m_targetColourResult ) ).first;
		}

		return *it->second;
	}

	//*********************************************************************************************
}
