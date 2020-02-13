#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <CastorUtils/Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassDirectional::UboBindingPoint = 10u;
	uint32_t const ShadowMapPassDirectional::TextureSize = 1024u;
	String const ShadowMapPassDirectional::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassDirectional::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, uint32_t cascadeIndex )
		: ShadowMapPass{ engine, matrixUbo, culler, shadowMap }
	{
		castor::Logger::logTrace( "Created ShadowMapPassDirectional_" + castor::string::toString( cascadeIndex ) );
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	void ShadowMapPassDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		auto & myCamera = getCuller().getCamera();
		light.getDirectionalLight()->updateShadow( camera
			, myCamera
			, index
			, getCuller().getMinCastersZ() );
		m_farPlane = std::abs( light.getDirectionalLight()->getSplitDepth( index ) );
		doUpdate( queues );
	}

	void ShadowMapPassDirectional::updateDeviceDependent( uint32_t index )
	{
		if ( m_initialised )
		{
			auto & config = m_shadowConfig->getData();

			if ( m_farPlane != config.farPlane )
			{
				config.farPlane = m_farPlane;
				m_shadowConfig->upload();
			}

			auto & myCamera = getCuller().getCamera();
			m_matrixUbo.update( myCamera.getView()
				, myCamera.getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		auto & device = getCurrentRenderDevice( *this );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				ShadowMapDirectional::RawDepthFormat,
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
				ShadowMapDirectional::LinearDepthFormat,
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
				ShadowMapDirectional::VarianceFormat,
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
			, "ShadowMapPassDirectionalShadowConfigUbo" );

		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassDirectional::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		getCuller().getCamera().detach();
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		getCuller().compute();
		queues.emplace_back( m_renderQueue );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassDirectional::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ShadowMapPassDirectional::UboBindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
		return uboBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassDirectional::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassDirectional::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}
