#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassSpot::TextureSize = 256u;

	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, uint32_t index
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ cuT( "ShadowMapPassSpot" ) + string::toString( index ), engine, matrixUbo, culler, shadowMap }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	bool ShadowMapPassSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		getCuller().compute();
		m_outOfDate = m_outOfDate
			|| getCuller().areAllChanged()
			|| getCuller().areCulledChanged();
		m_shadowType = light.getShadowType();
		auto & myCamera = getCuller().getCamera();
		light.getSpotLight()->updateShadow( myCamera
			, index );
		doUpdate( queues );
		return m_outOfDate;
	}

	void ShadowMapPassSpot::updateDeviceDependent( uint32_t index )
	{
		if ( m_initialised )
		{
			auto & myCamera = getCuller().getCamera();
			m_matrixUbo.update( myCamera.getView(), myCamera.getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	bool ShadowMapPassSpot::doInitialise( Size const & size )
	{
		auto & device = getCurrentRenderDevice( *this );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_shadowMap.getShadowPassResult()[SmTexture::eDepth].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				m_shadowMap.getShadowPassResult()[SmTexture::eLinearNormal].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				m_shadowMap.getShadowPassResult()[SmTexture::eVariance].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				m_shadowMap.getShadowPassResult()[SmTexture::ePosition].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				m_shadowMap.getShadowPassResult()[SmTexture::eFlux].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{
					{ 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					{ 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					{ 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					{ 4u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				},
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( m_name
			, std::move( createInfo ) );

		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_renderQueue.cleanup();
		getCuller().getCamera().detach();
	}

	void ShadowMapPassSpot::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
	}

	void ShadowMapPassSpot::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassSpot::doCreateUboBindings( PipelineFlags const & flags )const
	{
		m_initialised = true;
		return ashes::VkDescriptorSetLayoutBindingArray{};
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassSpot::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassSpot::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}
