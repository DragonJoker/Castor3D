#include "Castor3D/Render/Technique/Opaque/Lighting/StencilPipeline.hpp"

#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/MeshLightPass.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

namespace castor3d
{
	StencilPipeline::StencilPipeline( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightPipelineConfig const & config
		, std::vector< LightRenderPass > const & renderPasses
		, VkDescriptorSetLayout descriptorSetLayout )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, castor::string::snakeToCamelCase( getName( config.lightType ) )
			, MeshLightPass::getVertexShaderSource() }
		, m_stages{ makeShaderState( device, m_vertexShader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{ std::vector< crg::VkPipelineShaderStageCreateInfoArray >{ crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) }
				, crg::defaultV< crg::ProgramCreator >
				, std::vector< VkDescriptorSetLayout >{ descriptorSetLayout } }
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, 1u }
		, m_renderPasses{ renderPasses }
	{
		m_holder.initialise();
		doCreatePipeline();
	}

	void StencilPipeline::doCreatePipeline()
	{
		ashes::PipelineVertexInputStateCreateInfo vertexLayout = doCreateVertexLayout();
		crg::VkVertexInputAttributeDescriptionArray vertexAttribs;
		crg::VkVertexInputBindingDescriptionArray vertexBindings;
		auto iaState = makeVkStruct< VkPipelineInputAssemblyStateCreateInfo >( 0u
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, VK_FALSE );
		auto msState = makeVkStruct< VkPipelineMultisampleStateCreateInfo >( 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE );
		auto rsState = makeVkStruct< VkPipelineRasterizationStateCreateInfo >( 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, VkCullModeFlags( VK_CULL_MODE_NONE )
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f );
		VkPipelineVertexInputStateCreateInfo const & vsState = vertexLayout;
		ashes::PipelineDepthStencilStateCreateInfo depthStencil;
		depthStencil->depthTestEnable = VK_TRUE;
		depthStencil->depthWriteEnable = VK_FALSE;
		depthStencil->stencilTestEnable = VK_TRUE;
		depthStencil->back.compareMask = 0u;
		depthStencil->back.reference = 0u;
		depthStencil->back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencil->back.failOp = VK_STENCIL_OP_KEEP;
		depthStencil->back.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_WRAP;
		depthStencil->back.passOp = VK_STENCIL_OP_KEEP;
		depthStencil->front.compareMask = 0u;
		depthStencil->front.reference = 0u;
		depthStencil->front.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencil->front.failOp = VK_STENCIL_OP_KEEP;
		depthStencil->front.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_WRAP;
		depthStencil->front.passOp = VK_STENCIL_OP_KEEP;
		VkPipelineDepthStencilStateCreateInfo dsState = depthStencil;

		for ( uint32_t index = 0u; index < 2u; ++index )
		{
			auto viewportState = doCreateViewportState( *m_renderPasses[index].framebuffer );
			ashes::PipelineColorBlendStateCreateInfo colourBlend;
			auto & program = m_holder.getProgram( 0u );
			auto & pipeline = m_holder.getPipeline( 0u );
			VkPipelineColorBlendStateCreateInfo const & cbState = colourBlend;
			VkPipelineViewportStateCreateInfo const & vpState = viewportState;
			auto createInfo = makeVkStruct< VkGraphicsPipelineCreateInfo >( 0u
				, uint32_t( program.size() )
				, program.data()
				, &vsState
				, &iaState
				, nullptr
				, &vpState
				, &rsState
				, &msState
				, &dsState
				, &cbState
				, nullptr
				, m_holder.getPipelineLayout()
				, *m_renderPasses[index].renderPass
				, 0u
				, VK_NULL_HANDLE
				, 0 );
			auto res = m_holder.getContext().vkCreateGraphicsPipelines( m_holder.getContext().device
				, m_holder.getContext().cache
				, 1u
				, &createInfo
				, m_holder.getContext().allocator
				, &pipeline );
			crg::checkVkResult( res, m_holder.getPass().getGroupName() + " - Pipeline creation" );
			crgRegisterObject( m_holder.getContext(), m_holder.getPass().getGroupName(), pipeline );
		}
	}

	ashes::PipelineVertexInputStateCreateInfo StencilPipeline::doCreateVertexLayout()
	{
		return { 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( castor::Point3f )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } } };
	}

	ashes::PipelineViewportStateCreateInfo StencilPipeline::doCreateViewportState( ashes::FrameBuffer const & framebuffer )
	{
		auto dim = framebuffer.getDimensions();
		ashes::VkViewportArray viewports{ { 0.0f
			, 0.0f
			, float( dim.width )
			, float( dim.height )
			, 0.0f
			, 1.0f } };
		ashes::VkScissorArray scissors{ { {}, dim } };
		return { 0u
			, 1u
			, viewports
			, 1u
			, scissors };
	}
}
