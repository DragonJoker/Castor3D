#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassSpot::UboBindingPoint = 10u;
	uint32_t const ShadowMapPassSpot::TextureSize = 256u;
	String const ShadowMapPassSpot::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassSpot::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, matrixUbo, culler, shadowMap }
	{
		castor::Logger::logTrace( "Created ShadowMapPassSpot" );
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	void ShadowMapPassSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_shadowType = light.getShadowType();
		auto & myCamera = getCuller().getCamera();
		light.getSpotLight()->updateShadow( myCamera
			, index );
		auto & data = m_shadowConfig->getData();
		data.farPlane = light.getSpotLight()->getFarPlane();
		doUpdate( queues );
	}

	void ShadowMapPassSpot::updateDeviceDependent( uint32_t index )
	{
		if ( m_initialised )
		{
			m_shadowConfig->upload();
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
				ShadowMapSpot::RawDepthFormat,
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
				ShadowMapSpot::LinearDepthFormat,
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
				ShadowMapSpot::VarianceFormat,
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
		m_renderPass = device->createRenderPass( std::move( createInfo ) );

		m_shadowConfig = makeUniformBuffer< Configuration >( *getEngine()->getRenderSystem()
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, "ShadowMapPassSpotShadowConfigUbo" );

		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		getCuller().getCamera().detach();
	}

	void ShadowMapPassSpot::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassSpot::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & queues )
	{
		getCuller().compute();
		queues.emplace_back( m_renderQueue );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassSpot::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ShadowMapPassSpot::UboBindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
		return uboBindings;
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
